/*===-- include/flang/Common/api-attrs.h ---------------------------*- C -*-=//
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===------------------------------------------------------------------------===
 */

/*
 * The file defines a set macros that can be used to apply
 * different attributes/pragmas to functions/variables
 * declared/defined/used in Flang runtime library.
 */

#ifndef FORTRAN_RUNTIME_API_ATTRS_H_
#define FORTRAN_RUNTIME_API_ATTRS_H_

/*
 * RT_EXT_API_GROUP_BEGIN/END pair is placed around definitions
 * of functions exported by Flang runtime library. They are the entry
 * points that are referenced in the Flang generated code.
 * The macros may be expanded into any construct that is valid to appear
 * at C++ module scope.
 */
#ifndef RT_EXT_API_GROUP_BEGIN
#if defined(OMP_NOHOST_BUILD)
#define RT_EXT_API_GROUP_BEGIN \
  _Pragma("omp begin declare target device_type(nohost)")
#elif defined(OMP_OFFLOAD_BUILD)
#define RT_EXT_API_GROUP_BEGIN _Pragma("omp declare target")
#else
#define RT_EXT_API_GROUP_BEGIN
#endif
#endif /* !defined(RT_EXT_API_GROUP_BEGIN) */

#ifndef RT_EXT_API_GROUP_END
#if defined(OMP_NOHOST_BUILD) || defined(OMP_OFFLOAD_BUILD)
#define RT_EXT_API_GROUP_END _Pragma("omp end declare target")
#else
#define RT_EXT_API_GROUP_END
#endif
#endif /* !defined(RT_EXT_API_GROUP_END) */

/*
 * RT_OFFLOAD_API_GROUP_BEGIN/END pair is placed around definitions
 * of functions that can be referenced in other modules of Flang
 * runtime. For OpenMP offload, these functions are made "declare target"
 * making sure they are compiled for the target even though direct
 * references to them from other "declare target" functions may not
 * be seen. Host-only functions should not be put in between these
 * two macros.
 */
#define RT_OFFLOAD_API_GROUP_BEGIN RT_EXT_API_GROUP_BEGIN
#define RT_OFFLOAD_API_GROUP_END RT_EXT_API_GROUP_END

/*
 * RT_OFFLOAD_VAR_GROUP_BEGIN/END pair is placed around definitions
 * of variables (e.g. globals or static class members) that can be
 * referenced in functions marked with RT_OFFLOAD_API_GROUP_BEGIN/END.
 * For OpenMP offload, these variables are made "declare target".
 */
#define RT_OFFLOAD_VAR_GROUP_BEGIN RT_EXT_API_GROUP_BEGIN
#define RT_OFFLOAD_VAR_GROUP_END RT_EXT_API_GROUP_END

/*
 * RT_VAR_GROUP_BEGIN/END pair is placed around definitions
 * of module scope variables referenced by Flang runtime (directly
 * or indirectly).
 * The macros may be expanded into any construct that is valid to appear
 * at C++ module scope.
 */
#ifndef RT_VAR_GROUP_BEGIN
#define RT_VAR_GROUP_BEGIN RT_EXT_API_GROUP_BEGIN
#endif /* !defined(RT_VAR_GROUP_BEGIN) */

#ifndef RT_VAR_GROUP_END
#define RT_VAR_GROUP_END RT_EXT_API_GROUP_END
#endif /* !defined(RT_VAR_GROUP_END) */

/*
 * Each non-exported function used by Flang runtime (e.g. via
 * calling it or taking its address, etc.) is marked with
 * RT_API_ATTRS. The macros is placed at both declaration and
 * definition of such a function.
 * The macros may be expanded into a construct that is valid
 * to appear as part of a C++ decl-specifier.
 */
#ifndef RT_API_ATTRS
#if defined(__CUDACC__) || defined(__CUDA__)
#define RT_API_ATTRS __host__ __device__
#else
#define RT_API_ATTRS
#endif
#endif /* !defined(RT_API_ATTRS) */

/*
 * Each const/constexpr module scope variable referenced by Flang runtime
 * (directly or indirectly) is marked with RT_CONST_VAR_ATTRS.
 * The macros is placed at both declaration and definition of such a variable.
 * The macros may be expanded into a construct that is valid
 * to appear as part of a C++ decl-specifier.
 */
