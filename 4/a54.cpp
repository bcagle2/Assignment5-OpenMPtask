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


int matA[3][3] = {          //defined as 3x3 but only using the
   {0,0,0},                 //range starting from [1][1] to match
   {0,4,9},                 //the formula
   {0,12,33}};

int matB[3][3] = {
   {0,0,0},
   {0,7,1},
   {0,17,5}};

int wrkM[8];
int wrkN[8];
int wrkP[8];
int wrkT[4];

int numMpred[30];
int numTasks = 24;

int maxThreads;
int numThreads;

int intensity;
int intensityFactor;


/* Function declarations */
int myMult(int,int);           // my multiply function
int myAdd(int,int);
int mySub(int,int);
void moreWork();
void initTasks();
void startTask(string, int);

void flowMsg(string);
void checkSuccessors(int);
int task2Int(string);
void statusDisplay(string);

void* runM1(void*);
void* runM2(void*);
void* runM3(void*);
void* runM4(void*);
void* runM5(void*);
void* runM6(void*);
void* runM7(void*);

void* runN1(void*);
void* runN2(void*);
void* runN3(void*);
void* runN4(void*);
void* runN5(void*);
void* runN6(void*);
void* runN7(void*);

void* runP1(void*);
void* runP2(void*);
void* runP3(void*);
void* runP4(void*);
void* runP5(void*);
void* runP6(void*);
void* runP7(void*);

void* runT1(void*);
void* runT2(void*);
void* runT3(void*);


string  taskId [200];            // task name
int     pred [200];              // #predecessors for a task
int     numSuccessor[200];       // #successors for a task
string  successor[200][30];      //  first dimension is for task.   second is for successor for that task
bool    taskStarted[200];

double  taskStartTime[200];
double  taskEndTime[200];

int     tIdx[200];      //  array of int to use as storage for integers passed to threads
int     curIdx;



/*********************   mainline *************************/

