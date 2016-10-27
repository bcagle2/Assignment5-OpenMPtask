#!/bin/bash
#PBS -j oe
cd ~/assignment_5_brittany_cagle/1
dir
#delete output files
#set -x
file='output51seq.txt'
#if output files exist delete them
if [ -f $file ]  
then rm $file
fi 
file='output51seq.csv'
if [ -f $file ]  
then rm $file
fi 
#now run the program - exit ends early
#       arraySize   #threads   schedType   outputSuf   intensity      displayOption
#./a51seq.exe 1000000 1    static      51seq  0 
#exit 0
./a51seq.exe 100000000 1    static      51seq  0 
exit 0
