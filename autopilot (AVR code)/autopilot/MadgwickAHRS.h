//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h

#include "avrlibtypes.h"

//----------------------------------------------------------------------------------------------------
// Variable declaration

extern float beta;				// algorithm gain
extern float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame
extern u32 lastUpdate;

//---------------------------------------------------------------------------------------------------
// Function declarations

void MadgwickAHRSupdate(u32 currentTime, float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void MadgwickAHRSupdateIMU(u32 currentTime, float gx, float gy, float gz, float ax, float ay, float az);

void getYawPitchRollDegrees(float* yaw, float* pitch, float* roll);

#endif
//=====================================================================================================
// End of file
//=====================================================================================================
