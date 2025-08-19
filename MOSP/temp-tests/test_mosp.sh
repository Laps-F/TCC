#!/bin/bash

MCL_VALUES=(100 500 1000 2000 5000 10000 50000)
PTL_VALUES=(100 200 400 600)
TEMP_UPDATE_VALUES=(30 60 120 180)

for MCL in "${MCL_VALUES[@]}"; do
  for PTL in "${PTL_VALUES[@]}"; do
    for TEMP_UPDATE in "${TEMP_UPDATE_VALUES[@]}"; do
      echo "Rodando com MCL=$MCL, PTL=$PTL, TEMP_UPDATE=$TEMP_UPDATE"
      ./mainMOSP "../Frinhani/Instances/Random-1000-1000-28-10.txt" \
        --TEMP_INIT 0.1 \
        --TEMP_FIM 20 \
        --N_REPLICAS 7 \
        --MCL $MCL \
        --PTL $PTL \
        --TEMP_DIST 3 \
        --TYPE_UPDATE 1 \
        --TEMP_UPDATE $TEMP_UPDATE \
        --THREAD_USED 7 \
        --READ 1 \
        > "test_results_2/res_MCL${MCL}_PTL${PTL}_TEMP${TEMP_UPDATE}.txt"
    done
  done
done