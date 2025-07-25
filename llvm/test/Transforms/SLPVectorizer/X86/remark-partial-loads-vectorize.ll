; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 3
; RUN: opt < %s -passes=slp-vectorizer -S -mtriple=x86_64-unknown-linux-gnu -mcpu=skylake-avx512 -pass-remarks-output=%t | FileCheck %s
; RUN: FileCheck --input-file=%t --check-prefix=YAML %s

; YAML-LABEL: --- !Passed
; YAML-NEXT:  Pass:            slp-vectorizer
; YAML-NEXT:  Name:            VectorizedList
; YAML-NEXT:  Function:        test
; YAML-NEXT:  Args:
; YAML-NEXT:    - String:          'SLP vectorized with cost '
; YAML-NEXT:    - Cost:            '-4'
; YAML-NEXT:    - String:          ' and with tree size '
; YAML-NEXT:    - TreeSize:        '5'

define <4 x float> @test(ptr %x, float %v, float %a) {
; CHECK-LABEL: define <4 x float> @test(
; CHECK-SAME: ptr [[X:%.*]], float [[V:%.*]], float [[A:%.*]]) #[[ATTR0:[0-9]+]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <2 x float>, ptr [[X]], align 4
; CHECK-NEXT:    [[TMP2:%.*]] = insertelement <4 x float> poison, float [[A]], i32 0
; CHECK-NEXT:    [[TMP3:%.*]] = shufflevector <4 x float> [[TMP2]], <4 x float> poison, <4 x i32> zeroinitializer
; CHECK-NEXT:    [[TMP4:%.*]] = insertelement <4 x float> poison, float [[V]], i32 0
; CHECK-NEXT:    [[TMP5:%.*]] = shufflevector <4 x float> [[TMP4]], <4 x float> poison, <4 x i32> <i32 0, i32 0, i32 poison, i32 poison>
; CHECK-NEXT:    [[TMP6:%.*]] = shufflevector <2 x float> [[TMP1]], <2 x float> poison, <4 x i32> <i32 0, i32 1, i32 poison, i32 poison>
; CHECK-NEXT:    [[TMP7:%.*]] = shufflevector <4 x float> [[TMP5]], <4 x float> [[TMP6]], <4 x i32> <i32 0, i32 1, i32 4, i32 5>
; CHECK-NEXT:    [[TMP8:%.*]] = fadd <4 x float> [[TMP3]], [[TMP7]]
; CHECK-NEXT:    ret <4 x float> [[TMP8]]
;
  %gep1 = getelementptr inbounds <4 x float>, ptr %x, i64 0, i64 1
  %x0 = load float, ptr %x, align 4
  %x1 = load float, ptr %gep1, align 4
  %add1 = fadd float %a, %v
  %add2 = fadd float %a, %v
  %add3 = fadd float %a, %x0
  %add4 = fadd float %a, %x1
  %i0 = insertelement <4 x float> undef, float %add1, i32 0
  %i1 = insertelement <4 x float> %i0, float %add2, i32 1
  %i2 = insertelement <4 x float> %i1, float %add3, i32 2
  %i3 = insertelement <4 x float> %i2, float %add4, i32 3
  ret <4 x float> %i3
}
