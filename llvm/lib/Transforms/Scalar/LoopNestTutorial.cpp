//===- LoopNestTutorial.cpp - Loop Nest Tutorial Pass ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This pass interchanges perfect loop nest with loop depth two.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Scalar/LoopNestTutorial.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

#define DEBUG_TYPE "loop-nest-tutorial"
static const char *VerboseDebug = DEBUG_TYPE "-verbose";

class LoopNestTutorial {
public:
  LoopNestTutorial(LoopInfo *LI, DominatorTree *DT) : LI(LI), DT(DT) {}

  bool run(LoopNest &LN) const {
    LLVM_DEBUG(dbgs() << "Entering LoopNestTutorial::run\n");

    // New: determine whether a loop nest is a candidate for our tranformation.
    if (!isCandidate(LN)) {
      LLVM_DEBUG(dbgs() << "Loop nest " << LN.getName()
                        << " is not a candidate for interchanging.\n");
      return false;
    }

    LLVM_DEBUG(dbgs() << "Loop nest " << LN.getName()
                      << " is a candidate for interchanging!\n");

    loopInterchange(LN);
    return true;
  }

private:
  /// Determines if \p LN is a candidate for loop interchange.
  /// Here we can use a number of handy utilities from the LoopNest class.
  bool isCandidate(const LoopNest &LN) const {
    assert(LN.areAllLoopsSimplifyForm() &&
           "Expecting all loops are in simplify form");

    if (!LN.areAllLoopsRotatedForm()) {
      LLVM_DEBUG(dbgs() << "Only rotated loop nests are considered.\n");
      return false;
    }

    if (!LN.getOutermostLoop().getExitBlock()) {
      LLVM_DEBUG(dbgs() << "Only loop nests with single exit outermost loop "
                           "are considered.\n");
      return false;
    }

    if (LN.getNestDepth() != 2) {
      LLVM_DEBUG(dbgs() << "Only loop nests with depth two are considered.\n");
      return false;
    }

    if (LN.getMaxPerfectDepth() != LN.getNestDepth()) {
      LLVM_DEBUG(dbgs() << "Only perfect loop nests are considered.\n");
      return false;
    }

    return true;
  }

  /// New: Perform loop interchange of the given 2-levels perfect loop nest.
  /// Note: for a proper LoopInterchange implementation please refer to the pass
  /// in LLVM.
  void loopInterchange(const LoopNest &LN) const {
    Loop &OuterLoop = LN.getOutermostLoop();
    Loop &InnerLoop = *LN.getInnermostLoop();

    BasicBlock &InnerHeader = *InnerLoop.getHeader();
    SplitBlock(&InnerHeader, InnerHeader.getFirstNonPHI(), DT, LI, nullptr,
               "loopbodyfirst");
    BasicBlock &LoopNestBodyLast = *InnerLoop.getLoopLatch();
    SplitBlock(&LoopNestBodyLast, LoopNestBodyLast.getTerminator(), DT, LI,
               nullptr, "latch.inner");

    LLVM_DEBUG({
      dbgs() << "After split blocks:\n";
      LN.getParent()->dump();
    });

    adjustLoopBranches(OuterLoop, InnerLoop);
    LLVM_DEBUG({
      dbgs() << "After adjust loop branches:\n";
      LN.getParent()->dump();
    });

    restructureLoops(OuterLoop, InnerLoop);
  }

