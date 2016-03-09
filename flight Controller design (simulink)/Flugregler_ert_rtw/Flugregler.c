/*
 * File: Flugregler.c
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

const simulink_outputCommandSet Flugregler_rtZsimulink_outputCommandSet = {
    0U,                                /* rudder */
    0U,                                /* elevator */
    0U                                 /* thrust */
} ;                                    /* simulink_outputCommandSet ground */

/* Block signals (auto storage) */
B_Flugregler_T Flugregler_B;

/* Block states (auto storage) */
DW_Flugregler_T Flugregler_DW;

/* External inputs (root inport signals with auto storage) */
ExtU_Flugregler_T Flugregler_U;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_Flugregler_T Flugregler_Y;

/* Real-time model */
RT_MODEL_Flugregler_T Flugregler_M_;
RT_MODEL_Flugregler_T *const Flugregler_M = &Flugregler_M_;
real_T sMultiWord2Double(const uint32_T u1[], int16_T n1, int16_T e1)
{
    real_T y;
    int16_T i;
    int16_T exp_0;
    uint32_T u1i;
    uint32_T cb;
    y = 0.0;
    exp_0 = e1;
    if ((u1[n1 - 1] & 2147483648UL) != 0UL) {
        cb = 1UL;
        for (i = 0; i < n1; i++) {
            u1i = ~u1[i];
            cb += u1i;
            y -= ldexp(cb, exp_0);
            cb = (uint32_T)(cb < u1i);
            exp_0 += 32;
        }
    } else {
        for (i = 0; i < n1; i++) {
            y += ldexp(u1[i], exp_0);
            exp_0 += 32;
        }
    }

    return y;
}

void sMultiWordMul(const uint32_T u1[], int16_T n1, const uint32_T u2[], int16_T
                   n2, uint32_T y[], int16_T n)
{
    int16_T i;
    int16_T j;
    int16_T k;
    int16_T nj;
    uint32_T u1i;
    uint32_T yk;
    uint32_T a1;
    uint32_T a0;
    uint32_T b1;
    uint32_T w10;
    uint32_T w01;
    uint32_T cb;
    boolean_T isNegative1;
    boolean_T isNegative2;
    uint32_T cb1;
    uint32_T cb2;
    isNegative1 = ((u1[n1 - 1] & 2147483648UL) != 0UL);
    isNegative2 = ((u2[n2 - 1] & 2147483648UL) != 0UL);
    cb1 = 1UL;

    /* Initialize output to zero */
    for (k = 0; k < n; k++) {
        y[k] = 0UL;
    }

    for (i = 0; i < n1; i++) {
        cb = 0UL;
        u1i = u1[i];
        if (isNegative1) {
            u1i = ~u1i + cb1;
            cb1 = (uint32_T)(u1i < cb1);
        }

        a1 = u1i >> 16U;
        a0 = u1i & 65535UL;
        cb2 = 1UL;
        k = n - i;
        nj = n2 <= k ? n2 : k;
        k = i;
        for (j = 0; j < nj; j++) {
            yk = y[k];
            u1i = u2[j];
            if (isNegative2) {
                u1i = ~u1i + cb2;
                cb2 = (uint32_T)(u1i < cb2);
            }

            b1 = u1i >> 16U;
            u1i &= 65535UL;
            w10 = a1 * u1i;
            w01 = a0 * b1;
            yk += cb;
            cb = (uint32_T)(yk < cb);
            u1i *= a0;
            yk += u1i;
            cb += (yk < u1i);
            u1i = w10 << 16U;
            yk += u1i;
            cb += (yk < u1i);
            u1i = w01 << 16U;
            yk += u1i;
            cb += (yk < u1i);
            y[k] = yk;
            cb += w10 >> 16U;
            cb += w01 >> 16U;
            cb += a1 * b1;
            k++;
        }

        if (k < n) {
            y[k] = cb;
        }
    }

    /* Apply sign */
    if (isNegative1 != isNegative2) {
        cb = 1UL;
        for (k = 0; k < n; k++) {
            yk = ~y[k] + cb;
            y[k] = yk;
            cb = (uint32_T)(yk < cb);
        }
    }
}

