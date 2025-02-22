; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=thumbv8.1m.main-none-none-eabi -mattr=+mve.fp -verify-machineinstrs %s -o - | FileCheck %s

define arm_aapcs_vfpcc <4 x i32> @cmp_sel_C1_or_C2_vec(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: cmp_sel_C1_or_C2_vec:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    adr r0, .LCPI0_0
; CHECK-NEXT:    adr r1, .LCPI0_1
; CHECK-NEXT:    vcmp.i32 eq, q0, q1
; CHECK-NEXT:    vldrw.u32 q0, [r1]
; CHECK-NEXT:    vldrw.u32 q1, [r0]
; CHECK-NEXT:    vpsel q0, q1, q0
; CHECK-NEXT:    bx lr
; CHECK-NEXT:    .p2align 4
; CHECK-NEXT:  @ %bb.1:
; CHECK-NEXT:  .LCPI0_0:
; CHECK-NEXT:    .long 3000 @ 0xbb8
; CHECK-NEXT:    .long 1 @ 0x1
; CHECK-NEXT:    .long 4294967295 @ 0xffffffff
; CHECK-NEXT:    .long 0 @ 0x0
; CHECK-NEXT:  .LCPI0_1:
; CHECK-NEXT:    .long 42 @ 0x2a
; CHECK-NEXT:    .long 0 @ 0x0
; CHECK-NEXT:    .long 4294967294 @ 0xfffffffe
; CHECK-NEXT:    .long 4294967295 @ 0xffffffff
  %cond = icmp eq <4 x i32> %x, %y
  %add = select <4 x i1> %cond, <4 x i32> <i32 3000, i32 1, i32 -1, i32 0>, <4 x i32> <i32 42, i32 0, i32 -2, i32 -1>
  ret <4 x i32> %add
}

define arm_aapcs_vfpcc <4 x i32> @cmp_sel_Cplus1_or_C_vec(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: cmp_sel_Cplus1_or_C_vec:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    adr r0, .LCPI1_0
; CHECK-NEXT:    adr r1, .LCPI1_1
; CHECK-NEXT:    vcmp.i32 eq, q0, q1
; CHECK-NEXT:    vldrw.u32 q0, [r1]
; CHECK-NEXT:    vldrw.u32 q1, [r0]
; CHECK-NEXT:    vpsel q0, q1, q0
; CHECK-NEXT:    bx lr
; CHECK-NEXT:    .p2align 4
; CHECK-NEXT:  @ %bb.1:
; CHECK-NEXT:  .LCPI1_0:
; CHECK-NEXT:    .long 43 @ 0x2b
; CHECK-NEXT:    .long 1 @ 0x1
; CHECK-NEXT:    .long 4294967295 @ 0xffffffff
; CHECK-NEXT:    .long 0 @ 0x0
; CHECK-NEXT:  .LCPI1_1:
; CHECK-NEXT:    .long 42 @ 0x2a
; CHECK-NEXT:    .long 0 @ 0x0
; CHECK-NEXT:    .long 4294967294 @ 0xfffffffe
; CHECK-NEXT:    .long 4294967295 @ 0xffffffff
  %cond = icmp eq <4 x i32> %x, %y
  %add = select <4 x i1> %cond, <4 x i32> <i32 43, i32 1, i32 -1, i32 0>, <4 x i32> <i32 42, i32 0, i32 -2, i32 -1>
  ret <4 x i32> %add
}

define arm_aapcs_vfpcc <4 x i32> @cmp_sel_Cminus1_or_C_vec(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: cmp_sel_Cminus1_or_C_vec:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    adr r0, .LCPI2_0
; CHECK-NEXT:    adr r1, .LCPI2_1
; CHECK-NEXT:    vcmp.i32 eq, q0, q1
; CHECK-NEXT:    vldrw.u32 q0, [r1]
; CHECK-NEXT:    vldrw.u32 q1, [r0]
; CHECK-NEXT:    vpsel q0, q1, q0
; CHECK-NEXT:    bx lr
; CHECK-NEXT:    .p2align 4
; CHECK-NEXT:  @ %bb.1:
; CHECK-NEXT:  .LCPI2_0:
; CHECK-NEXT:    .long 43 @ 0x2b
; CHECK-NEXT:    .long 1 @ 0x1
; CHECK-NEXT:    .long 4294967295 @ 0xffffffff
; CHECK-NEXT:    .long 0 @ 0x0
; CHECK-NEXT:  .LCPI2_1:
; CHECK-NEXT:    .long 44 @ 0x2c
; CHECK-NEXT:    .long 2 @ 0x2
; CHECK-NEXT:    .long 0 @ 0x0
; CHECK-NEXT:    .long 1 @ 0x1
  %cond = icmp eq <4 x i32> %x, %y
  %add = select <4 x i1> %cond, <4 x i32> <i32 43, i32 1, i32 -1, i32 0>, <4 x i32> <i32 44, i32 2, i32 0, i32 1>
  ret <4 x i32> %add
}