  /// Adjust loop branches (to interchange the loops).
  /// Note: like for loop transformation passes we are responsible for
  /// maintaining the dominator tree up to date (so that other transformations
  /// can still use it).
  void adjustLoopBranches(Loop &OuterLoop, Loop &InnerLoop) const {
    BasicBlock &OuterPreheader = *OuterLoop.getLoopPreheader();
    BasicBlock &OuterHeader = *OuterLoop.getHeader();
    BasicBlock &OuterLatch = *OuterLoop.getLoopLatch();
    BasicBlock &OuterExit = *OuterLoop.getExitBlock();
    BasicBlock &InnerPreheader = *InnerLoop.getLoopPreheader();
    BasicBlock &InnerHeader = *InnerLoop.getHeader();
    BasicBlock &InnerLatch = *InnerLoop.getLoopLatch();
    BasicBlock &InnerExit = *InnerLoop.getExitBlock();
    BasicBlock &LoopNestBodyFirst = *InnerHeader.getUniqueSuccessor();
    BasicBlock &LoopNestBodyLast = *InnerLatch.getUniquePredecessor();

    DEBUG_WITH_TYPE(VerboseDebug, {
      dbgs() << "OuterPreheader " << OuterPreheader << "\n";
      dbgs() << "OuterHeader " << OuterHeader << "\n";
      dbgs() << "OuterLatch " << OuterLatch << "\n";
      dbgs() << "OuterExit " << OuterExit << "\n";
      dbgs() << "InnerPreheader " << InnerPreheader << "\n";
      dbgs() << "InnerHeader " << InnerHeader << "\n";
      dbgs() << "LoopNestBodyFirst " << LoopNestBodyFirst << "\n";
      dbgs() << "LoopNestBodyLast " << LoopNestBodyLast << "\n";
      dbgs() << "InnerLatch " << InnerLatch << "\n";
      dbgs() << "InnerExit " << InnerExit << "\n";
    });

    SmallVector<DominatorTree::UpdateType, 4> DTUpdates;
    auto UpdateSuccessor = [&DTUpdates](BasicBlock &BB, BasicBlock &OldSucc,
                                        BasicBlock &NewSucc) {
      BranchInst *BI = dyn_cast<BranchInst>(BB.getTerminator());
      assert(BI && "Expecting BB to be terminated by a BranchInst");
      for (unsigned I = 0; I < BI->getNumSuccessors(); ++I)
        if (&OldSucc == BI->getSuccessor(I))
          BI->setSuccessor(I, &NewSucc);
      DTUpdates.push_back({DominatorTree::UpdateKind::Delete, &BB, &OldSucc});
      DTUpdates.push_back({DominatorTree::UpdateKind::Insert, &BB, &NewSucc});
    };

    UpdateSuccessor(OuterPreheader, OuterHeader, InnerHeader);
    UpdateSuccessor(InnerHeader, LoopNestBodyFirst, OuterHeader);
    UpdateSuccessor(InnerPreheader, InnerHeader, LoopNestBodyFirst);
    OuterHeader.replacePhiUsesWith(&OuterPreheader, &InnerHeader);
    InnerHeader.replacePhiUsesWith(&InnerPreheader, &OuterPreheader);

    UpdateSuccessor(LoopNestBodyLast, InnerLatch, InnerExit);
    UpdateSuccessor(OuterLatch, OuterExit, InnerLatch);
    UpdateSuccessor(InnerLatch, InnerExit, OuterExit);
    InnerExit.replacePhiUsesWith(&InnerLatch, &LoopNestBodyLast);
    OuterExit.replacePhiUsesWith(&OuterLatch, &InnerLatch);
    DT->applyUpdates(DTUpdates);
  }

  /// Update LoopInfo, after interchanging. OuterLoop is the original outer loop
  /// and InnerLoop is the original inner loop.
  /// Note: the LoopInfo data structure needs to be updated to reflect the
  /// changes that this transformation has made.
  void restructureLoops(Loop &OuterLoop, Loop &InnerLoop) const {
    OuterLoop.removeChildLoop(&InnerLoop);
    LI->changeTopLevelLoop(&OuterLoop, &InnerLoop);
    InnerLoop.addChildLoop(&OuterLoop);
    for (BasicBlock *BB : OuterLoop.blocks())
      if (LI->getLoopFor(BB) == &OuterLoop)
        InnerLoop.addBlockEntry(BB);

    BasicBlock &InnerHeader = *InnerLoop.getHeader();
    BasicBlock &InnerLatch = *InnerLoop.getLoopLatch();
    OuterLoop.removeBlockFromLoop(&InnerHeader);
    OuterLoop.removeBlockFromLoop(&InnerLatch);
    for (BasicBlock *BB : OuterLoop.blocks())
      if (LI->getLoopFor(BB) == &InnerLoop)
        LI->changeLoopFor(BB, &OuterLoop);
    DEBUG_WITH_TYPE(VerboseDebug, {
      dbgs() << "InnerLoop ";
      InnerLoop.dump();
      dbgs() << "OuterLoop ";
      OuterLoop.dump();
    });
  }

  LoopInfo *LI = nullptr;
  DominatorTree *DT = nullptr;
};

static void verify(const LoopInfo &LI, const DominatorTree &DT) {
  Function &F = *(*LI.begin())->getHeader()->getParent();
  assert(!verifyFunction(F, &errs()) && "Incorrect Function");
  assert(DT.verify() && "Incorrect DT");
  LI.verify(DT);
}

PreservedAnalyses LoopNestTutorialPass::run(LoopNest &LN,
                                            LoopAnalysisManager &LAM,
                                            LoopStandardAnalysisResults &AR,
                                            LPMUpdater &U) {

  LLVM_DEBUG(dbgs() << "Entering LoopNestTutorialPass::run\n");

  // auto *DT = &AM.getResult<DominatorTreeAnalysis>(F);
  // auto *LI = &AM.getResult<LoopAnalysis>(F);
  // auto *SE = &AM.getResult<ScalarEvolutionAnalysis>(F);
  // bool Simplified = false;
  // for (const auto *L : *LI) {
  //   Simplified |= simplifyLoop(L, DT, LI, SE, nullptr, nullptr, true);
  //   LoopNest LN(*L, *SE);
  // }
  LLVM_DEBUG(dbgs() << "LoopNest: " << LN << "\n");
  bool Changed = LoopNestTutorial(&AR.LI, &AR.DT).run(LN);

  if (!Changed)
    return PreservedAnalyses::all();

  verify(AR.LI, AR.DT);

  // Similar to a loop pass: need to preserve the analysis cached in
  // LoopStandardAnalysisResults.
  return getLoopPassPreservedAnalyses();
}
