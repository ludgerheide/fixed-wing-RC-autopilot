/*
 * File: multiword_types.h
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

#ifndef __MULTIWORD_TYPES_H__
#define __MULTIWORD_TYPES_H__
#include "rtwtypes.h"

/*
 * MultiWord supporting definitions
 */
typedef long int long_T;

/*
 * MultiWord types
 */
typedef struct {
    uint32_T chunks[2];
} int64m_T;

typedef struct {
    uint32_T chunks[2];
} uint64m_T;

typedef struct {
    uint32_T chunks[3];
} int96m_T;

typedef struct {
    uint32_T chunks[3];
} uint96m_T;

typedef struct {
    uint32_T chunks[4];
} int128m_T;

typedef struct {
    uint32_T chunks[4];
} uint128m_T;

typedef struct {
    uint32_T chunks[5];
} int160m_T;

typedef struct {
    uint32_T chunks[5];
} uint160m_T;

typedef struct {
    uint32_T chunks[6];
} int192m_T;

typedef struct {
    uint32_T chunks[6];
} uint192m_T;

typedef struct {
    uint32_T chunks[7];
} int224m_T;

typedef struct {
    uint32_T chunks[7];
} uint224m_T;

typedef struct {
    uint32_T chunks[8];
} int256m_T;

typedef struct {
    uint32_T chunks[8];
} uint256m_T;

#endif                                 /* __MULTIWORD_TYPES_H__ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
