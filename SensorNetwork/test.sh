#!/bin/bash

echo "Cleaning..."
make clean-all
rm -f output_* log*.msg gateway.log sensor1 sensor2 sensor3

echo "******************************************"
echo "*** Compiling with non default defines ***"
echo "******************************************"
make gateway DEFINES="-DSET_MIN_TEMP=10 -DSET_MAX_TEMP=15 -DSET_QUEUE_SIZE=10"
for i in 1 2 3 ; do
  rm -f sensor.o
  make sensor DEFINES="-DSET_ID=$i -DSET_FREQUENCY=1 -DSET_QUEUE_SIZE=10"
  mv ./sensor ./sensor$i
done
make log DEFINES="-DSET_QUEUE_SIZE=10"

echo
echo "***************************************"
echo "*** Running Gateway and SensorNodes ***"
echo "***************************************"
procs=("log" "gateway" "sensor1" "sensor2" "sensor3")
# *** For-loop will redirect all output to files
#params=("" "1234" "127.0.0.1 1234" "127.0.0.1 1234" "127.0.0.1 1234")
#timeout=(12 10 5 5 5)
#for ((i=0; i < ${#procs[@]}; i++)); do
#  echo "Starting '${procs[i]} ${params[i]}'  for ${timeout[i]}s"
#  timeout ${timeout[i]}s ./${procs[i]} ${params[i]} &>output_${procs[i]} &
#  pids[$i]=$!
#done

# *** Gateway prints on stdout, all others redirect their output to a file
echo "Starting 'log'  for 15s, redirecting output to file"
timeout 15s ./log &>output_log &
pids[0]=$!
echo "Starting 'gateway 1234'  for 14s"
timeout 14s ./gateway 1234 &
pids[1]=$!
echo "Starting 'sensor1 127.0.0.1 1234'  for 12s, redirecting output to file"
timeout 12s ./sensor1 127.0.0.1 1234 &>output_sensor1 &
pids[2]=$!
echo "Starting 'sensor2 127.0.0.1 1234'  for 12s, redirecting output to file"
timeout 12s ./sensor2 127.0.0.1 1234 &>output_sensor2 &
pids[3]=$!
echo "Starting 'sensor3 127.0.0.1 1234'  for 12s, redirecting output to file"
timeout 12s ./sensor3 127.0.0.1 1234 &>output_sensor3 &
pids[4]=$!

for ((i=0; i < ${#procs[@]}; i++)); do
  wait ${pids[i]}
  if [ $? -ne 124 ]; then #every process should timeout
    status[$i]="Terminated before timeout"
  else
    status[$i]="Kept runing ok"
  fi
done

echo
echo "**********************************"
echo "*** Basic Evaluation of output ***"
echo "**********************************"
echo
for ((i=0; i < ${#procs[@]}; i++)); do
  echo "*** ${procs[i]} : ${status[i]} ***"
done
echo
if [[ -z $(grep "SET_ID" sensor.h) ]]; then
  echo "ERROR : SET_ID not found in sensor.h"
fi
if [[ -z $(grep "SET_FREQUENCY" sensor.h) ]]; then
  echo "ERROR : SET_FREQUENCY not found in sensor.h"
fi
if [[ -z $(grep "SET_MIN_TEMP" gateway.h) ]]; then
  echo "ERROR : SET_MIN_TEMP not found in gateway.h"
fi
if [[ -z $(grep "SET_MAX_TEMP" gateway.h) ]]; then
  echo "ERROR : SET_MAX_TEMP not found in gateway.h"
fi
for i in 1 2 3 ; do
  if [ ! -s log$i.msg ] ; then
    echo "ERROR : No (or empty) log file for SensorNode $i"
  else
    if [[ $(sed -n $= log$i.msg) < 10 ]]; then
      echo "ERROR : Log file to small for SensorNode $i"
    fi
  fi
done
if [ ! -s gateway.log ] ; then
  echo "ERROR : No (or empty) log file for Gateway $i"
else
  if [[ $(sed -n $= gateway.log) < 5 ]]; then
    echo "ERROR : Log file to small for Gateway"
  fi
fi
