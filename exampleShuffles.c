#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "MillerShuffle.h"

unsigned int algoChkSum(int); // advance prototype
float MeanTest(int, unsigned int);

/*------------------------------------------------------------------*/
/*  Generates samples of shuffling 52 letters a-zA-Z (~songs)       */
/*                                                                  */
/*  You may run this a few times to get demonstrative comparisons   */ 

int main(int argc, char **argv)
{
	int i, n, item;
	char c;
	char reps[53];
	char seen[52];
	int shuffleID;
	int algo;
	unsigned int chks;
	float mean;
	bool PRIG_err = false;
	int stdChkSums[] = {6351454, 5980121, 6603945, 6386077};
   
	printf("\nFirst we'll valid the MillerShuffle algorithms' implementation ensuring that they'll deliver the performance\n"); 
	printf("seen in their testing results during development. This is done by check-summing their output given specific\n"); 
	printf("inputs, which is repeatable due to their deterministic feature. \n");
	printf("Note that any changes in an algorithm can result in undesirable patterns in the shuffles produced.\n");

	// verify algorithms
	printf("\nChecksums for     MS_d,   MS_e,   MS_lite,   MS_xlite:   (with Jan2025 update)\n");
	printf(  "               ");
	for (algo=4; algo<=7; algo++) {
		chks=algoChkSum(algo);  // show algo chksum
		printf("  %d",chks);
		if (chks != stdChkSums[algo-4]) PRIG_err = true;
	}
	printf("\n should be:      6351454  5980121  6603945  6386077.\n");

	printf("\nMean must=256.0:");
	for (algo=4; algo<=7; algo++) {
		mean = MeanTest(algo, 314159);
		printf("  %.2f ",mean);  // show algo chksum
		if (mean != 256.0) PRIG_err = true;
		// test corner case for errors
		mean = MeanTest(algo, 0xFFFFFFFF);
		if (mean != 256.0) PRIG_err = true;
	}
	if (PRIG_err) printf("\n*** Error: computed results were wrong or inconsistant with PRIG funtionality.");
	printf("\n");

	reps[52]=0;
	/* Intializes random number generator */
	srand((unsigned) time(NULL));

    for (n=0; n<3; n++) {
	    shuffleID = rand();
	    for (i = 0; i < 52; i++) seen[i]=0;

	    printf("\n\n Shuffles using rand()  vs  Miller_Shuffle_Algo.\n    ");
	    // output generated instances of shuffles
		// ---------------------------------------
		for (i = 0; i < 52; i++) {
			item = rand()%52;                 // used to see rand() vs MSA_a
			//     ---------
			if (item<26) c=item+'a';
			else         c=item-26+'A';
			printf("%c",c);
			reps[i] = (seen[item])? '-':' ';
			seen[item]=1;
		}
		printf("\n    %s  <- repeats\n    ",reps); // highlight the repeated selections
		
		for (i = 0; i < 52; i++) {
			item = MillerShuffle(i, shuffleID, 52);
			//     -----------------
			if (item<26) c=item+'a';
			else         c=item-26+'A';
			printf("%c",c);
		}

	}
	printf("\n\n* Note the repeated letters in the rand() generated \"shuffles\".\n");
	printf(" FisherYate's output would ~= MillerShuffle, but requires a persistent RAM array.\n");
	printf(" With Fisher-Yates a new session normally requires a new shuffled array, you then get repeats between sessions.\n");
	printf(" With the Miller Shuffle Algorithm you easily avoid all the (inter & intra-session) annoying repeats.\n\n");

	//for (i = 0; i < 52; i++) {  // quick sanity check test of random()
	//	printf("random(52): %d \t  random(0): %u \n", random(52), random(0));
	//}

//#ifdef _DEBUG
	while(1); // so as to examine the stdout console, during dev.
//#endif
	return(0);
}

//			 Local Utilities
// ------------------------------------
unsigned int algoChkSum(int algo) {  // does a Simple Shifting Check Sum (both value and sequence dependant)
  unsigned int nlimit=256;
  unsigned int randCut;
  unsigned int i, item, csum, lim;
  char sh;
  
  srand(314159); // set for repeatabilty on use of rand()
  randCut = 314159;   //
  csum=sh=0;

  for (lim=nlimit; lim<(nlimit+3); lim++) {
	  for (i=0; i<(2*lim); i++) // '2*' in order to exercise input inx overflow feature
	  {
	//    if (algo==1)      item = MillerShuffleAlgo_a(i, randCut, lim); 
	//    else if (algo==2) item = ShuffleAlgo_b(i, randCut, lim); 
	//	if (algo==3)      item = MillerShuffleAlgo_c(i, randCut, lim); 
		if (algo==4)      item = MillerShuffleAlgo_d(i, randCut, lim);
		else if (algo==5) item = MillerShuffleAlgo_e(i, randCut, lim);
		else if (algo==6) item = MillerShuffle_lite (i, randCut, lim); 
		else if (algo==7) item = MillerShuffle_xlite(i, randCut, lim);  
		csum += (item<<sh);
		if (++sh==8) sh=0;
	  }
  }
  //printf("   Algorithm %d chkSum: %d\n",algo,csum);
  return (csum);
}

// ------------------------------------
float MeanTest(int algo, unsigned int randCut) {  // does a Simple Shifting Check Sum (both value and sequence dependant)
      // for nlimit=513 the mean Must be = 256.0 else there is a problem with the shuffle
  unsigned int genMax, nlimit=513; // use nlimit != 2^n for full range error detection
  unsigned int i, item, sum;
  
  srand(314159); // set for repeatabilty on use of rand()
  sum=0;
  genMax=2*nlimit; // genMax must be a multiple of nlimit

  for (i=0; i<genMax; i++) 
  {
//    if (algo==1)      item = MillerShuffleAlgo_a(i, randCut, nlimit); 
//    else if (algo==2) item = ShuffleAlgo_b(i, randCut, nlimit); 
//    if (algo==3) item = MillerShuffleAlgo_c(i, randCut, nlimit); 
    if (algo==4)      item = MillerShuffleAlgo_d (i, randCut, nlimit); 
    else if (algo==5) item = MillerShuffleAlgo_e (i, randCut, nlimit); 
    else if (algo==6) item = MillerShuffle_lite (i, randCut, nlimit); 
    else if (algo==7) item = MillerShuffle_xlite(i, randCut, nlimit); 
    sum += item;
  }

  //printf("   Algorithm %d (255.5)Mean: %.2f\n",algo,(float)sum/genMax);
  return ((float)sum/genMax);
}

// ------------------------------------
unsigned int random(unsigned int limit) {  // 15 bit to full 32bit PRNG hack (~= arduino 'random()')
	unsigned int randv;

	// -----------  collect 32 bits from 3 15-bit rand() values
	//randv = ((unsigned int) rand()<<17) | rand()<<2 | rand()>>13;  // ~25 repeats/mil
	randv = (unsigned int)rand() | (rand() << 15) | (rand() << 30);  // ~15 repeats/mil
	rand();    // + another rand() for: 0 repeats/500Mil !!!!!  Careful compiler optimization will remove this & more.

	if (limit!=0) randv = randv % limit;   // with limit==0 provide maximum range results
	return(randv);
}
