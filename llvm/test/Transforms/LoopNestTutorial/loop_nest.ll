; RUN: opt -passes='loop-nest-tutorial' -debug-only=loop-nest-tutorial -S < %s 2>&1 | FileCheck %s
; REQUIRES: asserts

; CHECK: Entering LoopNestTutorialPass::run
; CHECK-NEXT: LoopNest: IsPerfect=true, Depth=2, OutermostLoop: header.outer, Loops: ( header.outer header.inner )
; CHECK-NEXT: Entering LoopNestTutorial::run
; CHECK-NEXT: Loop nest header.outer is a candidate for interchanging!

define void @perfectloopnest() {
preheader.outer:
  br label %header.outer

header.outer:
  %i = phi i32 [ 0, %preheader.outer ], [ %inc.outer, %latch.outer ]
  br label %header.inner

header.inner:
  %j = phi i32 [ 0, %header.outer ], [ %inc.inner, %header.inner ]
  call void @foo(i32 %i, i32 %j)
  %inc.inner = add nsw i32 %j, 1
  %cmp.inner = icmp slt i32 %inc.inner, 100
  br i1 %cmp.inner, label %header.inner, label %latch.outer

latch.outer:
  %inc.outer = add nsw i32 %i, 1
  %cmp.outer = icmp slt i32 %inc.outer, 100
  br i1 %cmp.outer, label %header.outer, label %exit.outer

exit.outer:
  ret void
}

declare void @foo(i32, i32)