define arm_aapcs_vfpcc <4 x i32> @cmp_sel_minus1_or_0_vec(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: cmp_sel_minus1_or_0_vec:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    vmov.i32 q2, #0x0
; CHECK-NEXT:    vmov.i8 q3, #0xff
; CHECK-NEXT:    vcmp.i32 eq, q0, q1
; CHECK-NEXT:    vpsel q0, q3, q2
; CHECK-NEXT:    bx lr
  %cond = icmp eq <4 x i32> %x, %y
  %add = select <4 x i1> %cond, <4 x i32> <i32 -1, i32 -1, i32 -1, i32 -1>, <4 x i32> <i32 0, i32 0, i32 0, i32 0>
  ret <4 x i32> %add
}

define arm_aapcs_vfpcc <4 x i32> @cmp_sel_0_or_minus1_vec(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: cmp_sel_0_or_minus1_vec:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    vmov.i8 q2, #0xff
; CHECK-NEXT:    vmov.i32 q3, #0x0
; CHECK-NEXT:    vcmp.i32 eq, q0, q1
; CHECK-NEXT:    vpsel q0, q3, q2
; CHECK-NEXT:    bx lr
  %cond = icmp eq <4 x i32> %x, %y
  %add = select <4 x i1> %cond, <4 x i32> <i32 0, i32 0, i32 0, i32 0>, <4 x i32> <i32 -1, i32 -1, i32 -1, i32 -1>
  ret <4 x i32> %add
}

define arm_aapcs_vfpcc <4 x i32> @cmp_sel_1_or_0_vec(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: cmp_sel_1_or_0_vec:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    vmov.i32 q2, #0x0
; CHECK-NEXT:    vmov.i32 q3, #0x1
; CHECK-NEXT:    vcmp.i32 eq, q0, q1
; CHECK-NEXT:    vpsel q0, q3, q2
; CHECK-NEXT:    bx lr
  %cond = icmp eq <4 x i32> %x, %y
  %add = select <4 x i1> %cond, <4 x i32> <i32 1, i32 1, i32 1, i32 1>, <4 x i32> <i32 0, i32 0, i32 0, i32 0>
  ret <4 x i32> %add
}

define arm_aapcs_vfpcc <4 x i32> @cmp_sel_0_or_1_vec(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: cmp_sel_0_or_1_vec:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    vmov.i32 q2, #0x1
; CHECK-NEXT:    vmov.i32 q3, #0x0
; CHECK-NEXT:    vcmp.i32 eq, q0, q1
; CHECK-NEXT:    vpsel q0, q3, q2
; CHECK-NEXT:    bx lr
  %cond = icmp eq <4 x i32> %x, %y
  %add = select <4 x i1> %cond, <4 x i32> <i32 0, i32 0, i32 0, i32 0>, <4 x i32> <i32 1, i32 1, i32 1, i32 1>
  ret <4 x i32> %add
}

define arm_aapcs_vfpcc <16 x i8> @signbit_mask_v16i8(<16 x i8> %a, <16 x i8> %b) {
; CHECK-LABEL: signbit_mask_v16i8:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    vshr.s8 q0, q0, #7
; CHECK-NEXT:    vand q0, q0, q1
; CHECK-NEXT:    bx lr
  %cond = icmp slt <16 x i8> %a, zeroinitializer
  %r = select <16 x i1> %cond, <16 x i8> %b, <16 x i8> zeroinitializer
  ret <16 x i8> %r
}

define arm_aapcs_vfpcc <8 x i16> @signbit_mask_v8i16(<8 x i16> %a, <8 x i16> %b) {
; CHECK-LABEL: signbit_mask_v8i16:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    vshr.s16 q0, q0, #15
; CHECK-NEXT:    vand q0, q0, q1
; CHECK-NEXT:    bx lr
  %cond = icmp slt <8 x i16> %a, zeroinitializer
  %r = select <8 x i1> %cond, <8 x i16> %b, <8 x i16> zeroinitializer
  ret <8 x i16> %r
}

define arm_aapcs_vfpcc <4 x i32> @signbit_mask_v4i32(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: signbit_mask_v4i32:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    vshr.s32 q0, q0, #31
; CHECK-NEXT:    vand q0, q0, q1
; CHECK-NEXT:    bx lr
  %cond = icmp slt <4 x i32> %a, zeroinitializer
  %r = select <4 x i1> %cond, <4 x i32> %b, <4 x i32> zeroinitializer
  ret <4 x i32> %r
}

define arm_aapcs_vfpcc <2 x i64> @signbit_mask_v2i64(<2 x i64> %a, <2 x i64> %b) {
; CHECK-LABEL: signbit_mask_v2i64:
; CHECK:       @ %bb.0:
; CHECK-NEXT:    vmov r0, s3
; CHECK-NEXT:    vmov r1, s1
; CHECK-NEXT:    asrs r0, r0, #31
; CHECK-NEXT:    asrs r1, r1, #31
; CHECK-NEXT:    vmov q0[2], q0[0], r1, r0
; CHECK-NEXT:    vmov q0[3], q0[1], r1, r0
; CHECK-NEXT:    vand q0, q1, q0
; CHECK-NEXT:    bx lr
  %cond = icmp slt <2 x i64> %a, zeroinitializer
  %r = select <2 x i1> %cond, <2 x i64> %b, <2 x i64> zeroinitializer
  ret <2 x i64> %r
}