#ifndef RT_CONST_VAR_ATTRS
#if (defined(__CUDACC__) || defined(__CUDA__)) && defined(__CUDA_ARCH__)
#define RT_CONST_VAR_ATTRS __constant__
#else
#define RT_CONST_VAR_ATTRS
#endif
#endif /* !defined(RT_CONST_VAR_ATTRS) */

/*
 * RT_VAR_ATTRS is marking non-const/constexpr module scope variables
 * referenced by Flang runtime.
 */
#ifndef RT_VAR_ATTRS
#if (defined(__CUDACC__) || defined(__CUDA__)) && defined(__CUDA_ARCH__)
#define RT_VAR_ATTRS __device__
#else
#define RT_VAR_ATTRS
#endif
#endif /* !defined(RT_VAR_ATTRS) */

/*
 * RT_DEVICE_COMPILATION is defined for any device compilation.
 * Note that it can only be used reliably with compilers that perform
 * separate host and device compilations.
 */
#if ((defined(__CUDACC__) || defined(__CUDA__)) && defined(__CUDA_ARCH__)) || \
    (defined(_OPENMP) && (defined(__AMDGCN__) || defined(__NVPTX__)))
#define RT_DEVICE_COMPILATION 1
#else
#undef RT_DEVICE_COMPILATION
#endif

/*
 * Recurrence in the call graph prevents computing minimal stack size
 * required for a kernel execution. This macro can be used to disable
 * some F18 runtime functionality that is implemented using recurrent
 * function calls or to use alternative implementation.
 */
#if (defined(__CUDACC__) || defined(__CUDA__)) && defined(__CUDA_ARCH__)
#define RT_DEVICE_AVOID_RECURSION 1
#else
#undef RT_DEVICE_AVOID_RECURSION
#endif

#if defined(__CUDACC__)
#define RT_DIAG_PUSH _Pragma("nv_diagnostic push")
#define RT_DIAG_POP _Pragma("nv_diagnostic pop")
#define RT_DIAG_DISABLE_CALL_HOST_FROM_DEVICE_WARN \
  _Pragma("nv_diag_suppress 20011") _Pragma("nv_diag_suppress 20014")
#else /* !defined(__CUDACC__) */
#define RT_DIAG_PUSH
#define RT_DIAG_POP
#define RT_DIAG_DISABLE_CALL_HOST_FROM_DEVICE_WARN
#endif /* !defined(__CUDACC__) */

/*
 * RT_DEVICE_NOINLINE may be used for non-performance critical
 * functions that should not be inlined to minimize the amount
 * of code that needs to be processed by the device compiler's
 * optimizer.
 */
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
#if __has_attribute(noinline)
#define RT_NOINLINE_ATTR __attribute__((noinline))
#else
#define RT_NOINLINE_ATTR
#endif
#if (defined(__CUDACC__) || defined(__CUDA__)) && defined(__CUDA_ARCH__)
#define RT_DEVICE_NOINLINE RT_NOINLINE_ATTR
#define RT_DEVICE_NOINLINE_HOST_INLINE
#else
#define RT_DEVICE_NOINLINE
#define RT_DEVICE_NOINLINE_HOST_INLINE inline
#endif

/* RT_OPTNONE_ATTR allows disabling optimizations per function. */
#if __has_attribute(optimize)
/* GCC style. */
#define RT_OPTNONE_ATTR __attribute__((optimize("O0")))
#elif __has_attribute(optnone)
/* Clang style. */
#define RT_OPTNONE_ATTR __attribute__((optnone))
#else
#define RT_OPTNONE_ATTR
#endif

/* Detect system endianness if it was not explicitly set. */
#if !defined(FLANG_LITTLE_ENDIAN) && !defined(FLANG_BIG_ENDIAN)

/* We always assume Windows is little endian, otherwise use the GCC compatible
 * flags. */
#if defined(_MSC_VER) || defined(_WIN32)
#define FLANG_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define FLANG_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define FLANG_BIG_ENDIAN 1
#else
#error "Unknown or unsupported endianness."
#endif

#endif /* !defined(FLANG_LITTLE_ENDIAN) && !defined(FLANG_BIG_ENDIAN) */

#endif /* !FORTRAN_RUNTIME_API_ATTRS_H_ */
