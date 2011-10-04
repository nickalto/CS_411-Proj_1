/*
==============================================================================
CS 411 Assignment 1  - Nick Alto
Program Name: pMult
Date: 10-3-11
Oregon State University
==============================================================================
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Global Variables */
int dimension;      /*user provided dimension of matrix e.g. nxn*/
int threadID = 0;   /*thread ID of given thread*/


/*
 *MatMult method called upon creation of threads
 *used to compute the sum of matrix1 and matrix2
 */
void *matMult(void *abc) {

  typedef struct {
    float matrix1[dimension][dimension];
    float matrix2[dimension][dimension];
    float matrixResult[dimension][dimension];
    int numOfThreads;
    int prevCalc;
} data;

data* sysData= (data*)abc;
int threadCalc;
int leftOvers;
int i;
int x;
int y;
int mtxCount;
float result;
  threadCalc  = floor((double)(dimension*dimension)/(double)sysData->numOfThreads);
leftOvers = dimension %sysData->numOfThreads; 

 for(i = 0; i<leftOvers; i++)
 {
   if(threadID == i)
   {
     threadCalc++;
   }
 }

 for(i=0; i< threadCalc; i++)
 { 
  x = floor((double)sysData->prevCalc / (double)dimension);
 y = sysData->prevCalc % dimension;
    for(mtxCount = 0; mtxCount < dimension; mtxCount++)
    {
      result += (sysData->matrix1[x][mtxCount] * sysData->matrix2[mtxCount][y]);
      /*printf("\nmatrix 1: %f  *  matrix 2: %f\n", sysData->matrix1[x][mtxCount], sysData->matrix2[mtxCount][y]);*/
    }
    /*printf("result = %f\n\n", result);*/
    sysData->matrixResult[x][y] = result;
    sysData->prevCalc ++;
    result = 0;

 } 


/*printf("\n Thread %d   work: %d\n   previous work = %d  x: %d  y: %d\n", threadID,threadCalc, sysData->prevCalc,x,y);
printf("working up here %f for thread %d\n",sysData->matrix1[0][0], threadID);
*/
threadID++;
pthread_exit(NULL);
}/*end function matMult()*/



 void *printData(void *arg, int k){
int i;
int j;

     typedef struct {
    float matrix1[dimension][dimension];
    float matrix2[dimension][dimension];
    float matrixResult[dimension][dimension];
    int numOfThreads;
    int prevCalc;
} data;

data* sysData= (data*)arg;


for(i=0; i< dimension; i++)
{
  printf("[");
  for(j=0; j<dimension; j++)
  {
    if(k == 1)
    {
      if((j+1) < dimension)
      {
      printf(" %f, ", sysData->matrix1[i][j]);
      }
      else
        printf(" %f ", sysData->matrix1[i][j]);

    }
    else if(k==2)
    {
      if((j+1) < dimension)
      {
      printf(" %f, ", sysData->matrix2[i][j]);
      } 
      else
        printf(" %f ", sysData->matrix2[i][j]);

    }
    else if(k==3)
    {
      if((j+1) < dimension)
      {
      printf(" %f, ", sysData->matrixResult[i][j]);
      }
      else
       printf(" %f ", sysData->matrixResult[i][j]);

    }
    
  }
  printf("]\n");

}
return 0;
}



int main(int argc, char *argv[]) {
  
int numThreads;   /* Number of threads provided via CL by user */
int printOut;     /* Optional CL argument, to print out resulting matrix*/
int rc;           /* For storing return value of pthread_create and pthread_join*/
int j;            /* Used for iterating through number of threads*/
int i;            /* Used for iterating through matrixes */
int k;            /* Used for iterating through matrixes */

/* Error checking to make sure proper number of command line arguments are
 * present, if not print error/proper syntax and exit program.
 */
if(argc < 4)
{
  printf("\ninvalid number of arguments: ./program [matrix dimension] [# of pthreads] [printing]\n");
  exit(-1);
}
 /* Otherwise proper number of command line arguments have been provided */
  printf("\nvalid number of command line arguments\n");
  dimension = atoi(argv[1]);
  numThreads = atoi(argv[2]);
  printOut = atoi(argv[3]);

if(numThreads < 1)
{
  numThreads = 1;
}
/* data struct declared here because the matrixes dimensions are specified
 * by the user and cannot be declared until the proper dimensions are provided.
 */
  typedef struct {
    float matrix1[dimension][dimension];
    float matrix2[dimension][dimension];
    float matrixResult[dimension][dimension];
    int numOfThreads;
    int prevCalc;
} data;


data *sysData = (data*)malloc(sizeof(data));


/* Fill both matrixes with random values, currently
 * hardcoding the seed value because srand48()
 * does not seem to set the seed value properly
 */
for(i = 0; i < dimension; i++)
{
  for(k = 0; k < dimension; k++)
  {
    sysData->matrix1[i][k] = (drand48()+47);
    sysData->matrix2[i][k] = (drand48()+47);
  }
}

printf("%f\n",sysData->matrix1[0][0]);


sysData->numOfThreads = numThreads;
sysData->prevCalc=0;
 if(numThreads > dimension)
 {
   numThreads = dimension;
   sysData->numOfThreads = dimension;
 }
pthread_t tid;
pthread_attr_t attr;

pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


/* Create threads and wait for them to finish and rejoin before
 * doing anything. If no threads are specified call matMult() explicitly.
 */

for (j=0; j<numThreads; j++) {
  rc = pthread_create(&tid, &attr, matMult, sysData);
  if (rc) {
    printf("ERROR: pthread_create(): is %d\n", rc);
    exit(-1);
    }

  /* Wait for the thread */
  rc = pthread_join(tid, NULL);
  if (rc) {
    printf("ERROR: pthread_join(): %d\n", rc);
    exit(-1);
    }
  }
  /* Default case where number of threads specified == 0 */
   if(numThreads == 0)
   {
     matMult(sysData);
   }

printf("\nMATRIX 1\n");
printData(sysData, 1);
printf("\nMATRIX 2\n");
printData(sysData, 2);
printf("\nRESULT\n");
printData(sysData, 3);
pthread_attr_destroy(&attr);
pthread_exit(NULL);

}