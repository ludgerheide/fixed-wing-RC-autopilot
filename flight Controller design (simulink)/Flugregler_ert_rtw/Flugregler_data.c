/*
 * File: Flugregler_data.c
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

#include "Flugregler.h"
#include "Flugregler_private.h"

/* Block parameters (auto storage) */
P_Flugregler_T Flugregler_P = {
    20.0F,                             /* Variable: maxPitch
                                        * Referenced by:
                                        *   '<S7>/Saturate'
                                        *   '<S8>/DeadZone'
                                        */

    /* Start of '<Root>/Flugregler' */
    {
        1.80284905F,                   /* Mask Parameter: DiscretePIDController_D
                                        * Referenced by: '<S7>/Derivative Gain'
                                        */
        0.0F,                          /* Mask Parameter: DiscretePIDController_D_c
                                        * Referenced by: '<S10>/Derivative Gain'
                                        */
        0.0F,                          /* Mask Parameter: DiscretePIDController_D_l
                                        * Referenced by: '<S13>/Derivative Gain'
                                        */
        0.0242744237F,                 /* Mask Parameter: DiscretePIDController_I
                                        * Referenced by: '<S7>/Integral Gain'
                                        */
        1.7399472F,                    /* Mask Parameter: DiscretePIDController_I_m
                                        * Referenced by: '<S10>/Integral Gain'
                                        */
        5.0F,                          /* Mask Parameter: DiscretePIDController_I_h
                                        * Referenced by: '<S13>/Integral Gain'
                                        */
        -20.0F,                        /* Mask Parameter: DiscretePIDController_LowerSatu
                                        * Referenced by:
                                        *   '<S7>/Saturate'
                                        *   '<S8>/DeadZone'
                                        */
        -127.0F,                       /* Mask Parameter: DiscretePIDController_LowerSa_j
                                        * Referenced by:
                                        *   '<S10>/Saturate'
                                        *   '<S11>/DeadZone'
                                        */
        -127.0F,                       /* Mask Parameter: DiscretePIDController_LowerSa_h
                                        * Referenced by:
                                        *   '<S13>/Saturate'
                                        *   '<S15>/DeadZone'
                                        */
        1.40744936F,                   /* Mask Parameter: DiscretePIDController_N
                                        * Referenced by: '<S7>/Filter Coefficient'
                                        */
        0.470803052F,                  /* Mask Parameter: DiscretePIDController_P
                                        * Referenced by: '<S7>/Proportional Gain'
                                        */
        3.82846761F,                   /* Mask Parameter: DiscretePIDController_P_i
                                        * Referenced by: '<S10>/Proportional Gain'
                                        */
        10.0F,                         /* Mask Parameter: DiscretePIDController_P_b
                                        * Referenced by: '<S13>/Proportional Gain'
                                        */
        127.0F,                        /* Mask Parameter: DiscretePIDController_UpperSatu
                                        * Referenced by:
                                        *   '<S10>/Saturate'
                                        *   '<S11>/DeadZone'
                                        */
        127.0F,                        /* Mask Parameter: DiscretePIDController_UpperSa_m
                                        * Referenced by:
                                        *   '<S13>/Saturate'
                                        *   '<S15>/DeadZone'
                                        */
        180.0,                         /* Expression: 180
                                        * Referenced by: '<S9>/Constant2'
                                        */
        360.0,                         /* Expression: 360
                                        * Referenced by: '<S9>/Constant1'
                                        */
        180.0,                         /* Expression: 180
                                        * Referenced by: '<S9>/Constant'
                                        */
        -1.0,                          /* Expression: -1
                                        * Referenced by: '<S4>/Invert'
                                        */
        0.66,                          /* Expression: 0.66
                                        * Referenced by: '<S3>/base thrust'
                                        */
        255.0,                         /* Expression: 255
                                        * Referenced by: '<S3>/to uint8'
                                        */
        255.0,                         /* Expression: 255
                                        * Referenced by: '<S3>/Saturation'
                                        */
        0.0,                           /* Expression: 0
                                        * Referenced by: '<S3>/Saturation'
                                        */
        127.0,                         /* Expression: 127
                                        * Referenced by: '<S5>/bias'
                                        */
        127.0,                         /* Expression: 127
                                        * Referenced by: '<S6>/Constant'
                                        */
        0.0F,                          /* Computed Parameter: Constant_Value_j
                                        * Referenced by: '<S7>/Constant'
                                        */
        0.0F,                          /* Computed Parameter: Constant_Value_d
                                        * Referenced by: '<S10>/Constant'
                                        */
        0.0F,                          /* Computed Parameter: Constant_Value_g
                                        * Referenced by: '<S13>/Constant'
                                        */
        0.01F,                         /* Computed Parameter: converttometers_Gain
                                        * Referenced by: '<S2>/convert to meters'
                                        */
        0.02F,                         /* Computed Parameter: Integrator_gainval
                                        * Referenced by: '<S7>/Integrator'
                                        */
        0.0F,                          /* Computed Parameter: Integrator_IC
                                        * Referenced by: '<S7>/Integrator'
                                        */
        0.02F,                         /* Computed Parameter: Filter_gainval
                                        * Referenced by: '<S7>/Filter'
                                        */
        0.0F,                          /* Computed Parameter: Filter_IC
                                        * Referenced by: '<S7>/Filter'
                                        */
        0.0F,                          /* Computed Parameter: ZeroGain_Gain
                                        * Referenced by: '<S8>/ZeroGain'
                                        */
        0.02F,                         /* Computed Parameter: Integrator_gainval_e
                                        * Referenced by: '<S10>/Integrator'
                                        */
        0.0F,                          /* Computed Parameter: Integrator_IC_i
                                        * Referenced by: '<S10>/Integrator'
                                        */
        50.0F,                         /* Computed Parameter: TSamp_WtEt
                                        * Referenced by: '<S12>/TSamp'
                                        */
        0.0F,                          /* Computed Parameter: UD_InitialCondition
                                        * Referenced by: '<S12>/UD'
                                        */
        0.0F,                          /* Computed Parameter: ZeroGain_Gain_e
                                        * Referenced by: '<S11>/ZeroGain'
                                        */
        0.0787401572F,                 /* Computed Parameter: converttodegs_Gain
                                        * Referenced by: '<S6>/convert to deg//s'
                                        */
        57.2957802F,                   /* Computed Parameter: Gain_Gain
                                        * Referenced by: '<S14>/Gain'
                                        */
        0.02F,                         /* Computed Parameter: Integrator_gainval_l
                                        * Referenced by: '<S13>/Integrator'
                                        */
        0.0F,                          /* Computed Parameter: Integrator_IC_o
                                        * Referenced by: '<S13>/Integrator'
                                        */
        50.0F,                         /* Computed Parameter: TSamp_WtEt_g
                                        * Referenced by: '<S16>/TSamp'
                                        */
        0.0F,                          /* Computed Parameter: UD_InitialCondition_i
                                        * Referenced by: '<S16>/UD'
                                        */
        0.0F,                          /* Computed Parameter: ZeroGain_Gain_j
                                        * Referenced by: '<S15>/ZeroGain'
                                        */
        1417339208,                    /* Computed Parameter: strengthofclimbcorrection_Gain
                                        * Referenced by: '<S3>/strength of climb correction'
                                        */
        1U,                            /* Computed Parameter: UD_DelayLength
                                        * Referenced by: '<S12>/UD'
                                        */
        1U,                            /* Computed Parameter: UD_DelayLength_k
                                        * Referenced by: '<S16>/UD'
                                        */
        2560,                          /* Computed Parameter: Saturation_UpperSat_e
                                        * Referenced by: '<S4>/Saturation'
                                        */
        -2560,                         /* Computed Parameter: Saturation_LowerSat_e
                                        * Referenced by: '<S4>/Saturation'
                                        */
        26010,                         /* Computed Parameter: Gain_Gain_i
                                        * Referenced by: '<S4>/Gain'
                                        */
        26214                          /* Computed Parameter: tofractionofmaxPitch_Gain
                                        * Referenced by: '<S3>/to fraction of maxPitch'
                                        */
    }
    /* End of '<Root>/Flugregler' */
};

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