int main(int argc, char *argv[])
{
    myFlow   = false;
    myDebug  = false;
    dispStat = false;

    numThreads = 0;

    flowMsg("in Mainline");

    curIdx = -1;

          /*  if argument 1 is null, print out an error message and end the program */
    if (argv[1] == NULL
    || argv[2] == NULL
    || argv[3] == NULL)
    {
      cout << "The argument is null. Three arguments are required" << endl;
      cout << "arg1 is intensity.   arg2 is the intensity factor" << endl;
      cout << "arg3 is the max number of threads" << endl;
      exit(0);
    }

         /* now set intensity */
    stringstream arg1;  /* convert to (intensity) */
    arg1 << argv[1];
    arg1 >> intensity;

        stringstream arg2;  /* convert to (intensity factor) */
    arg2 << argv[2];
    arg2 >> intensityFactor;

    stringstream arg3;  /* convert to maxThreads */
    arg3 << argv[3];
    arg3 >> maxThreads;

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




    /*  intensityFactor = 5; */


    omp_set_dynamic(0);     // Explicitly disable dynamic #threads
    omp_set_num_threads(maxThreads);  // set # of threads to the requested amount

    /* initialize the predecessors/successors for the tasks*/
    initTasks();



            /* get the starting time */
    /*time_t startTime,endTime;*/
    timeval curTime;
    gettimeofday(&curTime, NULL);
    double startTime = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    flowMsg("    start Scheduler");

    bool allStarted = false;
    int loopCt = 0;

    statusDisplay("Scheduler starting");

        /*  schedule the tasks - start threads as tasks become ready */
    #pragma omp parallel
    {
        #pragma omp single
        while (!allStarted)
        {
            if (myDebug)
            {
                cout << " Mainline inside while" << endl;
            }
            loopCt = loopCt + 1;
                //   if (loopCt >= 25)
                //   {
                //        cout << "BREAKING OUT" << endl;
                //     break;
                //    }
            allStarted = true;

            for (int i=0; i<numTasks; i++)
            {
                if (myDebug)
                {
                    cout << " ML in for loopCt =" << loopCt << " i=" << i
                    << " taskStarted[i]=" << taskStarted[i]
                    << " taskID=" << taskId[i]
                    << " numThreads=" << numThreads << endl;
                }


                if (taskStarted[i] == false  && numThreads < maxThreads)
                //if (taskStarted[i] == false  )
                {
                    startTask(taskId[i], i);
                    taskStarted[i] = true;
                    statusDisplay("Starting " + taskId[i]);
                    if (myDebug)
                    {
                        cout << " after startedTask taskStarted=" << taskStarted[i] << endl;
                    }

                }

                if (!taskStarted[i])
                {
                    allStarted = false;
                }
            }  // end of for loop

            sched_yield();     // give everyone a chance to run

        } // end of whild loop
        #pragma omp taskwait
    }   //  end of parallel section

    statusDisplay("Scheduler has started all tasks");

    flowMsg("  Mainline -  Scheduler done - joins next");

          //  pthread_join(m2,NULL);
          //  pthread_join(n2,NULL);
          //  pthread_join(p2,NULL);


    statusDisplay("All threads complete");
    flowMsg("    Joins Done");

                     /* get the end time */
    /*time(&endTime);
    double elapse = difftime(endTime,startTime);*/
    gettimeofday(&curTime, NULL);
    double endTime = curTime.tv_sec + (curTime.tv_usec / 1000000.0);
    double elapse = endTime - startTime;




                  /* first write to console */
    cout << "intensity=" << intensity
        << " intensityFactor=" << intensityFactor
        << " maxThreads=" << maxThreads
        << " and it took " << elapse << " seconds" << endl;

    for (int i=0; i<numTasks; i++)
    {
        cout << taskId[i] << " Start " << taskStartTime[i] - startTime
             << "\tEnd " << taskEndTime[i] - startTime
             << "\tElapsed " << taskEndTime[i] - taskStartTime[i] << endl;
    }

              /* write same thing to output text file */

    //freopen("output22.txt","a",stdout);    /* redirect output to output.txt - append to existing file */
                  /* write same thing to output text file */
    string fileName;
    fileName = "output" + fileSuffix + ".txt";
    freopen(fileName.c_str(),"a",stdout);    /* redirect output to the specified filename - append to existing file */

    cout << " intensity=" << intensity
        << " intensityFactor=" << intensityFactor
        << " maxThreads=" << maxThreads
        << " and it took " << elapse << " seconds" << endl;


             /* now build a comma delimited .csv file */
        //freopen("output22.csv","a",stdout);    /* redirect output to output.csv - append to existing file */
    fileName = "output" + fileSuffix + ".csv";
    freopen(fileName.c_str(),"a",stdout);    /* redirect output to the specified filename - append to existing file */


    cout << intensity
        << "," << intensityFactor
        << "," << maxThreads
        << "," << elapse << endl;




    //freopen("output31.txt","a",stdout);    /* redirect output to output.txt - append to existing file */
    fileName = "output.detail" + fileSuffix + ".txt";
    freopen(fileName.c_str(),"a",stdout);    /* redirect output to the specified filename - append to existing file */

    cout << " intensity=" << intensity
        << " intensityFactor=" << intensityFactor
        << " maxThreads=" << maxThreads
        << " and it took " << elapse << " seconds" << endl;

    for (int i=0; i<numTasks; i++)
    {
        cout << taskId[i] << " Start " << taskStartTime[i] - startTime
             << "\tEnd " << taskEndTime[i] - startTime
             << "\tElapsed " << taskEndTime[i] - taskStartTime[i] << endl;
    }

             /* now build a comma delimited .csv file */
    //  freopen("output31.csv","a",stdout);    /* redirect output to output.csv - append to existing file */
    fileName = "output.detail" + fileSuffix + ".CSV";
    freopen(fileName.c_str(),"a",stdout);    /* redirect output to the specified filename - append to existing file */

    cout << intensity
        << "," << intensityFactor
        << "," << elapse << endl;

    cout << "Intensity"
        << "," << "Factor"
        << "," << "Total Elapse"
        << "," << " Task Id"
        << "," << "Start Time"
        << "," << "End Time"
        << "," << "Task Elapse" << endl;

    for (int i=0; i<numTasks; i++)
    {
        cout << intensity
            << "," << intensityFactor
            << "," << elapse
            << "," << taskId[i]
            << "," << taskStartTime[i] - startTime
            << "," << taskEndTime[i] - startTime
            << "," << taskEndTime[i] - taskStartTime[i] << endl;
    }

}