real_T rt_roundd(real_T u)
{
    real_T y;
    if (fabs(u) < 4.503599627370496E+15) {
        if (u >= 0.5) {
            y = floor(u + 0.5);
        } else if (u > -0.5) {
            y = 0.0;
        } else {
            y = ceil(u - 0.5);
        }
    } else {
        y = u;
    }

    return y;
}

real_T rt_modd(real_T u0, real_T u1)
{
    real_T y;
    real_T tmp;
    if (u1 == 0.0) {
        y = u0;
    } else {
        tmp = u0 / u1;
        if (u1 <= floor(u1)) {
            y = u0 - floor(tmp) * u1;
        } else {
            if (fabs(tmp - rt_roundd(tmp)) <= DBL_EPSILON * fabs(tmp)) {
                y = 0.0;
            } else {
                y = (tmp - floor(tmp)) * u1;
            }
        }
    }

    return y;
}

/* Initial conditions for atomic system: '<Root>/Flugregler' */
void Flugregler_Flugregler_Init(DW_Flugregler_Flugregler_T *localDW,
    P_Flugregler_Flugregler_T *localP)
{
    /* InitializeConditions for DiscreteIntegrator: '<S7>/Integrator' */
    localDW->Integrator_DSTATE = localP->Integrator_IC;

    /* InitializeConditions for DiscreteIntegrator: '<S7>/Filter' */
    localDW->Filter_DSTATE = localP->Filter_IC;

    /* InitializeConditions for DiscreteIntegrator: '<S10>/Integrator' */
    localDW->Integrator_DSTATE_b = localP->Integrator_IC_i;

    /* InitializeConditions for Delay: '<S12>/UD' */
    localDW->UD_DSTATE = localP->UD_InitialCondition;

    /* InitializeConditions for DiscreteIntegrator: '<S13>/Integrator' */
    localDW->Integrator_DSTATE_h = localP->Integrator_IC_o;

    /* InitializeConditions for Delay: '<S16>/UD' */
    localDW->UD_DSTATE_c = localP->UD_InitialCondition_i;
}

