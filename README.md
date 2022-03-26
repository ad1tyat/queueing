# CS544 Topics in Networks
## Queueing in Packet Switches

### Team Members :
1. Aditya Trivedi, 190101005
2. Atharva Vijay Varde, 190101018

## Steps for compilation

```
g++ routing.cpp
```

## Steps for running

Some sample execution commands : 

```
./a.out -N 5 -B 4 -p 0.5 -queue INQ -T 10000
./a.out -N 15 -B 4 -p 1.0 -queue ISLIP -T 10000
./a.out -N 25 -B 4 -p 1.0 -K 0.8 -queue KOUQ -T 10000
```

## Details about command line options

- -N : Number of Ports `default=8`
- -B : Buffer Size `default=4`
- -queue : Queue type (INQ/KOUQ/ISLIP) `default=INQ`
- -out : Output File `default=output.txt`
- -T : Number of time slots `default=10000`
- -K : knockout `default=0.6`

## File details

1. routing.cpp : It contains the main source code for simulation
2. routing.h : It contains the utility functions and data structure definitions
3. run.sh : It is a bash script to run the code multiple times (optional)
4. pics/ : It contains all the figures used in report
5. scripts/ : It contains all the scripts used to generate graphs
6. outputs/ : It contains all the output files
7. Makefile : It is used to clean, compile and execute the files

## Files produced in output

1. a.out : It is the object code produced after compilation
2. output.txt : It contains the statistics produced after simulation completes
3. debug.txt : It contains debug messages

## Reading the output file

The statistics in output file are in following format (separated by tabs):

NUMBER_OF_PORTS PACKETGENPROB QUEUE_TYPE AVG_DELAY STD_DEV_DELAY LINK_UTIL KOUQ_DROP_PROB

**Note** KOUQ_DROP_PROB is only printed when the queue is KOUQ

## Assumptions 
- The output buffer (in case of KOUQ) can only accommodate up to `B` packets. 
- The total size of input buffer, including all virtual output queues is `B` (in case of ISLIP).