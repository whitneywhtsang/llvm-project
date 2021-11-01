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
    LLVM_DEBUG(
        dbgs() << "TODO: Need to check if LoopNest is a valid candidate\n");
    (void)LI;
    (void)DT;
    return false;
  }

private:
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
