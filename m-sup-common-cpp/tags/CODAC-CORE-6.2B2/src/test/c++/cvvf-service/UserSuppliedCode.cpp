/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/cvvf-service/UserSuppliedCode.cpp $
* $Id: UserSuppliedCode.cpp 99275 2019-04-25 06:34:12Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
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

// Global header files

#include <common/BasicTypes.h> // Misc. type definition

#include <common/SysTools.h> // ccs::HelperTools::SafeStringCopy, etc.
#include <common/log-api.h> // Logging helper routines

#include <math.h>
// Local header files

#include "UserSuppliedCode.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "55a0::cvvf"

// Type definition

namespace _55a0 {

namespace cvvf {

// Function declaration

// Global variables

// Function definition

static bool SectorToIndex(ccs::types::uint32 sector, ccs::types::uint32 &relativeIdx) {
    relativeIdx = 0u;
    bool found = false;
    for (relativeIdx = 0u; (relativeIdx < NUMBER_OF_BEST_IPS) && (!found); relativeIdx += 1u) {
        found = (sector == VALID_SECTORS[relativeIdx]);
    }
    if (found) {
        relativeIdx -= 1u;
    }
    else {
        log_error("Invalid sector specified: %d", sector);
    }
    return found;
}

ccs::types::float32 _BestIPComputeDL (ccs::types::float32 r1, ccs::types::float32 z1, ccs::types::float32 r2, ccs::types::float32 z2){
	ccs::types::float64 aux = (r2-r1)*(r2-r1)+(z2-z1)*(z2-z1);
	ccs::types::float64 aux2 = sqrt(aux);
	return static_cast<ccs::types::float32>(aux2);
}

static bool ValidateBestIP (ccs::types::float32 *bestIPRef, ccs::types::float32 *bestIP) {
	bool ok = true;
    //TODO move this to the config
    static const ccs::types::float64 MAX_ERROR = 500000000000;
    ccs::types::float64 err = 0.0;
	for(ccs::types::int32 t = 0; (t < NUMBER_OF_SAMPLES_PER_SENSOR) && ok; t ++){
        ccs::types::float32 squareDiff = (bestIPRef[t] - bestIP[t]);
        squareDiff = squareDiff * squareDiff;
        err += squareDiff;
    }
    err /= NUMBER_OF_SAMPLES_PER_SENSOR;
    ok = (err < MAX_ERROR);
    if (!ok) {
        log_info("Validate BestIP failed: %f > %f", err, MAX_ERROR);
    }
    return ok;
}

bool BestIPComputeIPErrors(Config_t* config, BestIPValidateWeightsInputs_t *input, BestIPComputeIPErrorsOutputs_t *output) {
	bool ok = true;
    //Validate BestIPs
    output->bestIP1 = (ValidateBestIP(input->bestIP0, input->bestIP1) ? 1 : 0);
    output->bestIP2 = (ValidateBestIP(input->bestIP0, input->bestIP2) ? 1 : 0);
    output->bestIP3 = (ValidateBestIP(input->bestIP0, input->bestIP3) ? 1 : 0);
    output->bestIP4 = (ValidateBestIP(input->bestIP0, input->bestIP4) ? 1 : 0);
    output->bestIP5 = (ValidateBestIP(input->bestIP0, input->bestIP5) ? 1 : 0);
    output->bestIP6 = (ValidateBestIP(input->bestIP0, input->bestIP6) ? 1 : 0);
    return ok;
}


bool BestIPValidateWeights(Config_t* config, BestIPValidateWeightsInputs_t *input) {
	bool ok = true;
    ccs::types::uint32 validIPs = 0;
    //Validate BestIPs
    ok = ValidateBestIP(input->bestIP0, input->bestIP1);
    if (ok) {
        validIPs++;
    }
    if (!ok) {
        log_info("BestIP 1 has failed the validation!");
    }
    ok = ValidateBestIP(input->bestIP0, input->bestIP2);
    if (ok) {
        validIPs++;
    }
    if (!ok) {
        log_info("BestIP 2 has failed the validation!");
    }
    ok = ValidateBestIP(input->bestIP0, input->bestIP3);
    if (ok) {
        validIPs++;
    }
    if (!ok) {
        log_info("BestIP 3 has failed the validation!");
    }
    ok = ValidateBestIP(input->bestIP0, input->bestIP4);
    if (ok) {
        validIPs++;
    }
    if (!ok) {
        log_info("BestIP 4 has failed the validation!");
    }
    ok = ValidateBestIP(input->bestIP0, input->bestIP5);
    if (ok) {
        validIPs++;
    }
    if (!ok) {
        log_info("BestIP 5 has failed the validation!");
    }
    ok = ValidateBestIP(input->bestIP0, input->bestIP6);
    if (ok) {
        validIPs++;
    }
    if (!ok) {
        log_info("BestIP 6 has failed the validation!");
    }

    ok = (validIPs >= input->minValidIPs);
    if (!ok) {
        log_info("No sufficient valid IPs were found!");
    }
    return ok;
}

bool BestIPComputeIP (Config_t* config, BestIPComputeIPInput_t *input, BestIPComputeIPOutput_t *output) {
    ccs::types::float32 accum;
	for(ccs::types::int32 t = 0; t < NUMBER_OF_SAMPLES_PER_SENSOR; t++){
		accum = 0.0;
		for(int s = 0; s < NUMBER_OF_MLF_SENSORS_PER_IP; s++){
			accum += input->weights[s] * input->data[s * NUMBER_OF_SAMPLES_PER_SENSOR + t]; 
		}
		(*output)[t] = accum;
	}

    return true;
}

bool BestIPSensorsSelector (Config_t* config, BestIPSensorsSelectorInput_t* input, BestIPWeightsComputationInputs_t* output) { 
    bool ok = true;
    log_info("55a0::cvvf::BestIPSensorsSelector::Process - Method called .. ");
    for (ccs::types::uint32 index = 0u; (index < NUMBER_OF_MLF_SENSORS_PER_IP) && (ok); index += 1u) {
        if (input->selectedSectors[index] == 0) {
            (*output)[index].enabled = 0;
        }
        else {
            ccs::types::uint32 sector = input->selectedSectors[index];
            ccs::types::uint32 relativeIdx = 0u;
            ok = SectorToIndex(sector, relativeIdx);
            log_info("For sensor at index %d allocating sector %d [%d]", index, sector, relativeIdx);
            if (ok) {
                (*output)[index].enabled = 1;
                ccs::types::uint32 allSensorsInfoIndex = index;
                allSensorsInfoIndex += relativeIdx * NUMBER_OF_MLF_SENSORS_PER_IP;
                (*output)[index].geometry.r = input->sensorsInfo[allSensorsInfoIndex].geometry.r;
                (*output)[index].geometry.z = input->sensorsInfo[allSensorsInfoIndex].geometry.z;
                (*output)[index].geometry.angle = input->sensorsInfo[allSensorsInfoIndex].geometry.angle;
            }
            else {
                (*output)[index].enabled = 0;
            }
        }
    }

    return ok;

}

bool BestIPWeightsComputation (Config_t* config, BestIPWeightsComputationInputs_t* input, BestIPWeightsComputationOutputs_t* output) {
    log_info("55a0::cvvf::BestIPWeightsComputation::Process - Method called .. ");
    ccs::types::int32 firstIndex;
    ccs::types::int32 nextIndex;
    ccs::types::float32 dl1 = 0.0;
    ccs::types::float32 dl2 = 0.0;
    for (ccs::types::int32 i = 0; i < NUMBER_OF_MLF_SENSORS_PER_IP; i++) {
        if((*input)[i].enabled) {
            firstIndex = i;
            // Get the next enable index 
            nextIndex = i + 1;
            if(nextIndex == NUMBER_OF_MLF_SENSORS_PER_IP) {
                nextIndex = 0;
            }
            while((!(*input)[nextIndex].enabled) && (nextIndex != firstIndex)) {
                nextIndex ++;
                if (nextIndex == NUMBER_OF_MLF_SENSORS_PER_IP) {
                    nextIndex = 0;
                }
            }//end get next index
            if(nextIndex != firstIndex) {//if not a complete cycle is done then compute dli
                dl1 = _BestIPComputeDL((*input)[firstIndex].geometry.r, (*input)[firstIndex].geometry.z, (*input)[nextIndex].geometry.r, (*input)[nextIndex].geometry.z); 
                firstIndex = nextIndex;
                nextIndex = firstIndex + 1;
                if(nextIndex == NUMBER_OF_MLF_SENSORS_PER_IP) {
                    nextIndex = 0;
                }
                while((!(*input)[nextIndex].enabled) && (nextIndex != firstIndex)) {
                    nextIndex ++;
                    if (nextIndex == NUMBER_OF_MLF_SENSORS_PER_IP) {
                        nextIndex = 0;
                    }
                }
                dl2 = _BestIPComputeDL((*input)[firstIndex].geometry.r, (*input)[firstIndex].geometry.z, (*input)[nextIndex].geometry.r, (*input)[nextIndex].geometry.z);
                (*output)[i]=  -(dl1+dl2)/TWO_MU;
            }
        }
        else {
            (*output)[i]= 0.0;
        }
	}
    log_info("55a0::cvvf::BestIPWeightsComputation::Process - Method finished.. ");
	return true;
}

bool BestIPComputeSensorsErrors (Config_t* config, BestIPComputeSensorsErrorsInput_t *input, BestIPComputeSensorsErrorsOutput_t *output){
	bool ok = true;
    for(ccs::types::int32 sensorPosition = 0;  (sensorPosition < NUMBER_OF_MLF_SENSORS_PER_IP) && ok; sensorPosition++){
        for(ccs::types::int32 t = 0; (t < NUMBER_OF_SAMPLES_PER_SENSOR) && ok; t ++){
			ccs::types::float32 accum = 0;
			for(ccs::types::int32 sector = 0; sector < NUMBER_OF_SECTORS; sector++){
                accum += input->sectorIdx[sector].data[sensorPosition * NUMBER_OF_SAMPLES_PER_SENSOR + t];
			}
			ccs::types::float32 avgSensorPosition = 0;
            avgSensorPosition = accum/NUMBER_OF_SECTORS;
            //printf("avgSensorPosition = %f\n", avgSensorPosition);
            accum = 0;
            for(ccs::types::int32 sector = 0; sector < NUMBER_OF_SECTORS;sector++){
                ccs::types::float32 squareDiff = (input->sectorIdx[sector].data[sensorPosition * NUMBER_OF_SAMPLES_PER_SENSOR + t] - avgSensorPosition);
                squareDiff = squareDiff * squareDiff;
                accum += squareDiff;
            }
            for(ccs::types::int32 sector = 0; (sector < NUMBER_OF_SECTORS) && ok; sector++){
                ccs::types::float32 diff = avgSensorPosition;
                diff -= input->sectorIdx[sector].data[sensorPosition * NUMBER_OF_SAMPLES_PER_SENSOR + t];
                ccs::types::float32 diffAbs = fabs(diff);
                if(output->sectorIdx[sector].error[sensorPosition] < diffAbs) {
                    output->sectorIdx[sector].error[sensorPosition] = diffAbs;
                }
            }
        }
    }
	return ok;
}

bool BestIPValidateSensors (Config_t* config, BestIPValidateSensorsInput_t *input) {
    //TODO move this to the config
    static const ccs::types::float32 MAX_ERROR = 0.4;
    bool ok = true;
    for (ccs::types::int32 i = 0; (i < NUMBER_OF_MLF_SENSORS_PER_IP) && (ok); i++) {
        if (input->selectedSectors[i] != 0) {
            ccs::types::uint32 sector = input->selectedSectors[i];
            ccs::types::uint32 relativeIdx = 0u;
            ok = SectorToIndex(sector, relativeIdx);
            if (ok) {
                ok = (input->error.sectorIdx[relativeIdx].error[i] < MAX_ERROR);
                if (!ok) {
                    log_info("Sensor at position %d in sector %d is likely to be broken and it is enabled (%f > %f)!", i, input->selectedSectors[i], input->error.sectorIdx[relativeIdx].error[i], MAX_ERROR);
                }
            }
        }
    }
    return ok;
}

} // namespace cvvf

} // namespace _55a0

#undef LOG_ALTERN_SRC