void moreWork()
{
    double wrkDbl;

              /* add extra work */
    for(int w=0; w<intensity; w++)
    {
      wrkDbl = pow(intensityFactor,2.0);
      wrkDbl = sqrt(intensityFactor);
      wrkDbl = wrkDbl;
    } /* end of for loop */

}   /* end of function f */

int myMult(int mult1,int mult2)
{
    for(int i=0; i<intensityFactor; i++)
    {
        moreWork();
    }

    return mult1 * mult2;
}

int myAdd(int add1,int add2)
{
    moreWork();

    return add1 + add2;
}

int mySub(int sub1,int sub2)
{
    moreWork();

    return sub1 - sub2;
}


void initTasks()
{
    flowMsg("In initTasks");
          /* now the M's */

    taskId[0]           = "M1";
    pred[0]             = 0;
    numSuccessor[0]     = 1;
    successor[0][0]     = "P1";

    taskId[1]           = "M2";
    pred[1]             = 0;
    numSuccessor[1]     = 1;
    successor[1][0]     = "P2";

    taskId[2]           = "M3";
    pred[2]             = 0;
    numSuccessor[2]     = 2;
    successor[2][0]     = "M4";
    successor[2][1]     = "P3";

    taskId[3]           = "M4";
    pred[3]             = 1;
    numSuccessor[3]     = 2;
    successor[3][0]     = "M6";
    successor[3][1]     = "P4";

    taskId[4]           = "M5";
    pred[4]             = 0;
    numSuccessor[4]     = 1;
    successor[4][0]     = "P5";

    taskId[5]           = "M6";
    pred[5]             = 1;
    numSuccessor[5]     = 1;
    successor[5][0]     = "P6";

    taskId[6]           = "M7";
    pred[6]             = 0;
    numSuccessor[6]     = 1;
    successor[6][0]     = "P7";

    taskId[7]           = "N1";
    pred[7]             = 0;
    numSuccessor[7]     = 1;
    successor[7][0]     = "P1";

         /*  N's start here */
    taskId[8]           = "N2";
    pred[8]             = 0;
    numSuccessor[8]     = 1;
    successor[8][0]     = "P2";

    taskId[9]           = "N3";
    pred[9]             = 0;
    numSuccessor[9]     = 2;
    successor[9][0]     = "P3";
    successor[9][1]     = "N4";

    taskId[10]          = "N4";
    pred[10]            = 1;
    numSuccessor[10]    = 2;
    successor[10][0]    = "P4";
    successor[10][1]    = "N7";

    taskId[11]          = "N5";
    pred[11]            = 0;
    numSuccessor[11]    = 1;
    successor[11][0]    = "P5";

    taskId[12]          = "N6";
    pred[12]            = 0;
    numSuccessor[12]    = 1;
    successor[12][0]    = "P6";

    taskId[13]          = "N7";
    pred[13]            = 1;
    numSuccessor[13]    = 1;
    successor[13][0]    = "P7";

         /*  P's start here */
    taskId[14]          = "P1";
    pred[14]            = 2;
    numSuccessor[14]    = 1;
    successor[14][0]    = "T1";

    taskId[15]          = "P2";
    pred[15]            = 2;
    numSuccessor[15]    = 0;

    taskId[16]          = "P3";
    pred[16]            = 2;
    numSuccessor[16]    = 1;
    successor[16][0]    = "T3";

    taskId[17]          = "P4";
    pred[17]            = 2;
    numSuccessor[17]    = 1;
    successor[17][0]    = "T1";

    taskId[18]          = "P5";
    pred[18]            = 2;
    numSuccessor[18]    = 1;
    successor[18][0]    = "T2";

    taskId[19]          = "P6";
    pred[19]            = 2;
    numSuccessor[19]    = 0;

    taskId[20]          = "P7";
    pred[20]            = 2;
    numSuccessor[20]    = 0;

         /*  T's start here */
    taskId[21]          = "T1";
    pred[21]            = 2;
    numSuccessor[21]    = 2;
    successor[21][0]    = "T2";
    successor[21][1]    = "T3";

    taskId[22]          = "T2";
    pred[22]            = 2;
    numSuccessor[22]    = 0;

    taskId[23]          = "T3";
    pred[23]            = 2;
    numSuccessor[23]    = 0;

    for (int i=0; i<numTasks; i++)
    {
        taskStarted[i] = false;
    }

    flowMsg("   leaving initTasks");

}



