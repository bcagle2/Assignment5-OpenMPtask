#!/bin/bash
#PBS -j oe
cd ~/assignment_5_brittany_cagle/1
dir
#delete output files
#set -x
file='output51.txt'
#if output files exist delete them
if [ -f $file ]  
then rm $file
fi 
file='output51.csv'
if [ -f $file ]  
then rm $file
fi 
#now run the program - exit ends early
#       arraySize   #threads   schedType   outputSuf   intensity      displayOption
#
./a51.exe 100000000 1    dynamic1000      51  0 
./a51.exe 100000000 2    dynamic1000      51  0 
./a51.exe 100000000 4    dynamic1000      51  0 
./a51.exe 100000000 8    dynamic1000      51  0 
./a51.exe 100000000 16   dynamic1000      51  0 
#
./a51.exe 100000000 1    dynamic100000    51  0 
./a51.exe 100000000 2    dynamic100000    51  0 
./a51.exe 100000000 4    dynamic100000    51  0 
./a51.exe 100000000 8    dynamic100000    51  0 
./a51.exe 100000000 16   dynamic100000    51  0 
exit 0
