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

using namespace llvm;

#define DEBUG_TYPE "loop-nest-tutorial"

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

    (void)LI;
    (void)DT;
    return false;
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
  LLVM_DEBUG(dbgs() << "LoopNest: " << LN << "\n");
  bool Changed = LoopNestTutorial(&AR.LI, &AR.DT).run(LN);
  // }

  if (!Changed)
    return PreservedAnalyses::all();

  verify(AR.LI, AR.DT);

  return getLoopPassPreservedAnalyses();
}