void calcM1()
{
    wrkM[1] = myAdd(matA[1][1],0);
}

void calcM2()
{
    wrkM[2] = myAdd(matA[1][2],0);
}
void calcM3()
{
    wrkM[3] = myAdd(matA[2][1],matA[2][2]);
}
void calcM4()
{
    wrkM[4] = mySub(wrkM[3],matA[1][1]);
}
void calcM5()
{
    wrkM[5] = mySub(matA[1][1],matA[2][1]);
}
void calcM6()
{
    wrkM[6] = mySub(matA[1][2],wrkM[4]);
}
void calcM7()
{
    wrkM[7] = myAdd(matA[2][2],0);
}

void calcN1()
{
    wrkN[1] = myAdd(matB[1][1],0);
}
void calcN2()
{
    wrkN[2] = myAdd(matB[2][1],0);
}
void calcN3()
{
    wrkN[3] = mySub(matB[1][2],matB[1][1]);
}
void calcN4()
{
    wrkN[4] = mySub(matB[2][2],wrkN[3]);
}
void calcN5()
{
    wrkN[5] = mySub(matB[2][2],matB[1][2]);
}
void calcN6()
{
    wrkN[6] = myAdd(matB[2][2],0);
}
void calcN7()
{
    wrkN[7] = mySub(matB[2][1],wrkN[4]);
}

   /*     calcP* functions */
void calcP1()
{
    wrkP[1] = myMult(wrkM[1],wrkN[1]);
}
void calcP2()
{
    wrkP[2] = myMult(wrkM[2],wrkN[2]);
}
void calcP3()
{
    wrkP[3] = myMult(wrkM[3],wrkN[3]);
}
void calcP4()
{
    wrkP[4] = myMult(wrkM[4],wrkN[4]);
}
void calcP5()
{
    wrkP[5] = myMult(wrkM[5],wrkN[5]);
}
void calcP6()
{
    wrkP[6] = myMult(wrkM[6],wrkN[6]);
}
void calcP7()
{
    wrkP[7] = myMult(wrkM[7],wrkN[7]);
}


void calcT1()
{
    wrkT[1] = myAdd(wrkP[1],wrkP[4]);
}
void calcT2()
{
    wrkT[2] = myAdd(wrkT[1],wrkP[5]);
}
void calcT3()
{
    wrkT[3] = myAdd(wrkT[1],wrkP[3]);
}





/* ****************   start of functions that run in the threads ********/
void* runM1(void* inI)
{
    flowMsg("In runM1");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);


    calcM1();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for M1 ending");
    flowMsg("  Leaving runM1");
    return NULL;

}  /* end of subroutine */

void* runM2(void* inI)
{
    flowMsg("In runM2");
    //  int taskIndex = *((int *) inI);
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    if (myDebug)
    {
        cout << " *taskIndex=" << *taskIndex << endl;

    }

    calcM2();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for M2 ending");
    flowMsg("  Leaving runM2");
    return NULL;

}  /* end of subroutine */

void* runM3(void* inI)
{
    flowMsg("In runM3");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcM3();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for M3 ending");
    flowMsg("  Leaving runM3");
    return NULL;

}  /* end of subroutine */

void* runM4(void* inI)
{
    flowMsg("In runM4");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcM4();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for M4 ending");
    flowMsg("  Leaving runM4");
    return NULL;

}  /* end of subroutine */