/* Output and update for atomic system: '<Root>/Flugregler' */
void Flugregler_Flugregler(real32_T rtu_currentAttitude, real32_T
    rtu_currentAttitude_e, int32_T rtu_barometricAltitude, int8_T
    rtu_targetHeading, boolean_T rtu_targetHeading_h, uint16_T
    rtu_targetHeading_m, int32_T rtu_targetPitch, int8_T rtu_targetPitch_h,
    boolean_T rtu_targetPitch_e, real32_T rtu_gyro, B_Flugregler_Flugregler_T
    *localB, DW_Flugregler_Flugregler_T *localDW, P_Flugregler_Flugregler_T
    *localP)
{
    real32_T rtb_UD;
    real32_T rtb_SignPreIntegrator;
    real32_T rtb_IntegralGain;
    real32_T rtb_FilterCoefficient;
    real32_T rtb_ZeroGain;
    boolean_T rtb_NotEqual;
    int8_T rtb_Switch_f;
    real32_T rtb_TSamp;
    boolean_T rtb_NotEqual_m0;
    int8_T rtb_Switch1;
    real32_T rtb_TSamp_f;
    real32_T rtb_ZeroGain_oq;
    int16_T rtb_Saturation;
    real32_T tmp;
    int64m_T tmp_0;
    uint32_T tmp_1;
    uint32_T tmp_2;
    real_T u0;
    int8_T rtb_SignPreIntegrator_0;

    /* Gain: '<S2>/convert to meters' incorporates:
     *  DataTypeConversion: '<S2>/Data Type Conversion'
     *  Sum: '<S2>/Sum'
     */
    rtb_UD = (real32_T)(rtu_targetPitch - rtu_barometricAltitude) *
        localP->converttometers_Gain;

    /* Gain: '<S7>/Filter Coefficient' incorporates:
     *  DiscreteIntegrator: '<S7>/Filter'
     *  Gain: '<S7>/Derivative Gain'
     *  Sum: '<S7>/SumD'
     */
    rtb_FilterCoefficient = (localP->DiscretePIDController_D * rtb_UD -
        localDW->Filter_DSTATE) * localP->DiscretePIDController_N;

    /* Sum: '<S7>/Sum' incorporates:
     *  DiscreteIntegrator: '<S7>/Integrator'
     *  Gain: '<S7>/Proportional Gain'
     */
    rtb_SignPreIntegrator = (localP->DiscretePIDController_P * rtb_UD +
        localDW->Integrator_DSTATE) + rtb_FilterCoefficient;

    /* Switch: '<S1>/Switch' incorporates:
     *  DataTypeConversion: '<S2>/Data Type Conversion1'
     *  Saturate: '<S7>/Saturate'
     */
    if (rtu_targetPitch_e) {
        rtb_Switch_f = rtu_targetPitch_h;
    } else {
        if (rtb_SignPreIntegrator > Flugregler_P.maxPitch) {
            /* Saturate: '<S7>/Saturate' */
            rtb_ZeroGain_oq = Flugregler_P.maxPitch;
        } else if (rtb_SignPreIntegrator <
                   localP->DiscretePIDController_LowerSatu) {
            /* Saturate: '<S7>/Saturate' */
            rtb_ZeroGain_oq = localP->DiscretePIDController_LowerSatu;
        } else {
            /* Saturate: '<S7>/Saturate' */
            rtb_ZeroGain_oq = rtb_SignPreIntegrator;
        }

        /* DataTypeConversion: '<S2>/Data Type Conversion1' incorporates:
         *  Saturate: '<S7>/Saturate'
         */
        tmp = fmodf(floorf(rtb_ZeroGain_oq), 256.0F);
        rtb_Switch_f = (int8_T)(tmp < 0.0F ? (int16_T)(int8_T)-(int8_T)(uint8_T)
                                -tmp : (int16_T)(int8_T)(uint8_T)tmp);
    }

    /* End of Switch: '<S1>/Switch' */

    /* Switch: '<S1>/Switch1' incorporates:
     *  DataTypeConversion: '<S4>/Data Type Conversion3'
     *  Gain: '<S4>/Gain'
     */
    if (rtu_targetHeading_h) {
        rtb_Switch1 = rtu_targetHeading;
    } else {
        /* DataTypeConversion: '<S4>/Data Type Conversion' */
        tmp = fmodf(roundf(rtu_currentAttitude_e), 65536.0F);

        /* DataTypeConversion: '<S4>/Fixed-Point' incorporates:
         *  Constant: '<S9>/Constant'
         *  Constant: '<S9>/Constant1'
         *  Constant: '<S9>/Constant2'
         *  DataTypeConversion: '<S4>/Data Type Conversion'
         *  DataTypeConversion: '<S4>/Data Type Conversion1'
         *  Gain: '<S4>/Invert'
         *  Math: '<S9>/Math Function'
         *  Sum: '<S9>/Sum'
         *  Sum: '<S9>/Sum1'
         *  Sum: '<S9>/Sum2'
         */
        u0 = fmod(floor((rt_modd((real_T)((int16_T)rtu_targetHeading_m - (tmp <
                       0.0F ? -(int16_T)(uint16_T)-tmp : (int16_T)(uint16_T)tmp))
                     + localP->Constant_Value, localP->Constant1_Value) -
                         localP->Constant2_Value) * localP->Invert_Gain * 128.0),
                  65536.0);
        rtb_Saturation = u0 < 0.0 ? -(int16_T)(uint16_T)-u0 : (int16_T)(uint16_T)
            u0;

        /* Saturate: '<S4>/Saturation' */
        if (rtb_Saturation > localP->Saturation_UpperSat_e) {
            rtb_Saturation = localP->Saturation_UpperSat_e;
        } else {
            if (rtb_Saturation < localP->Saturation_LowerSat_e) {
                rtb_Saturation = localP->Saturation_LowerSat_e;
            }
        }

        /* End of Saturate: '<S4>/Saturation' */
        rtb_Switch1 = (int8_T)((int32_T)localP->Gain_Gain_i * rtb_Saturation >>
                               19);
    }

    /* End of Switch: '<S1>/Switch1' */

    /* Gain: '<S7>/Integral Gain' */
    rtb_UD *= localP->DiscretePIDController_I;

    /* Gain: '<S8>/ZeroGain' */
    rtb_ZeroGain = localP->ZeroGain_Gain * rtb_SignPreIntegrator;

    /* DeadZone: '<S8>/DeadZone' */
    if (rtb_SignPreIntegrator > Flugregler_P.maxPitch) {
        rtb_SignPreIntegrator -= Flugregler_P.maxPitch;
    } else if (rtb_SignPreIntegrator >= localP->DiscretePIDController_LowerSatu)
    {
        rtb_SignPreIntegrator = 0.0F;
    } else {
        rtb_SignPreIntegrator -= localP->DiscretePIDController_LowerSatu;
    }

    /* End of DeadZone: '<S8>/DeadZone' */

    /* RelationalOperator: '<S8>/NotEqual' */
    rtb_NotEqual = (rtb_ZeroGain != rtb_SignPreIntegrator);

    /* Signum: '<S8>/SignDeltaU' */
    if (rtb_SignPreIntegrator < 0.0F) {
        rtb_SignPreIntegrator = -1.0F;
    } else {
        if (rtb_SignPreIntegrator > 0.0F) {
            rtb_SignPreIntegrator = 1.0F;
        }
    }

    /* End of Signum: '<S8>/SignDeltaU' */

    /* Signum: '<S8>/SignPreIntegrator' */
    if (rtb_UD < 0.0F) {
        rtb_ZeroGain_oq = -1.0F;
    } else if (rtb_UD > 0.0F) {
        rtb_ZeroGain_oq = 1.0F;
    } else {
        rtb_ZeroGain_oq = rtb_UD;
    }

    /* DataTypeConversion: '<S8>/DataTypeConv2' incorporates:
     *  Signum: '<S8>/SignPreIntegrator'
     */
    tmp = fmodf(rtb_ZeroGain_oq, 256.0F);

    /* DataTypeConversion: '<S8>/DataTypeConv1' */
    if (rtb_SignPreIntegrator < 128.0F) {
        rtb_SignPreIntegrator_0 = (int8_T)rtb_SignPreIntegrator;
    } else {
        rtb_SignPreIntegrator_0 = MAX_int8_T;
    }

    /* End of DataTypeConversion: '<S8>/DataTypeConv1' */

    /* Gain: '<S3>/strength of climb correction' incorporates:
     *  DataTypeConversion: '<S3>/Convert'
     *  Gain: '<S3>/to fraction of maxPitch'
     */
    tmp_1 = (uint32_T)localP->strengthofclimbcorrection_Gain;
    tmp_2 = (uint32_T)((int32_T)(rtb_Switch_f << 7) *
                       localP->tofractionofmaxPitch_Gain);
    sMultiWordMul(&tmp_1, 1, &tmp_2, 1, &tmp_0.chunks[0U], 2);

    /* Gain: '<S3>/to uint8' incorporates:
     *  Constant: '<S3>/base thrust'
     *  Sum: '<S3>/Sum'
     */
    u0 = (sMultiWord2Double(&tmp_0.chunks[0U], 2, 0) * 3.4694469519536142E-18 +
          localP->basethrust_Value) * localP->touint8_Gain;

    /* Saturate: '<S3>/Saturation' */
    if (u0 > localP->Saturation_UpperSat) {
        u0 = localP->Saturation_UpperSat;
    } else {
        if (u0 < localP->Saturation_LowerSat) {
            u0 = localP->Saturation_LowerSat;
        }
    }

    /* DataTypeConversion: '<S3>/Data Type Conversion' incorporates:
     *  Saturate: '<S3>/Saturation'
     */
    u0 = fmod(floor(u0), 256.0);
    localB->DataTypeConversion = (uint8_T)(u0 < 0.0 ? (int16_T)(uint8_T)-(int8_T)
        (uint8_T)-u0 : (int16_T)(uint8_T)u0);

    /* Sum: '<S5>/Sum' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtb_IntegralGain = (real32_T)rtb_Switch_f - rtu_currentAttitude;

    /* SampleTimeMath: '<S12>/TSamp' incorporates:
     *  Gain: '<S10>/Derivative Gain'
     *
     * About '<S12>/TSamp':
     *  y = u * K where K = 1 / ( w * Ts )
     */
    rtb_TSamp = localP->DiscretePIDController_D_c * rtb_IntegralGain *
        localP->TSamp_WtEt;

    /* Sum: '<S10>/Sum' incorporates:
     *  Delay: '<S12>/UD'
     *  DiscreteIntegrator: '<S10>/Integrator'
     *  Gain: '<S10>/Proportional Gain'
     *  Sum: '<S12>/Diff'
     */
    rtb_SignPreIntegrator = (localP->DiscretePIDController_P_i *
        rtb_IntegralGain + localDW->Integrator_DSTATE_b) + (rtb_TSamp -
        localDW->UD_DSTATE);

    /* Saturate: '<S10>/Saturate' */
    if (rtb_SignPreIntegrator > localP->DiscretePIDController_UpperSatu) {
        rtb_ZeroGain_oq = localP->DiscretePIDController_UpperSatu;
    } else if (rtb_SignPreIntegrator < localP->DiscretePIDController_LowerSa_j)
    {
        rtb_ZeroGain_oq = localP->DiscretePIDController_LowerSa_j;
    } else {
        rtb_ZeroGain_oq = rtb_SignPreIntegrator;
    }

    /* DataTypeConversion: '<S5>/Data Type Conversion1' incorporates:
     *  Saturate: '<S10>/Saturate'
     */
    rtb_ZeroGain_oq = fmodf(floorf(rtb_ZeroGain_oq), 65536.0F);

    /* DataTypeConversion: '<S5>/Data Type Conversion2' incorporates:
     *  Constant: '<S5>/bias'
     *  DataTypeConversion: '<S5>/Data Type Conversion1'
     *  Sum: '<S5>/Sum1'
     */
    u0 = fmod(floor((real_T)(rtb_ZeroGain_oq < 0.0F ? -(int16_T)(uint16_T)
                -rtb_ZeroGain_oq : (int16_T)(uint16_T)rtb_ZeroGain_oq) +
                    localP->bias_Value), 256.0);
    localB->DataTypeConversion2 = (uint8_T)(u0 < 0.0 ? (int16_T)(uint8_T)
        -(int8_T)(uint8_T)-u0 : (int16_T)(uint8_T)u0);

    /* Gain: '<S10>/Integral Gain' */
    rtb_IntegralGain *= localP->DiscretePIDController_I_m;

    /* Gain: '<S11>/ZeroGain' */
    rtb_ZeroGain = localP->ZeroGain_Gain_e * rtb_SignPreIntegrator;

    /* DeadZone: '<S11>/DeadZone' */
    if (rtb_SignPreIntegrator > localP->DiscretePIDController_UpperSatu) {
        rtb_SignPreIntegrator -= localP->DiscretePIDController_UpperSatu;
    } else if (rtb_SignPreIntegrator >= localP->DiscretePIDController_LowerSa_j)
    {
        rtb_SignPreIntegrator = 0.0F;
    } else {
        rtb_SignPreIntegrator -= localP->DiscretePIDController_LowerSa_j;
    }

    /* End of DeadZone: '<S11>/DeadZone' */

    /* RelationalOperator: '<S11>/NotEqual' */
    rtb_NotEqual_m0 = (rtb_ZeroGain != rtb_SignPreIntegrator);

    /* Signum: '<S11>/SignDeltaU' */
    if (rtb_SignPreIntegrator < 0.0F) {
        rtb_SignPreIntegrator = -1.0F;
    } else {
        if (rtb_SignPreIntegrator > 0.0F) {
            rtb_SignPreIntegrator = 1.0F;
        }
    }

    /* End of Signum: '<S11>/SignDeltaU' */

    /* Signum: '<S11>/SignPreIntegrator' */
    if (rtb_IntegralGain < 0.0F) {
        rtb_ZeroGain_oq = -1.0F;
    } else if (rtb_IntegralGain > 0.0F) {
        rtb_ZeroGain_oq = 1.0F;
    } else {
        rtb_ZeroGain_oq = rtb_IntegralGain;
    }

    /* DataTypeConversion: '<S11>/DataTypeConv2' incorporates:
     *  Signum: '<S11>/SignPreIntegrator'
     */
    rtb_ZeroGain_oq = fmodf(rtb_ZeroGain_oq, 256.0F);

    /* DataTypeConversion: '<S11>/DataTypeConv1' */
    if (rtb_SignPreIntegrator < 128.0F) {
        rtb_Switch_f = (int8_T)rtb_SignPreIntegrator;
    } else {
        rtb_Switch_f = MAX_int8_T;
    }

    /* End of DataTypeConversion: '<S11>/DataTypeConv1' */

    /* Switch: '<S10>/Switch' incorporates:
     *  Constant: '<S10>/Constant'
     *  DataTypeConversion: '<S11>/DataTypeConv2'
     *  Logic: '<S11>/AND'
     *  RelationalOperator: '<S11>/Equal'
     */
    if (rtb_NotEqual_m0 && ((rtb_ZeroGain_oq < 0.0F ? (int16_T)(int8_T)-(int8_T)
                             (uint8_T)-rtb_ZeroGain_oq : (int16_T)(int8_T)
                             (uint8_T)rtb_ZeroGain_oq) == rtb_Switch_f)) {
        rtb_ZeroGain = localP->Constant_Value_d;
    } else {
        rtb_ZeroGain = rtb_IntegralGain;
    }

    /* End of Switch: '<S10>/Switch' */

    /* Sum: '<S6>/Sum' incorporates:
     *  DataTypeConversion: '<S6>/Data Type Conversion'
     *  Gain: '<S14>/Gain'
     *  Gain: '<S6>/convert to deg//s'
     */
    rtb_IntegralGain = localP->converttodegs_Gain * (real32_T)rtb_Switch1 -
        localP->Gain_Gain * rtu_gyro;

    /* SampleTimeMath: '<S16>/TSamp' incorporates:
     *  Gain: '<S13>/Derivative Gain'
     *
     * About '<S16>/TSamp':
     *  y = u * K where K = 1 / ( w * Ts )
     */
    rtb_TSamp_f = localP->DiscretePIDController_D_l * rtb_IntegralGain *
        localP->TSamp_WtEt_g;

    /* Sum: '<S13>/Sum' incorporates:
     *  Delay: '<S16>/UD'
     *  DiscreteIntegrator: '<S13>/Integrator'
     *  Gain: '<S13>/Proportional Gain'
     *  Sum: '<S16>/Diff'
     */
    rtb_SignPreIntegrator = (localP->DiscretePIDController_P_b *
        rtb_IntegralGain + localDW->Integrator_DSTATE_h) + (rtb_TSamp_f -
        localDW->UD_DSTATE_c);

    /* Saturate: '<S13>/Saturate' */
    if (rtb_SignPreIntegrator > localP->DiscretePIDController_UpperSa_m) {
        rtb_ZeroGain_oq = localP->DiscretePIDController_UpperSa_m;
    } else if (rtb_SignPreIntegrator < localP->DiscretePIDController_LowerSa_h)
    {
        rtb_ZeroGain_oq = localP->DiscretePIDController_LowerSa_h;
    } else {
        rtb_ZeroGain_oq = rtb_SignPreIntegrator;
    }

    /* DataTypeConversion: '<S6>/Data Type Conversion1' incorporates:
     *  Constant: '<S6>/Constant'
     *  Saturate: '<S13>/Saturate'
     *  Sum: '<S6>/Sum1'
     */
    u0 = fmod(floor(rtb_ZeroGain_oq + localP->Constant_Value_n), 256.0);
    localB->DataTypeConversion1 = (uint8_T)(u0 < 0.0 ? (int16_T)(uint8_T)
        -(int8_T)(uint8_T)-u0 : (int16_T)(uint8_T)u0);

    /* Gain: '<S13>/Integral Gain' */
    rtb_IntegralGain *= localP->DiscretePIDController_I_h;

    /* Gain: '<S15>/ZeroGain' */
    rtb_ZeroGain_oq = localP->ZeroGain_Gain_j * rtb_SignPreIntegrator;

    /* DeadZone: '<S15>/DeadZone' */
    if (rtb_SignPreIntegrator > localP->DiscretePIDController_UpperSa_m) {
        rtb_SignPreIntegrator -= localP->DiscretePIDController_UpperSa_m;
    } else if (rtb_SignPreIntegrator >= localP->DiscretePIDController_LowerSa_h)
    {
        rtb_SignPreIntegrator = 0.0F;
    } else {
        rtb_SignPreIntegrator -= localP->DiscretePIDController_LowerSa_h;
    }

    /* End of DeadZone: '<S15>/DeadZone' */

    /* RelationalOperator: '<S15>/NotEqual' */
    rtb_NotEqual_m0 = (rtb_ZeroGain_oq != rtb_SignPreIntegrator);

    /* Signum: '<S15>/SignDeltaU' */
    if (rtb_SignPreIntegrator < 0.0F) {
        rtb_SignPreIntegrator = -1.0F;
    } else {
        if (rtb_SignPreIntegrator > 0.0F) {
            rtb_SignPreIntegrator = 1.0F;
        }
    }

    /* End of Signum: '<S15>/SignDeltaU' */

    /* Switch: '<S7>/Switch' incorporates:
     *  Constant: '<S7>/Constant'
     *  DataTypeConversion: '<S8>/DataTypeConv2'
     *  Logic: '<S8>/AND'
     *  RelationalOperator: '<S8>/Equal'
     */
    if (rtb_NotEqual && ((tmp < 0.0F ? (int16_T)(int8_T)-(int8_T)(uint8_T)-tmp :
                          (int16_T)(int8_T)(uint8_T)tmp) ==
                         rtb_SignPreIntegrator_0)) {
        rtb_UD = localP->Constant_Value_j;
    }

    /* End of Switch: '<S7>/Switch' */

    /* Update for DiscreteIntegrator: '<S7>/Integrator' */
    localDW->Integrator_DSTATE += localP->Integrator_gainval * rtb_UD;

    /* Update for DiscreteIntegrator: '<S7>/Filter' */
    localDW->Filter_DSTATE += localP->Filter_gainval * rtb_FilterCoefficient;

    /* Update for DiscreteIntegrator: '<S10>/Integrator' */
    localDW->Integrator_DSTATE_b += localP->Integrator_gainval_e * rtb_ZeroGain;

    /* Update for Delay: '<S12>/UD' */
    localDW->UD_DSTATE = rtb_TSamp;

    /* Signum: '<S15>/SignPreIntegrator' */
    if (rtb_IntegralGain < 0.0F) {
        rtb_ZeroGain_oq = -1.0F;
    } else if (rtb_IntegralGain > 0.0F) {
        rtb_ZeroGain_oq = 1.0F;
    } else {
        rtb_ZeroGain_oq = rtb_IntegralGain;
    }

    /* DataTypeConversion: '<S15>/DataTypeConv2' incorporates:
     *  Signum: '<S15>/SignPreIntegrator'
     */
    tmp = fmodf(rtb_ZeroGain_oq, 256.0F);

    /* DataTypeConversion: '<S15>/DataTypeConv1' */
    if (rtb_SignPreIntegrator < 128.0F) {
        rtb_SignPreIntegrator_0 = (int8_T)rtb_SignPreIntegrator;
    } else {
        rtb_SignPreIntegrator_0 = MAX_int8_T;
    }

    /* End of DataTypeConversion: '<S15>/DataTypeConv1' */

    /* Switch: '<S13>/Switch' incorporates:
     *  Constant: '<S13>/Constant'
     *  DataTypeConversion: '<S15>/DataTypeConv2'
     *  Logic: '<S15>/AND'
     *  RelationalOperator: '<S15>/Equal'
     */
    if (rtb_NotEqual_m0 && ((tmp < 0.0F ? (int16_T)(int8_T)-(int8_T)(uint8_T)
                             -tmp : (int16_T)(int8_T)(uint8_T)tmp) ==
                            rtb_SignPreIntegrator_0)) {
        rtb_IntegralGain = localP->Constant_Value_g;
    }

    /* End of Switch: '<S13>/Switch' */

    /* Update for DiscreteIntegrator: '<S13>/Integrator' */
    localDW->Integrator_DSTATE_h += localP->Integrator_gainval_l *
        rtb_IntegralGain;

    /* Update for Delay: '<S16>/UD' */
    localDW->UD_DSTATE_c = rtb_TSamp_f;
}

