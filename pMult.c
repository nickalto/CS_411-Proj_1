/*==============================================================================
 * CS 411 Assignment 1  - Nick Alto
 * Program Name: pMult
 * Date: 10-3-11
 * Oregon State University
 * ==============================================================================*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>


/*Global Variables*/
/*===========================================================================*/
int g_dimension=0;     	 /* user provided g_dimension of matrix e.g. nxn*/
int g_numThread=0;	 /* user specified number of threads*/
int g_arg=0;		 /* user specified argument*/
int g_threadID = 0;  	 /* thread ID of given thread*/
/*===========================================================================*/


 typedef struct {
	float **matrix1;
	float **matrix2;
	float **matrixResult;
	int numOfThreads;
	int prevCalc;
} data;


/*==========================================================================
 * Name: matMult
 * Description:	MatMult method called upon creation of threads
 *   	 	used to compute the sum of matrix1 and matrix2
 *
 *==========================================================================*/

void *matMult(void *abc) {

data* sysData= (data*)abc;    /* Cast argument to struct pointer sysData*/
int threadCalc;               /* Used to calculate how many computations are necessary for given thread*/
int leftOvers;                /* Used to add work to given threads*/
int i = 0;                    /* Used for iteration*/
int x = 0;                    /* Used to calculate x value of where last thread left off*/
int y = 0;                    /* Used to calculate y value of where last thread left off*/
int mtxCount=0;               /* Used for iteration*/
float result;                 /* Used to store result of matrix calculation*/

/* Determine how much work needs to be done for each thread, given the user specified g_dimension
 * and provided number of threads. If not cleanly divisible by number of threads then iterate through
 * the "leftover" calculations and assign them to threads.
 */

threadCalc  = floor((g_dimension*g_dimension)/sysData->numOfThreads);
leftOvers = ((g_dimension*g_dimension)-(threadCalc * sysData->numOfThreads));

 for(i = 0; i<leftOvers; i++) {
   if(g_threadID == i) {
	threadCalc++;
   }
 }

/* Iterate through the calculations for the given thread and preform the necessary calculations
 * to produce the resulting matrix calculation and store it in the array designated for the 
 * matrix product.
 */

 for(i=0; i< threadCalc; i++) { 
	x = floor((double)sysData->prevCalc / (double)g_dimension);
	y = sysData->prevCalc % g_dimension;
	  for(mtxCount = 0; mtxCount < g_dimension; mtxCount++) {
		  result += (sysData->matrix1[x][mtxCount] * sysData->matrix2[mtxCount][y]);
	  }

	/* Make sure to add the resulting calculation to the new array, and to set the result to 0*/
	sysData->matrixResult[x][y] = result;
	sysData->prevCalc ++;
	result = 0;
 } 

/* Calculations for given thread are complete, increment g_threadID and exit*/
g_threadID++;
pthread_exit(NULL);
}/*end function matMult()*/



/*==========================================================================
 * Name: printData
 * Description:	Print function, prints out the matrix passed in determined 
 *    		by the value of the int passed,   int k = 1 - print matrix1;  
 *    		int k = 2 print matrix2; int k = 3 print matrixResult;
 *    				 				 
 *==========================================================================*/

void *printData(void *arg, int k){
int i = 0;     			 /* Used for iteration*/
int j = 0;     			 /* Used for iteration*/
data* sysData= (data*)arg;       /* Used for accessing data in struct data*/

/* iterate through array and print out the appropriate matrix depending on 
 * the value of k, passed into the printData function.
 */

for(i=0; i< g_dimension; i++) {

  printf("[");

  for(j=0; j<g_dimension; j++) {
	if(k == 1) {			/*Print out matrix1*/
	  if((j+1) < g_dimension) {
		printf(" %f, ", sysData->matrix1[i][j]);
	  } else
		printf(" %f ", sysData->matrix1[i][j]);
	} else if(k==2) {			/*Print out matrix2*/
	  if((j+1) < g_dimension) {
		printf(" %f, ", sysData->matrix2[i][j]);
	  } else
		printf(" %f ", sysData->matrix2[i][j]);
	} else if(k==3) {			/*Print out matrixResult*/
	  if((j+1) < g_dimension) {
		printf(" %f, ", sysData->matrixResult[i][j]);
	  } else
	   printf(" %f ", sysData->matrixResult[i][j]);
	}
	
  }
  printf("]\n");
}
return 0;
}


/*==========================================================================
 * Name: main
 * Description:	Initialize data, parse command line data, then create as many
 *  		threads as specified by the user and wait for them to finish
 *   		with the matrix multiplication.
 *    				 
 *==========================================================================*/

int main(int argc, char *argv[]) {

int pth = 0;	  /* Used for storing CL args*/	
int j   = 0;      /* Used for iterating through number of threads*/
int i   = 0;      /* Used for iterating through matrixes */
int numThreads;   /* Number of threads provided via CL by user */
int printOut;     /* Optional CL argument, to print out resulting matrix*/
int rc  = 0;      /* For storing return value of pthread_create and pthread_join*/
int k   = 0;      /* Used for iterating through matrixes */

/* Error checking to make sure proper number of command line arguments are
 * present, if not print error/proper syntax and exit program.
 */
 g_arg=atoi(argv[1]);
if(argc < 3) {
  printf("\ninvalid number of arguments: ./program [# of pthreads][matrix g_dimension] [printing]\n");
  exit(-1);
}

/* Otherwise proper number of command line arguments have been provided */

g_dimension = atoi(argv[2]);
pth=numThreads = atoi(argv[1]);
if(argc > 3)
{
	printOut = atoi(argv[3]);
}else {printOut = 0;}
data *sysData;
if(numThreads < 1) { numThreads = 1;}

/* Allocate memory for the struct and then individually for the **matrixes
 * In order to create array's from double pointers.
 */

sysData = (data*)malloc(sizeof(data));
sysData->matrix1 = malloc(g_dimension*sizeof(float*));
sysData->matrix2 = malloc(g_dimension*sizeof(float*));
sysData->matrixResult = malloc(g_dimension*sizeof(float*));

for(i=0;i<g_dimension;i++) {
	sysData->matrix1[i] = malloc(g_dimension*sizeof(float));
	sysData->matrix2[i] = malloc(g_dimension*sizeof(float));
	sysData->matrixResult[i] = malloc(g_dimension*sizeof(float));
}

/* Fill both matrixes with random values, currently
 * hardcoding the seed value because srand48()
 * does not seem to set the seed value properly
 */

 srand48(47u);
 (pth>g_arg) ? numThreads=atoi(argv[1]) : (pth=numThreads);
for(i = 0; i < g_dimension; i++) {
  for(k = 0; k < g_dimension; k++) {
	sysData->matrix1[i][k] = drand48();
	sysData->matrix2[i][k] = drand48();
  }
}

/* Initialize struct varibles and create thread with correct attributes and detachstate*/

sysData->numOfThreads = numThreads;
sysData->prevCalc=0;

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

/* Handle the third command line argument, wether or not to print out the calculated
 * product matrix 
 */
  
if(printOut != 0) {

	printData(sysData, 1);		/* specifies matrix1*/
	printf("\n");
	printData(sysData, 2);		/* specifies matrix2*/
	printf("\n");
	printData(sysData, 3);		/* specifies matrixResult*/
}

pthread_attr_destroy(&attr);
pthread_exit(NULL);

}