void* runM5(void* inI)
{
    flowMsg("In runM5");
    int *taskIndex = (int*)inI;
    if (myDebug)
    {
        cout << " *taskIndex=" << *taskIndex << endl;

    }

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcM5();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for M5 ending");
    flowMsg("  Leaving runM5");
    return NULL;

}  /* end of subroutine */

void* runM6(void* inI)
{
    flowMsg("In runM6");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcM6();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for M6 ending");
    flowMsg("  Leaving runM6");
    return NULL;

}  /* end of subroutine */

void* runM7(void* inI)
{
    flowMsg("In runM7");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcM7();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for M7 ending");
    flowMsg("  Leaving runM7");
    return NULL;

}  /* end of subroutine */



/* ****************   start of runN* functions that run in the threads ********/
void* runN1(void* inI)
{
    flowMsg("In runN1");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcN1();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for N1 ending");
    flowMsg("  Leaving runN1");
    return NULL;

}  /* end of subroutine */

void* runN2(void* inI)
{
    flowMsg("In runN2");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcN2();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for N2 ending");
    flowMsg("  Leaving runN2");
    return NULL;

}  /* end of subroutine */

void* runN3(void* inI)
{
    flowMsg("In runN3");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcN3();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for N3 ending");
    flowMsg("  Leaving runN3");
    return NULL;

}  /* end of subroutine */

void* runN4(void* inI)
{
    flowMsg("In runN4");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcN4();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for N4 ending");
    flowMsg("  Leaving runN4");
    return NULL;

}  /* end of subroutine */

void* runN5(void* inI)
{
    flowMsg("In runN5");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcN5();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for N5 ending");
    flowMsg("  Leaving runN5");
    return NULL;

}  /* end of subroutine */

void* runN6(void* inI)
{
    flowMsg("In runN6");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcN6();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for N6 ending");
    flowMsg("  Leaving runN6");
    return NULL;

}  /* end of subroutine */

void* runN7(void* inI)
{
    flowMsg("In runN7");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcN7();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for N7 ending");
    flowMsg("  Leaving runN7");
    return NULL;

}  /* end of subroutine */



/* ****************   start runP* functions ********/
void* runP1(void* inI)
{
    flowMsg("In runP1");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcP1();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for P1 ending");
    flowMsg("  Leaving runP1");
    return NULL;

}  /* end of subroutine */

void* runP2(void* inI)
{
    flowMsg("In runP2");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcP2();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for P2 ending");
    flowMsg("  Leaving runP2");
    return NULL;

}  /* end of subroutine */

void* runP3(void* inI)
{
    flowMsg("In runP3");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcP3();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for P3 ending");
    flowMsg("  Leaving runP3");
    return NULL;

}  /* end of subroutine */

void* runP4(void* inI)
{
    flowMsg("In runP4");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcP4();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for P4 ending");
    flowMsg("  Leaving runP4");
    return NULL;

}  /* end of subroutine */

void* runP5(void* inI)
{
    flowMsg("In runP5");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcP5();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for P5 ending");
    flowMsg("  Leaving runP5");
    return NULL;

}  /* end of subroutine */

void* runP6(void* inI)
{
    flowMsg("In runP6");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcP6();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for P6 ending");
    flowMsg("  Leaving runP6");
    return NULL;

}  /* end of subroutine */

void* runP7(void* inI)
{
    flowMsg("In runT1");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcP7();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for P7 ending");
    flowMsg("  Leaving runP7");
    return NULL;

}  /* end of subroutine */




/* ****************   start runT* functions ********/
void* runT1(void* inI)
{
    flowMsg("In runT1");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcT1();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for T1 ending");
    flowMsg("  Leaving runT1");
    return NULL;

}  /* end of subroutine */

void* runT2(void* inI)
{
    flowMsg("In runT2");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcT2();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for T2 ending");
    flowMsg("  Leaving runT2");
    return NULL;

}  /* end of subroutine */

void* runT3(void* inI)
{
    flowMsg("In runT3");
    int *taskIndex = (int*)inI;

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskStartTime[*taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);

    calcT3();

    checkSuccessors(*taskIndex);

    statusDisplay("Thread for T3 ending");
    flowMsg("  Leaving runT3");
    return NULL;


}  /* end of subroutine */






