//===- LoopNestTutorial.h - Loop Nest Tutorial Pass -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a small loop nest pass to be used to illustrate several
// aspects about writing a loop nest optimization. It was developed as part of
// the "How to utilize LoopNest pass" tutorial, presented at LLVM Devepeloper's
// Conference, 2021.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_SCALAR_LOOPNESTTUTORIAL_H
#define LLVM_TRANSFORMS_SCALAR_LOOPNESTTUTORIAL_H

#include "llvm/Transforms/Scalar/LoopPassManager.h"

namespace llvm {

// Creating a LoopNest pass is almost the same as creating a Loop pass.
class LoopNestTutorialPass : public PassInfoMixin<LoopNestTutorialPass> {
public:
  LoopNestTutorialPass() = default;

  // One difference: note that the run method takes a LoopNest object rather
  // than a Loop.
  PreservedAnalyses run(LoopNest &LN, LoopAnalysisManager &LAM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U);
};

} // end namespace llvm

#endif // LLVM_TRANSFORMS_SCALAR_LOOPNESTTUTORIAL_H
