#!/bin/bash
#PBS -j oe
cd ~/assignment_5_brittany_cagle/3
dir
#delete output files
#set -x
file='output53.txt'
#if output files exist delete them
if [ -f $file ]  
then rm $file
fi 
file='output53.csv'
if [ -f $file ]  
then rm $file
fi 
#now run the program - exit ends early
#       arraySize   #threads   schedType   outputSuf   displayOption
#./a53.exe 1000000 1    static      53   
#./a53.exe 1000000 2    static      53   
#./a53.exe 1000000 4    static      53   
#./a53.exe 1000000 8    static      53   
#./a53.exe 1000000 16   static      53 
#exit 0
./a53.exe 1000000000 1    static      53   
./a53.exe 1000000000 2    static      53   
./a53.exe 1000000000 4    static      53   
./a53.exe 1000000000 8    static      53   
./a53.exe 1000000000 16   static      53   
#

exit 0