/* this function starts a thread based on the input task id */  /*btc*/
void startTask(string inTask,int inI)
{
    flowMsg("In startTask");
//    int locI = inI;

    int *locI = new int;
    *locI = inI;
    curIdx = curIdx + 1;   //  get next int storage location to pass to thread
    tIdx[curIdx] = *locI;


    if (myDebug)
    {
        cout << "  startTask inI=" << inI
           << " locI=" << *locI
           << " inTask=" << inTask << endl;
    }

    numThreads = numThreads + 1;


    if (inTask == "M1")   /*btc*/
    {
	#pragma omp task depend(out :wrkM[1])
        runM1(&(tIdx[curIdx]));
    }

    if (inTask == "M2")
    {
        #pragma omp task depend(out :wrkM[2])
        runM2(&(tIdx[curIdx]));
    }

    if (inTask == "M3")
    {
        #pragma omp task depend(out :wrkM[3])
        runM3(&(tIdx[curIdx]));
    }

    if (inTask == "M4")
    {
        #pragma omp task depend(inout :wrkM[3],wrkM[4])
        runM4(&(tIdx[curIdx]));
    }

    if (inTask == "M5")
    {
        #pragma omp task depend(out :wrkM[5])
        runM5(&(tIdx[curIdx]));
    }

    if (inTask == "M6")
    {
        #pragma omp task depend(inout :wrkM[4],wrkM[6])
        runM6(&(tIdx[curIdx]));
    }

    if (inTask == "M7")
    {
        #pragma omp task depend(out :wrkM[7])
        runM7(&(tIdx[curIdx]));
    }

    if (inTask == "N1")
    {
        #pragma omp task depend(out :wrkN[1])
        runN1(&(tIdx[curIdx]));
    }

    if (inTask == "N2")
    {
        #pragma omp task depend(out :wrkN[2])
        runN2(&(tIdx[curIdx]));
    }

    if (inTask == "N3")
    {
        #pragma omp task depend(out :wrkN[3])
        runN3(&(tIdx[curIdx]));
    }

    if (inTask == "N4")
    {
        #pragma omp task depend(inout :wrkN[3],wrkN[4])
        runN4(&(tIdx[curIdx]));
    }

    if (inTask == "N5")
    {
        #pragma omp task depend(out :wrkN[5])
        runN5(&(tIdx[curIdx]));
    }
    if (inTask == "N6")
    {
        #pragma omp task depend(out :wrkN[6])
        runN6(&(tIdx[curIdx]));
    }

    if (inTask == "N7")
    {
        #pragma omp task depend(inout :wrkN[4],wrkN[7])
        runN7(&(tIdx[curIdx]));
    }

    if (inTask == "P1")
    {
        #pragma omp task depend(inout :wrkM[1],wrkN[1],wrkP[1])
        runP1(&(tIdx[curIdx]));
    }

    if (inTask == "P2")
    {
        #pragma omp task depend(inout :wrkM[2],wrkN[2],wrkP[2])
        runP2(&(tIdx[curIdx]));
    }
    if (inTask == "P3")
    {
        #pragma omp task depend(inout :wrkM[3],wrkN[3],wrkP[3])
        runP3(&(tIdx[curIdx]));
    }

    if (inTask == "P4")
    {
        #pragma omp task depend(inout :wrkM[4],wrkN[4],wrkP[4])
        runP4(&(tIdx[curIdx]));
    }

    if (inTask == "P5")
    {
        #pragma omp task depend(inout :wrkM[5],wrkN[5],wrkP[5])
        runP5(&(tIdx[curIdx]));
    }

    if (inTask == "P6")
    {
        #pragma omp task depend(inout :wrkM[6],wrkN[6],wrkP[6])
        runP6(&(tIdx[curIdx]));
    }

    if (inTask == "P7")
    {
        #pragma omp task depend(inout :wrkM[7],wrkN[7],wrkP[7])
        runP7(&(tIdx[curIdx]));
    }

    if (inTask == "T1")
    {
        #pragma omp task depend(inout :wrkP[1],wrkP[4],wrkT[1])
        runT1(&(tIdx[curIdx]));
    }

    if (inTask == "T2")
    {
        #pragma omp task depend(inout :wrkT[1],wrkP[5],wrkT[2])
        runT2(&(tIdx[curIdx]));
    }
    if (inTask == "T3")
    {
        #pragma omp task depend(inout :wrkP[3],wrkT[1])
        runT3(&(tIdx[curIdx]));
    }

    flowMsg("  leaving startTask");

  }   //  end of startTask


