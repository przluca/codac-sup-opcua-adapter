/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/cvvf-service/UserSuppliedCode.h $
* $Id: UserSuppliedCode.h 99275 2019-04-25 06:34:12Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Generic type class definition
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*				  CS 90 046
*				  13067 St. Paul-lez-Durance Cedex
*				  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/**
 * @file UserSuppliedCode.h
 * @brief Header file for User supplied code.
 * @date 14/01/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of TBD
 */

#ifndef _UserSuppliedCode_h_
#define _UserSuppliedCode_h_

// Global header files

#include <common/BasicTypes.h>

// Local header files

// Constants

// Type definition

#define TWO_MU 2.51327412287183e-06
#define NUMBER_OF_MLF_SENSORS_PER_IP 24
#define NUMBER_OF_BEST_IPS 6
#define NUMBER_OF_SECTORS 6
#define TOTAL_NUMBER_OF_MLF_SENSORS (NUMBER_OF_MLF_SENSORS_PER_IP * NUMBER_OF_SECTORS)
#define NUMBER_OF_SAMPLES_PER_SENSOR 100
const ccs::types::uint32 VALID_SECTORS[NUMBER_OF_BEST_IPS] = {1, 3, 4, 6, 7, 9};

namespace _55a0 {

namespace types {

// From 55.A0 BestIP use-case 
typedef struct __attribute__((packed)) SensorGeometryMLF {
  ccs::types::float32 angle = 0.f;
  ccs::types::float32 r = 0.f;
  ccs::types::float32 z = 0.f;
} SensorGeometryMLF_t;

typedef struct __attribute__((packed)) SensorInformationMLF {
  SensorGeometryMLF_t geometry;
  ccs::types::uint32 sector;
  ccs::types::string name;
} SensorInformationMLF_t;

typedef SensorInformationMLF_t SensorsInformationMLF_t [NUMBER_OF_MLF_SENSORS_PER_IP];

typedef struct __attribute__((packed)) BestIPSensorsSelectorInput {
  SensorInformationMLF_t sensorsInfo [TOTAL_NUMBER_OF_MLF_SENSORS];
  ccs::types::uint32 selectedSectors [NUMBER_OF_MLF_SENSORS_PER_IP];
} BestIPSensorsSelectorInput_t;

typedef struct __attribute__((packed)) BestIPComputeSensorsErrorsInputData {
  //All sensor data for a given sector is encoded in one array S1[0:NUMBER_OF_SAMPLES_PER_SENSOR]S2[0:NUMBER_OF_SAMPLES_PER_SENSOR]...
  ccs::types::float32 data [NUMBER_OF_SAMPLES_PER_SENSOR * NUMBER_OF_MLF_SENSORS_PER_IP];
} BestIPComputeSensorsErrorsInputData_t;

typedef struct __attribute__((packed)) BestIPComputeSensorsErrorsInput {
  BestIPComputeSensorsErrorsInputData_t sectorIdx [NUMBER_OF_SECTORS];
} BestIPComputeSensorsErrorsInput_t;

typedef struct __attribute__((packed)) BestIPComputeSensorsErrorsOutputData {
  ccs::types::float32 error [NUMBER_OF_MLF_SENSORS_PER_IP];
} BestIPComputeSensorsErrorsOutputData_t;

typedef struct __attribute__((packed)) BestIPComputeSensorsErrorsOutput {
  BestIPComputeSensorsErrorsOutputData_t sectorIdx [NUMBER_OF_SECTORS];
} BestIPComputeSensorsErrorsOutput_t ;

typedef struct __attribute__((packed)) BestIPValidateSensorsInput {
  BestIPComputeSensorsErrorsOutput_t error;
  ccs::types::uint32 selectedSectors [NUMBER_OF_MLF_SENSORS_PER_IP];
} BestIPValidateSensorsInput_t;

typedef struct __attribute__((packed)) BestIPWeightsComputationInput {
  SensorGeometryMLF_t geometry;
  ccs::types::uint8 enabled = 0; // In absence of boolean support in MARTe
} BestIPWeightsComputationInput_t;

typedef BestIPWeightsComputationInput_t BestIPWeightsComputationInputs_t [NUMBER_OF_MLF_SENSORS_PER_IP];

typedef ccs::types::float32 BestIPWeightsComputationOutputs_t [NUMBER_OF_MLF_SENSORS_PER_IP];

typedef struct __attribute__((packed)) BestIPComputeIPInput {
  ccs::types::float32 weights[NUMBER_OF_MLF_SENSORS_PER_IP];
  ccs::types::float32 data[NUMBER_OF_MLF_SENSORS_PER_IP * NUMBER_OF_SAMPLES_PER_SENSOR];
} BestIPComputeIPInput_t;
        
typedef ccs::types::float32 BestIPComputeIPOutput_t [NUMBER_OF_SAMPLES_PER_SENSOR];

typedef struct __attribute__((packed)) BestIPValidateWeightsInputs {
  ccs::types::uint32 minValidIPs;
  ccs::types::float32 bestIP0[NUMBER_OF_SAMPLES_PER_SENSOR];
  ccs::types::float32 bestIP1[NUMBER_OF_SAMPLES_PER_SENSOR];
  ccs::types::float32 bestIP2[NUMBER_OF_SAMPLES_PER_SENSOR];
  ccs::types::float32 bestIP3[NUMBER_OF_SAMPLES_PER_SENSOR];
  ccs::types::float32 bestIP4[NUMBER_OF_SAMPLES_PER_SENSOR];
  ccs::types::float32 bestIP5[NUMBER_OF_SAMPLES_PER_SENSOR];
  ccs::types::float32 bestIP6[NUMBER_OF_SAMPLES_PER_SENSOR];
} BestIPValidateWeightsInputs_t;

typedef struct __attribute__((packed)) BestIPComputeIPErrorsOutputs {
  ccs::types::uint32 bestIP1;
  ccs::types::uint32 bestIP2;
  ccs::types::uint32 bestIP3;
  ccs::types::uint32 bestIP4;
  ccs::types::uint32 bestIP5;
  ccs::types::uint32 bestIP6;
} BestIPComputeIPErrorsOutputs_t;

typedef struct __attribute__((packed)) Config { 
  ccs::types::uint8 ignored;
} Config_t;

} // namespace types

// Global variables

// Function declaration

namespace cvvf {

using namespace ::_55a0::types;

/**
 * @TODO
 */
bool BestIPComputeIPErrors(Config_t* config, BestIPValidateWeightsInputs_t *input, BestIPComputeIPErrorsOutputs_t *output);

/**
 * @TODO
 */
bool BestIPValidateWeights(Config_t* config, BestIPValidateWeightsInputs_t *input);

/**
 * @TODO
 */
bool BestIPComputeIP (Config_t* config, BestIPComputeIPInput_t *input, BestIPComputeIPOutput_t *output);

/**
 * @TODO
 */
bool BestIPValidateSensors (Config_t* config, BestIPValidateSensorsInput_t *input);

/**
 * @TODO
 */
bool BestIPComputeSensorsErrors (Config_t* config, BestIPComputeSensorsErrorsInput_t *input, BestIPComputeSensorsErrorsOutput_t *output);

/**
 * @TODO
 */
bool BestIPSensorsSelector (Config_t* config, BestIPSensorsSelectorInput_t* input, BestIPWeightsComputationInputs_t* output); // Old and hugly type erasure .. Is eventually called with void*

/**
 * @brief computes intermediate values needed by ComputeIPWeights
 * @details perform the following operation sqrt((r2-r1)*(r2-r1)+(z2-z1)*(z2-z1)), where r and d are the sensor positions
 * param r1 [in] is the distance from the torus center of the first operative coil involved in the weight calculation
 * param z1 [in] is the height of the first operative coil involved in the weight calculation
 * param r2 [in] is the distance from the torus center of the second operative coil involved in the weight calculation
 * param z2 [in] is the height of the second operative coil involved in the weight calculation
**/
ccs::types::float32 _BestIPComputeDL(ccs::types::float32 r1, ccs::types::float32 z1, ccs::types::float32 r2, ccs::types::float32 z2);

/**
 * @brief Given the sensor distribution of one sector calculates the sensor weights
 * @param config [in] configuration of the function. Currently unused.
 * @param input [in] information of the sensor position used to calculate the weights
 * @param output [out] the calculated weights
 * @return true always
**/
bool BestIPWeightsComputation (Config_t* config, BestIPWeightsComputationInputs_t* input, BestIPWeightsComputationOutputs_t* output);


// Function definition

} // namespace cvvf

} // namespace _55a0

#endif // _UserSuppliedCode_h_ 
