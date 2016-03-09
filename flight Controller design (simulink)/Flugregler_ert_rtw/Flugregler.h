/*
 * File: Flugregler.h
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

#ifndef RTW_HEADER_Flugregler_h_
#define RTW_HEADER_Flugregler_h_
#include <math.h>
#include <float.h>
#include <stddef.h>
#include <string.h>
#ifndef Flugregler_COMMON_INCLUDES_
# define Flugregler_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Flugregler_COMMON_INCLUDES_ */

#include "Flugregler_types.h"

/* Shared type includes */
#include "multiword_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* user code (top of header file) */
#define fabs(x)                        __builtin_fabs(x)         //Otherwise gcc complains

/* Block signals for system '<Root>/Flugregler' */
typedef struct {
    uint8_T DataTypeConversion;        /* '<S3>/Data Type Conversion' */
    uint8_T DataTypeConversion2;       /* '<S5>/Data Type Conversion2' */
    uint8_T DataTypeConversion1;       /* '<S6>/Data Type Conversion1' */
} B_Flugregler_Flugregler_T;

/* Block states (auto storage) for system '<Root>/Flugregler' */
typedef struct {
    real32_T Integrator_DSTATE;        /* '<S7>/Integrator' */
    real32_T Filter_DSTATE;            /* '<S7>/Filter' */
    real32_T Integrator_DSTATE_b;      /* '<S10>/Integrator' */
    real32_T UD_DSTATE;                /* '<S12>/UD' */
    real32_T Integrator_DSTATE_h;      /* '<S13>/Integrator' */
    real32_T UD_DSTATE_c;              /* '<S16>/UD' */
} DW_Flugregler_Flugregler_T;

/* Block signals (auto storage) */
typedef struct {
    B_Flugregler_Flugregler_T Flugregler_i;/* '<Root>/Flugregler' */
} B_Flugregler_T;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
    DW_Flugregler_Flugregler_T Flugregler_i;/* '<Root>/Flugregler' */
} DW_Flugregler_T;

/* External inputs (root inport signals with auto storage) */
typedef struct {
    simulink_currentAttitude currentAttitude;/* '<Root>/currentAttitude' */
    barometricAltitude barometricAltitude_m;/* '<Root>/barometricAltitude' */
    targetHeading targetHeading_g;     /* '<Root>/targetHeading' */
    targetPitch targetPitch_f;         /* '<Root>/targetPitch' */
    currentGyro gyro;                  /* '<Root>/gyro' */
} ExtU_Flugregler_T;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
    simulink_outputCommandSet outputCommandSet;/* '<Root>/outputCommandSet' */
} ExtY_Flugregler_T;

