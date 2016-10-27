#!/bin/bash
#PBS -j oe
cd ~/assignment_5_brittany_cagle/2
dir
#delete output files
#set -x
file='output52.txt'
#if output files exist delete them
if [ -f $file ]  
then rm $file
fi 
file='output52.csv'
if [ -f $file ]  
then rm $file
fi 
#now run the program - exit ends early
#       arraySize   #threads   schedType   outputSuf   intensity      displayOption
./a52.exe 100000000 1    static      52  0 
./a52.exe 100000000 2    static      52  0 
./a52.exe 100000000 4    static      52  0 
./a52.exe 100000000 8    static      52  0 
./a52.exe 100000000 16   static      52  0 
#
./a52.exe 100000000 1    dynamic1         52  0 
./a52.exe 100000000 2    dynamic1         52  0 
./a52.exe 100000000 4    dynamic1         52  0 
./a52.exe 100000000 8    dynamic1         52  0 
./a52.exe 100000000 16   dynamic1         52  0 
#
./a52.exe 100000000 1    dynamic1000      52  0 
./a52.exe 100000000 2    dynamic1000      52  0 
./a52.exe 100000000 4    dynamic1000      52  0 
./a52.exe 100000000 8    dynamic1000      52  0 
./a52.exe 100000000 16   dynamic1000      52  0 
#
./a52.exe 100000000 1    dynamic100000    52  0 
./a52.exe 100000000 2    dynamic100000    52  0 
./a52.exe 100000000 4    dynamic100000    52  0 
./a52.exe 100000000 8    dynamic100000    52  0 
./a52.exe 100000000 16   dynamic100000    52  0 
exit 0
