#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

unsigned int seed = 0;

int	nowYear;		// 2022 - 2027
int	nowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int nowNumFluffy;  // number of Fluffy 

float FluffyAte;  // How much of the population did Fluffy enjoy...?
float GrainAte;  // How much grain was able to be ate by deer...? 

const float GRAIN_GROWS_PER_MONTH = 9.0;
const float ONE_DEER_EATS_PER_MONTH = 1.0;

const float AVG_PRECIP_PER_MONTH = 7.0;	// average
const float AMP_PRECIP_PER_MONTH = 6.0;	// plus or minus
const float RANDOM_PRECIP = 2.0;	// plus or minus noise

const float AVG_TEMP = 60.0;	// average
const float AMP_TEMP = 20.0;	// plus or minus
const float RANDOM_TEMP = 10.0;	// plus or minus noise

const float MIDTEMP = 40.0;
const float MIDPRECIP = 10.0;


// Global Variables
omp_lock_t	Lock;
volatile int	NumInThreadTeam;
volatile int	NumAtBarrier;
volatile int	NumGone;

// Square root
float
SQR(float x)
{
    return x * x;
}

float
Ranf(unsigned int* seedp, float low, float high)
{
    float r = (float)rand_r(seedp);              // 0 - RAND_MAX

    return(low + r * (high - low) / (float)RAND_MAX);
}


int
Ranf(unsigned int* seedp, int ilow, int ihigh)
{
    float low = (float)ilow;
    float high = (float)ihigh + 0.9999f;

    return (int)(Ranf(seedp, low, high));
}



// The temperature and precipitation are a function of the particular month:
void Environment() {
    float ang = (30. * (float)nowMonth + 15.) * (M_PI / 180.);

    float temp = AVG_TEMP - AMP_TEMP * cos(ang);
    NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
    NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
    if (NowPrecip < 0.)
        NowPrecip = 0.;
}

// Set simulation 

void Deer() {

    while (nowYear < 2028)
    {
        int nextNumDeer = NowNumDeer;
        int carryingCapacity = (int)(NowHeight);
        if (nextNumDeer < carryingCapacity)
            nextNumDeer++;
        else
            if (nextNumDeer > carryingCapacity)
                nextNumDeer--;

        if (nextNumDeer < 0)
            nextNumDeer = 0;
        /*
        // track Deer remaining
        int nextDeer = nowNumFluffy;

        // Fluffy ate Deer
        if (float(NowNumDeer) > NowHeight) {
            nextDeer = NowNumDeer - 1;
        }

        // More food than Deer
        else if (float(NowNumDeer) < NowHeight) {
            nextDeer = NowNumDeer + 1;
        }

        // Nothing changes
        else {
            nextDeer = NowNumDeer;
        }

        // Next Number of Deer
        if (nextDeer < 0) {
            nextDeer = 0;
        }

        // Did Fluffy come....? 
        if (nowNumFluffy > 0) {
            nextDeer = nextDeer / 2; // divide 2 -->story line
        }
        */



        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
       // look at global state for next month -> store into temp var
        // call to wait barrier -- 3
            // DoneComputing barrier:
        #pragma omp barrier	//-- or --   WaitBarrier( );

        NowNumDeer = nextNumDeer;

       // NowNumDeer = nextDeer;

            // DoneAssigning barrier:
        #pragma omp barrier	//-- or --   WaitBarrier( );

            // DonePrinting barrier:
        #pragma omp barrier	//-- or --   WaitBarrier( );
           
    }
}

void Grain() {
    while (nowYear < 2028)
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:

        float tempFactor = exp(-SQR((NowTemp - MIDTEMP) / 10.));
        float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP) / 10.));

        float nextHeight = NowHeight;
        nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

        if (nextHeight < 0) {
            nextHeight = 0;
        }

        // Deer ate that survived Fluffy
      //  if (GrainAte > .2) {
      //  }

            // DoneComputing barrier:
        #pragma omp barrier //	-- or --   WaitBarrier( );
        NowHeight = nextHeight;

            // DoneAssigning barrier:
        #pragma omp barrier	//-- or --   WaitBarrier( );
            //. . .

            // DonePrinting barrier:
        #pragma omp barrier	//-- or --   WaitBarrier( ); // watcher 
            //. . .
    }
}