/* Parameters for system: '<Root>/Flugregler' */
struct P_Flugregler_Flugregler_T_ {
    real32_T DiscretePIDController_D;  /* Mask Parameter: DiscretePIDController_D
                                        * Referenced by: '<S7>/Derivative Gain'
                                        */
    real32_T DiscretePIDController_D_c;/* Mask Parameter: DiscretePIDController_D_c
                                        * Referenced by: '<S10>/Derivative Gain'
                                        */
    real32_T DiscretePIDController_D_l;/* Mask Parameter: DiscretePIDController_D_l
                                        * Referenced by: '<S13>/Derivative Gain'
                                        */
    real32_T DiscretePIDController_I;  /* Mask Parameter: DiscretePIDController_I
                                        * Referenced by: '<S7>/Integral Gain'
                                        */
    real32_T DiscretePIDController_I_m;/* Mask Parameter: DiscretePIDController_I_m
                                        * Referenced by: '<S10>/Integral Gain'
                                        */
    real32_T DiscretePIDController_I_h;/* Mask Parameter: DiscretePIDController_I_h
                                        * Referenced by: '<S13>/Integral Gain'
                                        */
    real32_T DiscretePIDController_LowerSatu;/* Mask Parameter: DiscretePIDController_LowerSatu
                                              * Referenced by:
                                              *   '<S7>/Saturate'
                                              *   '<S8>/DeadZone'
                                              */
    real32_T DiscretePIDController_LowerSa_j;/* Mask Parameter: DiscretePIDController_LowerSa_j
                                              * Referenced by:
                                              *   '<S10>/Saturate'
                                              *   '<S11>/DeadZone'
                                              */
    real32_T DiscretePIDController_LowerSa_h;/* Mask Parameter: DiscretePIDController_LowerSa_h
                                              * Referenced by:
                                              *   '<S13>/Saturate'
                                              *   '<S15>/DeadZone'
                                              */
    real32_T DiscretePIDController_N;  /* Mask Parameter: DiscretePIDController_N
                                        * Referenced by: '<S7>/Filter Coefficient'
                                        */
    real32_T DiscretePIDController_P;  /* Mask Parameter: DiscretePIDController_P
                                        * Referenced by: '<S7>/Proportional Gain'
                                        */
    real32_T DiscretePIDController_P_i;/* Mask Parameter: DiscretePIDController_P_i
                                        * Referenced by: '<S10>/Proportional Gain'
                                        */
    real32_T DiscretePIDController_P_b;/* Mask Parameter: DiscretePIDController_P_b
                                        * Referenced by: '<S13>/Proportional Gain'
                                        */
    real32_T DiscretePIDController_UpperSatu;/* Mask Parameter: DiscretePIDController_UpperSatu
                                              * Referenced by:
                                              *   '<S10>/Saturate'
                                              *   '<S11>/DeadZone'
                                              */
    real32_T DiscretePIDController_UpperSa_m;/* Mask Parameter: DiscretePIDController_UpperSa_m
                                              * Referenced by:
                                              *   '<S13>/Saturate'
                                              *   '<S15>/DeadZone'
                                              */
    real_T Constant2_Value;            /* Expression: 180
                                        * Referenced by: '<S9>/Constant2'
                                        */
    real_T Constant1_Value;            /* Expression: 360
                                        * Referenced by: '<S9>/Constant1'
                                        */
    real_T Constant_Value;             /* Expression: 180
                                        * Referenced by: '<S9>/Constant'
                                        */
    real_T Invert_Gain;                /* Expression: -1
                                        * Referenced by: '<S4>/Invert'
                                        */
    real_T basethrust_Value;           /* Expression: 0.66
                                        * Referenced by: '<S3>/base thrust'
                                        */
    real_T touint8_Gain;               /* Expression: 255
                                        * Referenced by: '<S3>/to uint8'
                                        */
    real_T Saturation_UpperSat;        /* Expression: 255
                                        * Referenced by: '<S3>/Saturation'
                                        */
    real_T Saturation_LowerSat;        /* Expression: 0
                                        * Referenced by: '<S3>/Saturation'
                                        */
    real_T bias_Value;                 /* Expression: 127
                                        * Referenced by: '<S5>/bias'
                                        */
    real_T Constant_Value_n;           /* Expression: 127
                                        * Referenced by: '<S6>/Constant'
                                        */
    real32_T Constant_Value_j;         /* Computed Parameter: Constant_Value_j
                                        * Referenced by: '<S7>/Constant'
                                        */
    real32_T Constant_Value_d;         /* Computed Parameter: Constant_Value_d
                                        * Referenced by: '<S10>/Constant'
                                        */
    real32_T Constant_Value_g;         /* Computed Parameter: Constant_Value_g
                                        * Referenced by: '<S13>/Constant'
                                        */
    real32_T converttometers_Gain;     /* Computed Parameter: converttometers_Gain
                                        * Referenced by: '<S2>/convert to meters'
                                        */
    real32_T Integrator_gainval;       /* Computed Parameter: Integrator_gainval
                                        * Referenced by: '<S7>/Integrator'
                                        */
    real32_T Integrator_IC;            /* Computed Parameter: Integrator_IC
                                        * Referenced by: '<S7>/Integrator'
                                        */
    real32_T Filter_gainval;           /* Computed Parameter: Filter_gainval
                                        * Referenced by: '<S7>/Filter'
                                        */
    real32_T Filter_IC;                /* Computed Parameter: Filter_IC
                                        * Referenced by: '<S7>/Filter'
                                        */
    real32_T ZeroGain_Gain;            /* Computed Parameter: ZeroGain_Gain
                                        * Referenced by: '<S8>/ZeroGain'
                                        */
    real32_T Integrator_gainval_e;     /* Computed Parameter: Integrator_gainval_e
                                        * Referenced by: '<S10>/Integrator'
                                        */
    real32_T Integrator_IC_i;          /* Computed Parameter: Integrator_IC_i
                                        * Referenced by: '<S10>/Integrator'
                                        */
    real32_T TSamp_WtEt;               /* Computed Parameter: TSamp_WtEt
                                        * Referenced by: '<S12>/TSamp'
                                        */
    real32_T UD_InitialCondition;      /* Computed Parameter: UD_InitialCondition
                                        * Referenced by: '<S12>/UD'
                                        */
    real32_T ZeroGain_Gain_e;          /* Computed Parameter: ZeroGain_Gain_e
                                        * Referenced by: '<S11>/ZeroGain'
                                        */
    real32_T converttodegs_Gain;       /* Computed Parameter: converttodegs_Gain
                                        * Referenced by: '<S6>/convert to deg//s'
                                        */
    real32_T Gain_Gain;                /* Computed Parameter: Gain_Gain
                                        * Referenced by: '<S14>/Gain'
                                        */
    real32_T Integrator_gainval_l;     /* Computed Parameter: Integrator_gainval_l
                                        * Referenced by: '<S13>/Integrator'
                                        */
    real32_T Integrator_IC_o;          /* Computed Parameter: Integrator_IC_o
                                        * Referenced by: '<S13>/Integrator'
                                        */
    real32_T TSamp_WtEt_g;             /* Computed Parameter: TSamp_WtEt_g
                                        * Referenced by: '<S16>/TSamp'
                                        */
    real32_T UD_InitialCondition_i;    /* Computed Parameter: UD_InitialCondition_i
                                        * Referenced by: '<S16>/UD'
                                        */
    real32_T ZeroGain_Gain_j;          /* Computed Parameter: ZeroGain_Gain_j
                                        * Referenced by: '<S15>/ZeroGain'
                                        */
    int32_T strengthofclimbcorrection_Gain;/* Computed Parameter: strengthofclimbcorrection_Gain
                                            * Referenced by: '<S3>/strength of climb correction'
                                            */
    uint16_T UD_DelayLength;           /* Computed Parameter: UD_DelayLength
                                        * Referenced by: '<S12>/UD'
                                        */
    uint16_T UD_DelayLength_k;         /* Computed Parameter: UD_DelayLength_k
                                        * Referenced by: '<S16>/UD'
                                        */
    int16_T Saturation_UpperSat_e;     /* Computed Parameter: Saturation_UpperSat_e
                                        * Referenced by: '<S4>/Saturation'
                                        */
    int16_T Saturation_LowerSat_e;     /* Computed Parameter: Saturation_LowerSat_e
                                        * Referenced by: '<S4>/Saturation'
                                        */
    int16_T Gain_Gain_i;               /* Computed Parameter: Gain_Gain_i
                                        * Referenced by: '<S4>/Gain'
                                        */
    int16_T tofractionofmaxPitch_Gain; /* Computed Parameter: tofractionofmaxPitch_Gain
                                        * Referenced by: '<S3>/to fraction of maxPitch'
                                        */
};

