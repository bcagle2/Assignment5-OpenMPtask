#!/bin/bash
#PBS -j oe
cd ~/assignment_5_brittany_cagle/4
dir
#delete output files
#set -x
file='output54.txt'
#if output files exist delete them
if [ -f $file ]  
then rm $file
fi 
file='output54.csv'
if [ -f $file ]  
then rm $file
fi 
file='output.detail54.txt'
#if output files exist delete them
if [ -f $file ]  
then rm $file
fi 
file='output.detail54.csv'
if [ -f $file ]  
then rm $file
fi 
#now run the program - exit ends early
#           intensity	intensity factor	max# threads	
#./a54.exe 100000000 5 4		54 
#./a54.exe 100000000 5 16		54
#exit 0
./a54.exe 50000000 5 16		54
./a54.exe 100000000 5 16	54
./a54.exe 150000000 5 16	54
./a54.exe 500000000 5 16	54
./a54.exe 1000000000 5 16	54
exit 0