/*
 * File: Flugregler_types.h
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

#ifndef RTW_HEADER_Flugregler_types_h_
#define RTW_HEADER_Flugregler_types_h_
#include "rtwtypes.h"
#include "multiword_types.h"
#ifndef _DEFINED_TYPEDEF_FOR_simulink_currentAttitude_
#define _DEFINED_TYPEDEF_FOR_simulink_currentAttitude_

typedef struct {
    real32_T courseMagnetic;
    real32_T pitch;
    real32_T roll;
} simulink_currentAttitude;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_barometricAltitude_
#define _DEFINED_TYPEDEF_FOR_barometricAltitude_

typedef struct {
    int32_T altitude;
} barometricAltitude;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_targetHeading_
#define _DEFINED_TYPEDEF_FOR_targetHeading_

typedef struct {
    boolean_T justRateOfTurnEnabled;
    uint16_T targetHeading;
    int8_T targetRateOfTurn;
} targetHeading;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_targetPitch_
#define _DEFINED_TYPEDEF_FOR_targetPitch_

typedef struct {
    boolean_T justPitchEnabled;
    int8_T targetPitch;
    int32_T targetAltitude;
} targetPitch;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_currentGyro_
#define _DEFINED_TYPEDEF_FOR_currentGyro_

typedef struct {
    real32_T x;
    real32_T y;
    real32_T z;
} currentGyro;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_simulink_outputCommandSet_
#define _DEFINED_TYPEDEF_FOR_simulink_outputCommandSet_

typedef struct {
    uint8_T rudder;
    uint8_T elevator;
    uint8_T thrust;
} simulink_outputCommandSet;

#endif

/* Parameters for system: '<Root>/Flugregler' */
typedef struct P_Flugregler_Flugregler_T_ P_Flugregler_Flugregler_T;

/* Parameters (auto storage) */
typedef struct P_Flugregler_T_ P_Flugregler_T;

/* Forward declaration for rtModel */
typedef struct tag_RTM_Flugregler_T RT_MODEL_Flugregler_T;

#endif                                 /* RTW_HEADER_Flugregler_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