/* Parameters (auto storage) */
struct P_Flugregler_T_ {
    real32_T maxPitch;                 /* Variable: maxPitch
                                        * Referenced by:
                                        *   '<S7>/Saturate'
                                        *   '<S8>/DeadZone'
                                        */
    P_Flugregler_Flugregler_T Flugregler_i;/* '<Root>/Flugregler' */
};

/* Real-time Model Data Structure */
struct tag_RTM_Flugregler_T {
    const char_T * volatile errorStatus;
};

/* Block parameters (auto storage) */
extern P_Flugregler_T Flugregler_P;

/* Block signals (auto storage) */
extern B_Flugregler_T Flugregler_B;

/* Block states (auto storage) */
extern DW_Flugregler_T Flugregler_DW;

/* External inputs (root inport signals with auto storage) */
extern ExtU_Flugregler_T Flugregler_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExtY_Flugregler_T Flugregler_Y;

/* External data declarations for dependent source files */
extern const simulink_outputCommandSet Flugregler_rtZsimulink_outputCommandSet;/* simulink_outputCommandSet ground */

/* Model entry point functions */
extern void Flugregler_initialize(void);
extern void Flugregler_step(void);

/* Real-time Model object */
extern RT_MODEL_Flugregler_T *const Flugregler_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('flightController/Flugregler')    - opens subsystem flightController/Flugregler
 * hilite_system('flightController/Flugregler/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'flightController'
 * '<S1>'   : 'flightController/Flugregler'
 * '<S2>'   : 'flightController/Flugregler/altitude controller'
 * '<S3>'   : 'flightController/Flugregler/engine controller'
 * '<S4>'   : 'flightController/Flugregler/heading controller'
 * '<S5>'   : 'flightController/Flugregler/pitch controller'
 * '<S6>'   : 'flightController/Flugregler/turn controller'
 * '<S7>'   : 'flightController/Flugregler/altitude controller/Discrete PID Controller'
 * '<S8>'   : 'flightController/Flugregler/altitude controller/Discrete PID Controller/Clamping circuit'
 * '<S9>'   : 'flightController/Flugregler/heading controller/Subsystem'
 * '<S10>'  : 'flightController/Flugregler/pitch controller/Discrete PID Controller'
 * '<S11>'  : 'flightController/Flugregler/pitch controller/Discrete PID Controller/Clamping circuit'
 * '<S12>'  : 'flightController/Flugregler/pitch controller/Discrete PID Controller/Differentiator'
 * '<S13>'  : 'flightController/Flugregler/turn controller/Discrete PID Controller'
 * '<S14>'  : 'flightController/Flugregler/turn controller/Radians to Degrees1'
 * '<S15>'  : 'flightController/Flugregler/turn controller/Discrete PID Controller/Clamping circuit'
 * '<S16>'  : 'flightController/Flugregler/turn controller/Discrete PID Controller/Differentiator'
 */
#endif                                 /* RTW_HEADER_Flugregler_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