/* routine started within a thread to do calc, etc */
void* runTask(void* inI)
{

    int taskIndex = *((int *) inI);

    if (taskId[taskIndex] == "M1")
        calcM1();

    if (taskId[taskIndex] == "M2")
        calcM2();

    if (taskId[taskIndex] == "M3")
        calcM3();

    if (taskId[taskIndex] == "M4")
        calcM4();

    if (taskId[taskIndex] == "M5")
        calcM5();

    if (taskId[taskIndex] == "M6")
        calcM6();

    if (taskId[taskIndex] == "M7")
        calcM7();

    if (taskId[taskIndex] == "N1")
        calcN1();

    if (taskId[taskIndex] == "N2")
        calcN2();

    if (taskId[taskIndex] == "N3")
        calcN3();

    if (taskId[taskIndex] == "N4")
        calcN4();

    if (taskId[taskIndex] == "N5")
        calcN5();

    if (taskId[taskIndex] == "N6")
        calcN6();

    if (taskId[taskIndex] == "N7")
        calcN7();

    if (taskId[taskIndex] == "P1")
        calcP1();

    if (taskId[taskIndex] == "P2")
        calcP2();

    if (taskId[taskIndex] == "P3")
        calcP3();

    if (taskId[taskIndex] == "P4")
        calcP4();

    if (taskId[taskIndex] == "P5")
        calcP5();

    if (taskId[taskIndex] == "P6")
        calcP6();

    if (taskId[taskIndex] == "P7")
        calcP7();

    if (taskId[taskIndex] == "T1")
        calcT1();

    if (taskId[taskIndex] == "T2")
        calcT2();

    if (taskId[taskIndex] == "T3")
        calcT3();

    statusDisplay("Calculation done for " + taskId[taskIndex]);

    checkSuccessors(taskIndex);

    return NULL;

}  /* end of subroutine */


/* routine to decrement the predecessor of a successor task */



// routine to check for successor tasks & decrease the predecessor count
void checkSuccessors(int inI)
{
    flowMsg("In checkSuccessors");

    int taskIndex = inI;

    if (myDebug)
    {
        cout << "  inI=" << inI << " taskIndex=" << taskIndex
          << " taskid =" << taskId[taskIndex]
          << " taskid from inI=" << taskId[inI]
          << " #successors=" << numSuccessor[inI] << endl;

    }

            // loop thru the successors if any
    for (int i=0; i<numSuccessor[inI]; i++)
    {           // call routine to decrease the predecessor count
        if (myDebug)
        {
            cout << "  in numsuccessor loop i=" << i << " taskIndex=" << inI
             << endl;

        }

    }


        // this is the last thing done before exiting a thread
        // subtract 1 from the number of threads and calculate the end time for the thread

    timeval curTime;
    gettimeofday(&curTime, NULL);
    taskEndTime[taskIndex] = curTime.tv_sec + (curTime.tv_usec / 1000000.0);


    numThreads = numThreads - 1;


    flowMsg("  leaving checkSuccessors");
    return;

}  /* end of subroutine */

void flowMsg(string inString)
{
    if (myFlow)
        cout << inString << endl;
    return;
}   // end of flowMsg


//   routine to convert a task id into an int that can be used in a case stmt
int task2Int(string inTaskId)
{
    flowMsg("in task2Int");

    for(int i=0; i<numTasks; i++)
    {
        if (inTaskId == taskId[i])
            return i;
    }
    cout << "   task2Int  taskid " << inTaskId << " not found" << endl;

    flowMsg("   leaving task2Int");
    return -1;

}

void statusDisplay(string inMsg)
{
    if (!dispStat)
      return;
    cout << inMsg << endl;
    return;
}