// ......... HEHEHEHEHEHEHEHEHEHEH
// fluffy was to come around every year to eat deer set to eat in 'May' 
void FluffyAttack() {

    while (nowYear < 2028)
    {
        int nextNumFluffy = nowNumFluffy;
        int carryingCapacity = (int)(NowHeight);
        if (nowMonth == 4) {
            nextNumFluffy++;
            //nextNumFluffy = 1;
        }
        else
            if (nowMonth == 5) {
                nextNumFluffy--;
                //nextNumFluffy = 0;
            }

        if (nextNumFluffy < 0)
            nextNumFluffy = 0;


    /*
    while (nowYear < 2028) {
        int nextNumFluffy = 0;
        if (nowMonth == 4) {
            nextNumFluffy = 1;
        }
        //if (nowMonth == 0) {
        //    nextNumFluffy = 0;
       // }
        else if (nextNumFluffy > NowNumDeer) {
            nextNumFluffy = nextNumFluffy;
        }
        if (nextNumFluffy < 0) {
            nextNumFluffy = 1;
        }
        */


        #pragma omp barrier
        nowNumFluffy = nextNumFluffy;

        #pragma omp barrier

        #pragma omp barrier
    }
}


void Watcher() {

    while (nowYear < 2028)
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
       // . . .

            // DoneComputing barrier:
        #pragma omp barrier   //	-- or --   WaitBarrier( );
           // . . .
        //Environment();
        //printf(" %d,  %d, %d ", nowMonth, NowNumDeer, nowNumFluffy);
        
        // DoneAssigning barrier:
        #pragma omp barrier	//-- or --   WaitBarrier( );
        //printf(" %d,  %d, %lf\n", nowMonth, NowNumDeer, NowHeight);  //, nowNumFluffy
       // FILE* myfile = fopen("Fluffy.csv", "w");
        //fprintf(myfile, "\n %d,  %d, %6.2lf, %d\n", nowMonth, NowNumDeer, NowHeight, nowNumFluffy);  //, nowNumFluffy
        printf(" %d, %d, %6.2lf, %6.2lf, %6.2lf, %d\n", nowMonth, NowNumDeer,NowTemp, NowPrecip, NowHeight, nowNumFluffy);  //, nowNumFluffy
        
        nowMonth++;
        if (nowMonth > 72) {
            nowYear++;
            nowMonth = 1;
        }
        Environment();
        //fclose(myfile);
           // . . .

            // DonePrinting barrier:
        #pragma omp barrier	//-- or --   WaitBarrier( );
           // . . .
    }


    //............. while loop and three barriers to increment the months and the year (possibly) 
    // compute barrier
    // finish barrier
    // Time increment
    // Environment()
    //printf
}


/*
void
WaitBarrier()
{
    omp_set_lock(&Lock);
    {
        NumAtBarrier++;
        if (NumAtBarrier == NumInThreadTeam)
        {
            NumGone = 0;
            NumAtBarrier = 0;
            // let all other threads get back to what they were doing
// before this one unlocks, knowing that they might immediately
// call WaitBarrier( ) again:
            while (NumGone != NumInThreadTeam - 1);
            omp_unset_lock(&Lock);
            return;
        }
    }
    omp_unset_lock(&Lock);

    while (NumAtBarrier != 0);	// this waits for the nth thread to arrive

#pragma omp atomic
    NumGone++;			// this flags how many threads have returned
}
*/

// watcher 
//fluffy 
// random number function 
// while loop 
int main() {
    // starting date and time:
#ifndef _OPENMP
    fprintf(stderr, "OpenMP is not supported\n");
#endif

    nowMonth = 0;
    nowYear = 2022;

    // starting state (feel free to change this if you want):
    NowNumDeer = 5;
    NowHeight = 1.;
    //FluffyAte = 0;
    //GrainAte = 0;

    // Set my environment 
    Environment();
    //printf("  %d,  %d, %d ", nowMonth, NowNumDeer, nowNumFluffy);
    //printf

    omp_set_num_threads(4);	// same as # of sections
#pragma omp parallel sections
    {
#pragma omp section
        {
            Deer();
        }

#pragma omp section
        {
            Grain();
        }

#pragma omp section
        {
            Watcher();
        }
        
        #pragma omp section
                {
                    FluffyAttack();	// your own
                }
            }       // implied barrier -- all functions must return in order
                // to allow any of them to get past here
                

    }
