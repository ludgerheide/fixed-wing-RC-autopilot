/*
 * File: Flugregler_private.h
 *
 * Code generated for Simulink model 'Flugregler'.
 *
 * Model version                  : 1.14
 * Simulink Coder version         : 8.8 (R2015a) 09-Feb-2015
 * C/C++ source code generated on : Wed Mar 09 17:05:53 2016
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 *    3. ROM efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Flugregler_private_h_
#define RTW_HEADER_Flugregler_private_h_
#include "rtwtypes.h"
#include "multiword_types.h"
#ifndef UCHAR_MAX
#include <limits.h>
#endif

#if ( UCHAR_MAX != (0xFFU) ) || ( SCHAR_MAX != (0x7F) )
#error Code was generated for compiler with different sized uchar/char. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( USHRT_MAX != (0xFFFFU) ) || ( SHRT_MAX != (0x7FFF) )
#error Code was generated for compiler with different sized ushort/short. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( UINT_MAX != (0xFFFFU) ) || ( INT_MAX != (0x7FFF) )
#error Code was generated for compiler with different sized uint/int. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( ULONG_MAX != (0xFFFFFFFFUL) ) || ( LONG_MAX != (0x7FFFFFFFL) )
#error Code was generated for compiler with different sized ulong/long. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

extern real_T rt_roundd(real_T u);
extern real_T rt_modd(real_T u0, real_T u1);
extern real_T sMultiWord2Double(const uint32_T u1[], int16_T n1, int16_T e1);
extern void sMultiWordMul(const uint32_T u1[], int16_T n1, const uint32_T u2[],
    int16_T n2, uint32_T y[], int16_T n);
extern void Flugregler_Flugregler_Init(DW_Flugregler_Flugregler_T *localDW,
    P_Flugregler_Flugregler_T *localP);
extern void Flugregler_Flugregler(real32_T rtu_currentAttitude, real32_T
    rtu_currentAttitude_e, int32_T rtu_barometricAltitude, int8_T
    rtu_targetHeading, boolean_T rtu_targetHeading_h, uint16_T
    rtu_targetHeading_m, int32_T rtu_targetPitch, int8_T rtu_targetPitch_h,
    boolean_T rtu_targetPitch_e, real32_T rtu_gyro, B_Flugregler_Flugregler_T
    *localB, DW_Flugregler_Flugregler_T *localDW, P_Flugregler_Flugregler_T
    *localP);

#endif                                 /* RTW_HEADER_Flugregler_private_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
