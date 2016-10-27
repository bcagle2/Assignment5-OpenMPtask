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
void flowMsg(string);
void statusDisplay(string);
void merge(int[],int[],int,int,int);
void mergeSort(int[], int[], int, int, int);


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
    ||  argv[4] == NULL)
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
    stringstream arg2;
    arg2 << argv[2];
    arg2 >> numThreads;

        // get sched type
    string schedType;
    stringstream arg3;
    arg3 << argv[3];
    arg3 >> schedType;

          /*  check for valid sched type */
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



    // check for optional parms of status, flow, or debug
    string prmOption;
    stringstream arg5;
    arg5 << argv[5];
    arg5 >> prmOption;
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

                     // set the array to sequential number 1-5000
    int adj = 1000000;
    if (adj > arraySize)
    {
        adj = arraySize;
    }
    int j = -1;

    for (int i=0; i<arraySize; i++)
    {
        j = j + 1;
        if (j > adj)
        {
            j = 0;
        }
        myArray[i] = adj - j;
    }

    int* wrkArray = NULL;   // Pointer to int array
    wrkArray = new int[arraySize];


    omp_set_dynamic(0);     // Explicitly disable dynamic #threads
    omp_set_num_threads(numThreads);  // set # of threads to the requested amount
    if (schedType == "static")
    {
        omp_set_schedule(omp_sched_static,0);
    }
    if (schedType == "dynamic1")
    {
        omp_set_schedule(omp_sched_dynamic,1);
    }
    if (schedType == "dynamic1000")
    {
        omp_set_schedule(omp_sched_dynamic,1000);
    }
    if (schedType == "dynamic100000")
    {
        omp_set_schedule(omp_sched_dynamic,100000);
    }


            /* get the starting time */
    /*time_t startTime,endTime;*/
    timeval curTime;
    gettimeofday(&curTime, NULL);
    double startTime = curTime.tv_sec + (curTime.tv_usec / 1000000.0);



    flowMsg("    start first mergeSort call");


    int half;

    half = (arraySize - 1) / 2;

        #pragma omp parallel
    {
        #pragma omp single    //  do thread totals in one thread
        {
            mergeSort(myArray,wrkArray,0,(arraySize - 1),numThreads);
        }
    }

    merge(myArray,wrkArray,0,half,(arraySize - 1));


    /*  past parallel section */
    if (myDebug)
    {
        cout << "print first 100 of array" << endl;
        int numPrt = arraySize;
        if (numPrt > 100)
            numPrt = 100;

        for (int i=0; i<numPrt; i++)
        {
            cout << "   index " << i << " " << myArray[i] << endl;
            if (i > 100)
            {
                break;
            }
        }
    }

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
        << " and it took " << elapse << " seconds" << endl;


              /* write same thing to output text file */
    string fileName;
    fileName = "output" + fileSuffix + ".txt";
    freopen(fileName.c_str(),"a",stdout);    /* redirect output to the specified filename - append to existing file */

               /* then write to then to file */
    cout << " arraySize=" << arraySize
        << " numThreads=" << numThreads
        << " and it took " << elapse << " seconds" << endl;

             /* now build a comma delimited .csv file */
    //freopen("output22.csv","a",stdout);    /* redirect output to output.csv - append to existing file */
    fileName = "output" + fileSuffix + ".csv";
    freopen(fileName.c_str(),"a",stdout);    /* redirect output to the specified filename - append to existing file */

    cout << arraySize
       << "," << numThreads
       << "," << elapse << endl;

}    //  *********    end of mainline **************


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


// routine to do the mergeSort.
//  note that number of threads is bein passed in.  We will divide it each time
//  when we are out of threads (down to 1), we just merge sort & don't do parallel any more
void mergeSort(int inArray[], int wrkArray[], int inStart, int inLast, int inNumThread)
{
    int half;

    if (inLast > inStart)
    {
        half = (inStart + inLast) / 2;

        if (inNumThread > 1)
        {


            #pragma omp task
            mergeSort(inArray,wrkArray,inStart,half,(inNumThread/2));

            #pragma omp task
            mergeSort(inArray,wrkArray,(half + 1),inLast,(inNumThread/2));

            #pragma omp taskwait

        } else
        {       //   down to one thread - just do merge sort in this thread
            mergeSort(inArray,wrkArray,inStart,half,1);
            mergeSort(inArray,wrkArray,(half + 1),inLast,1);
        }

        merge(inArray,wrkArray,inStart,half,inLast);
    }   //   end of if inlast > instart
}

   //  merge function
void merge(int inArray[], int wrkArray[], int inStart, int inHalf, int inLast)
{

    int leftIndex, rightIndex, wrkIndex, restIndex;

    wrkIndex   = inStart;     //  where to store the next item in the wrkArray
    leftIndex  = inStart;
    rightIndex = inHalf + 1;

        //  now loop thru both sides & compare to see which to assign first
    while ( (leftIndex <= inHalf) && (rightIndex <= inLast) )
    {
        if (inArray[leftIndex] <= inArray[rightIndex])
        {
            wrkArray[wrkIndex] = inArray[leftIndex];
            leftIndex = leftIndex + 1;
        } else
        {
            wrkArray[wrkIndex] = inArray[rightIndex];
            rightIndex = rightIndex + 1;
        }
        wrkIndex = wrkIndex + 1;
    }

        //  see which half is left over & process the rest on that side
        //   left side has some left
    if (leftIndex > inHalf)
    {
        for (restIndex=rightIndex; restIndex<=inLast; restIndex++)
        {
            wrkArray[wrkIndex] = inArray[restIndex];
            wrkIndex = wrkIndex + 1;
        }
    } else             //   right side has some left
    {
        for (restIndex=leftIndex; restIndex<=inHalf; restIndex++)
        {
            wrkArray[wrkIndex] = inArray[restIndex];
            wrkIndex = wrkIndex + 1;
        }
    }

        //  loop thru these & move the work array to the real array
    for (int i=inStart; i<=inLast; i++)
    {
        inArray[i] = wrkArray[i];
    }
}