/* Model step function */
void Flugregler_step(void)
{
    /* Outputs for Atomic SubSystem: '<Root>/Flugregler' */

    /* Inport: '<Root>/currentAttitude' incorporates:
     *  Inport: '<Root>/barometricAltitude'
     *  Inport: '<Root>/gyro'
     *  Inport: '<Root>/targetHeading'
     *  Inport: '<Root>/targetPitch'
     */
    Flugregler_Flugregler(Flugregler_U.currentAttitude.pitch,
                          Flugregler_U.currentAttitude.courseMagnetic,
                          Flugregler_U.barometricAltitude_m.altitude,
                          Flugregler_U.targetHeading_g.targetRateOfTurn,
                          Flugregler_U.targetHeading_g.justRateOfTurnEnabled,
                          Flugregler_U.targetHeading_g.targetHeading,
                          Flugregler_U.targetPitch_f.targetAltitude,
                          Flugregler_U.targetPitch_f.targetPitch,
                          Flugregler_U.targetPitch_f.justPitchEnabled,
                          Flugregler_U.gyro.z, &Flugregler_B.Flugregler_i,
                          &Flugregler_DW.Flugregler_i,
                          (P_Flugregler_Flugregler_T *)
                          &Flugregler_P.Flugregler_i);

    /* End of Outputs for SubSystem: '<Root>/Flugregler' */

    /* BusCreator: '<Root>/BusConversion_InsertedFor_outputCommandSet_at_inport_0' */
    Flugregler_Y.outputCommandSet.rudder =
        Flugregler_B.Flugregler_i.DataTypeConversion1;
    Flugregler_Y.outputCommandSet.elevator =
        Flugregler_B.Flugregler_i.DataTypeConversion2;
    Flugregler_Y.outputCommandSet.thrust =
        Flugregler_B.Flugregler_i.DataTypeConversion;
}

/* Model initialize function */
void Flugregler_initialize(void)
{
    /* Registration code */

    /* initialize error status */
    rtmSetErrorStatus(Flugregler_M, (NULL));

    /* block I/O */
    (void) memset(((void *) &Flugregler_B), 0,
                  sizeof(B_Flugregler_T));

    /* states (dwork) */
    (void) memset((void *)&Flugregler_DW, 0,
                  sizeof(DW_Flugregler_T));

    /* external inputs */
    (void) memset((void *)&Flugregler_U, 0,
                  sizeof(ExtU_Flugregler_T));

    /* external outputs */
    Flugregler_Y.outputCommandSet = Flugregler_rtZsimulink_outputCommandSet;

    /* InitializeConditions for Atomic SubSystem: '<Root>/Flugregler' */
    Flugregler_Flugregler_Init(&Flugregler_DW.Flugregler_i,
        (P_Flugregler_Flugregler_T *)&Flugregler_P.Flugregler_i);

    /* End of InitializeConditions for SubSystem: '<Root>/Flugregler' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
