
#include <stdio.h>
#include <stdlib.h>
#include "MillerShuffle.h"

unsigned int algoChkSum(int); // advance prototype
float MeanTest(int);

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
   
	// verify algorithms
	printf("\nChecksums for     MS_a,    MS_b,    MS_c,   MS_d,   MS_lite: \n");
	printf(  "                 ......   ......");
	for (algo=3; algo<=5; algo++) {
		chks=algoChkSum(algo);  // show algo chksum
		printf("  %d",chks);
	}
	printf("\n should be:     6273191, 6162166, 6389589, 6355280, 6463017.   note: MS_b is dependant on sys rand() implementation\n");
    // MSA_Max algoChkSum(6)=6476111

	printf("\nMean must=255.5:                ");
	for (algo=3; algo<=5; algo++) {
		printf("  %.2f ",MeanTest(algo));  // show algo chksum
	}
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
			item = rand()%52;                               // used to see rand() vs MSA_a
			//     ---------
			//item = MillerShuffleAlgo_b(i, shuffleID, 52); // used to see MSA_b vs MSA_a
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
	printf(" With Fisher-Yates a new session requires a new shuffled array, you then get repeats between sessions.\n");
	printf(" With the Miller Shuffle Algorithm you easily avoid all the (inter & intra-session) annoying repeats.\n\n");

#ifdef _DEBUG
	while(1); // so as to examine the stdout console, during dev.
#endif
	return(0);
}


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
	  for (i=0; i<(2*lim); i++) 
	  {
	//    if (algo==1)      item = MillerShuffleAlgo_a(i, randCut, lim); 
	//    else if (algo==2) item = MillerShuffleAlgo_b(i, randCut, lim); 
		if (algo==3)      item = MillerShuffleAlgo_c(i, randCut, lim); 
		else if (algo==4) item = MillerShuffle      (i, randCut, lim); // currently = MSA_d
		else if (algo==5) item = MillerShuffle_lite (i, randCut, lim); 
		else if (algo==6) item = MillerShuffle_Max (i, randCut, lim); 
		csum += (item<<sh);
		if (++sh==8) sh=0;
	  }
  }
  //printf("   Algorithm %d chkSum: %d\n",algo,csum);
  return (csum);
}

// ------------------------------------
float MeanTest(int algo) {  // does a Simple Shifting Check Sum (both value and sequence dependant)
      // for nlimit=512 the mean Must be = 255.5 else there is a problem with the shuffle
  unsigned int genMax, nlimit=512;
  unsigned int randCut;
  unsigned int i, item, sum;
  
  srand(314159); // set for repeatabilty on use of rand()
  randCut = 314159;   //
  sum=0;
  genMax=2*nlimit; // genMax must be a multiple of nlimit

  for (i=0; i<genMax; i++) 
  {
//    if (algo==1)      item = MillerShuffleAlgo_a(i, randCut, nlimit); 
//    else if (algo==2) item = MillerShuffleAlgo_b(i, randCut, nlimit); 
    if (algo==3) item = MillerShuffleAlgo_c(i, randCut, nlimit); 
	else if (algo==4) item = MillerShuffle      (i, randCut, nlimit); 
    else if (algo==5) item = MillerShuffle_lite (i, randCut, nlimit); 
    else if (algo==6) item = MillerShuffle_Max (i, randCut, nlimit); 
    sum += item;
  }

  //printf("   Algorithm %d (255.5)Mean: %.2f\n",algo,(float)sum/genMax);
  return ((float)sum/genMax);
}
