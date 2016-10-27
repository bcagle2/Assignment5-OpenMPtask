#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <sys/time.h>
#include <cmath>
#include <omp.h>

using namespace std;

/* Global variables */

bool myFlow;
bool myDebug;
bool dispStat;


/* Function declarations */
int getMin(int[],int,int,int);
void flowMsg(string);
void checkSuccessors(int);
int task2Int(string);
void statusDisplay(string);


/*********************   mainline *************************/

int main(int argc, char *argv[])
{
    myFlow   = false;
    myDebug  = false;
    dispStat = false;

    flowMsg("in Mainline");


          /*  if argument 1 is null, print out an error message and end the program */
    if (argv[1] == NULL
    ||  argv[2] == NULL
    ||  argv[3] == NULL
    ||  argv[4] == NULL
    ||  argv[5] == NULL)
    {
      cout << "The argument is null. Three arguments are required" << endl;
      cout << "arg1 is size of array." << endl;
      cout << "arg2 is the number of threads" <<endl;
      cout << "arg3 is the Schedule Type:  static   dynamic1  dynamic1000 dynamic100000" << endl;
      cout << "arg4 is the output suffix.  ie if 41 then output file names will be output41.txt, etc" << endl;
      cout << "arg5 is the intensity (not used for part 1" << endl;
      cout << "arg6 is optional to run on debugging features any of:  flow status debug" << endl;
      exit(0);
    }

         /* now get len of the array */
    int arraySize;
    stringstream arg1;  /* length of array */
    arg1 << argv[1];
    arg1 >> arraySize;

             /* now get #threads */
    int numThreads;
    stringstream arg2;  /* length of array */
    arg2 << argv[2];
    arg2 >> numThreads;

        // check for optional parms of status, flow, or debug
    string schedType;
    stringstream arg3;
    arg3 << argv[3];
    arg3 >> schedType;

          /*  if argument 1 is null, print out an error message and end the program */
   if (schedType != "static"
   &&  schedType != "dynamic1"
   &&  schedType != "dynamic1000"
   &&  schedType != "dynamic100000")
   {
     cout << "Argument 3 - schedule type is invalid" << endl;
     cout << "arg3 is the Schedule Type:  static   dynamic1  dynamic1000 dynamic100000" << endl;
     exit(0);
   }
    // check for optional parms of status, flow, or debug
    string fileSuffix;
    stringstream arg4;
    arg4 << argv[4];
    arg4 >> fileSuffix;

    int intensity;
    stringstream arg5;  /* length of array */
    arg5 << argv[5];
    arg5 >> intensity;

    // check for optional parms of status, flow, or debug
    string prmOption;
    stringstream arg6;
    arg6 << argv[6];
    arg6 >> prmOption;
                        // parm contains status
    if (prmOption.find("status") != std::string::npos)
        dispStat = true;

                            // parm contains status
    if (prmOption.find("flow") != std::string::npos)
        myFlow = true;

    if (prmOption.find("debug") != std::string::npos)
        myDebug = true;


    statusDisplay("Allocating the array");

    int* myArray = NULL;   // Pointer to int array
    myArray = new int[arraySize];

    srand(25);       /* Use a seed so random numbers will be */
                     /* the same for each test */

                     // set the array to random numbers
    for (int i=0; i<arraySize; i++)
    {
       myArray[i] = rand();
    }


    int minVal = 999999999;

               /* get the starting time */
    /*time_t startTime,endTime;*/
    timeval curTime;
    gettimeofday(&curTime, NULL);
    double startTime = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    flowMsg("    start loop");


    minVal = getMin(myArray,0,(arraySize-1),numThreads);
 
    statusDisplay("Array loop finished");

    flowMsg("  Mainline -  Loop done");

                     /* get the end time */
    /*time(&endTime);
    double elapse = difftime(endTime,startTime);*/
    gettimeofday(&curTime, NULL);
    double endTime = curTime.tv_sec + (curTime.tv_usec / 1000000.0);
    double elapse = endTime - startTime;


               /* first write to console */
    cout << " arraySize=" << arraySize
        << " numThreads=" << numThreads
        << " schedule type=" << schedType
        << " minimum was=" << minVal
        << " and it took " << elapse << " seconds" << endl;


              /* write same thing to output text file */
    string fileName;
    fileName = "output" + fileSuffix + ".txt";
    freopen(fileName.c_str(),"a",stdout);    /* redirect output to the specified filename - append to existing file */

               /* first write to then to file */
    cout << " arraySize=" << arraySize
        << " numThreads=" << numThreads
        << " schedule type=" << schedType
        << " minimum was=" << minVal
        << " and it took " << elapse << " seconds" << endl;

             /* now build a comma delimited .csv file */
    //freopen("output22.csv","a",stdout);    /* redirect output to output.csv - append to existing file */
    fileName = "output" + fileSuffix + ".csv";
    freopen(fileName.c_str(),"a",stdout);    /* redirect output to the specified filename - append to existing file */

    cout << arraySize
       << "," << numThreads
       << "," << schedType
       << "," << elapse << endl;

}


void flowMsg(string inString)
{
    if (myFlow)
        cout << inString << endl;
    return;
}   // end of flowMsg



void statusDisplay(string inMsg)
{
    if (!dispStat)
      return;
    cout << inMsg << endl;
    return;
}

//***********   routine to get the min value in an array.  recursive calls
int getMin(int inArray[], int inStart, int inLast, int inNumThread)
{
    int half;
    int min1;
    int min2;

    if (inStart == inLast)
    {                      //  down to one
        return inArray[inStart];
    }

    if ((inLast - inStart) == 1)
    {                      //  down to one pair
        if (inArray[inStart] < inArray[inLast])
        {
            return inArray[inStart];
        } else
        {
            return inArray[inLast];
        }
    }

    if (inLast > inStart)
    {
        half = (inStart + inLast) / 2;

            // just do recursive getMin in this thread
        min1 = getMin(inArray,inStart,half,1);
	min2 = getMin(inArray,(half + 1),inLast,1);

        if (min1 < min2)
        {
            return min1;
        } else
        {
            return min2;
        }
    }   //   end of if inlast > instart
}
