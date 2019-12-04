#!/bin/bash

#Configuration Parameters
counter=0
nOfTests=20

#Color settings
RED='\033[1;31m'
GREEN='\033[1;32m'
NC='\033[0m'

#Test loop
echo -e "${GREEN}PLC METHOD TEST: STARTED!"
for((i = 0 ; i < $nOfTests ; i++)); do
        echo -e "${RED}Configuration number $counter ${NC}"
	#Changing values on SCUs JSON
        sed -i 's/:'$counter'/:'$((counter+1))'/g' scus.json
	#Calling RPC Client
        ${CODAC_ROOT}/bin/cfg-client --service 55a0::Cfg::Interface --config-file scus.json --verbose
	sleep 4 
        ((counter = counter + 1))
	#SCUs parameters are uint8
        if [[ $counter -eq 256 ]]; then
                ((counter = counter - 256))
        fi
	#Restore default configuration at the end of the test sequence
	if [[ $counter -eq $nOfTests ]]; then
		sed -i 's/:'$counter'/:0/g' scus.json
	fi
done
echo -e "${GREEN}TEST COMPLETED!${NC}"
