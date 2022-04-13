// Copyright 2022 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "src/execution/clobber-registers.h"

#include "src/base/build_config.h"

#if V8_HOST_ARCH_ARM
#include "src/codegen/arm/register-arm.h"
#elif V8_HOST_ARCH_ARM64
#include "src/codegen/arm64/register-arm64.h"
#elif V8_HOST_ARCH_IA32
#include "src/codegen/ia32/register-ia32.h"
#elif V8_HOST_ARCH_X64
#include "src/codegen/x64/register-x64.h"
#endif

namespace v8 {
namespace internal {

#if V8_CC_MSVC
// msvc only support inline assembly on x86
#if V8_HOST_ARCH_IA32
#define CLOBBER_REGISTER(R) __asm xorps R, R

#endif

#else  // !V8_CC_MSVC

#if V8_HOST_ARCH_X64 || V8_HOST_ARCH_IA32
#define CLOBBER_REGISTER(R) \
  __asm__ volatile(         \
      "xorps "              \
      "%%" #R               \
      ","                   \
      "%%" #R ::            \
          :);

#elif V8_HOST_ARCH_ARM64
#define CLOBBER_REGISTER(R) __asm__ volatile("fmov " #R ",xzr" :::);

#endif  // V8_HOST_ARCH_X64 || V8_HOST_ARCH_IA32 || V8_HOST_ARCH_ARM64

#endif  // V8_CC_MSVC

double ClobberDoubleRegisters(double x1, double x2, double x3, double x4) {
  // clobber all double registers

#ifdef CLOBBER_REGISTER
  DOUBLE_REGISTERS(CLOBBER_REGISTER)
#undef CLOBBER_REGISTER
  return 0;

#else
  // TODO(v8:11798): This clobbers only subset of registers depending on
  // compiler, Rewrite this in assembly to really clobber all registers. GCC for
  // ia32 uses the FPU and does not touch XMM registers.
  return x1 * 1.01 + x2 * 2.02 + x3 * 3.03 + x4 * 4.04;
#endif  // CLOBBER_REGISTER
}

}  // namespace internal
}  // namespace v8