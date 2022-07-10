
#include <stdio.h>
#include <stdlib.h>
#include "MillerShuffle.h"

/*------------------------------------------------------------------*/
/*  Generates samples of shuffling 52 letters a-zA-Z (~songs)       */
/*                                                                  */
/*  You may run this a few times to get demonstrative comparisons   */ 

int main(int argc, char **argv)
{
	int i, n, item;
	char c;
	int shuffleID;
   
	/* Intializes random number generator */
	srand((unsigned) time(NULL));

    for (n=0; n<3; n++) {
		printf("\n\n  Shuffles using rand() vs Miller_Shuffle_Algo.\n");
		// output generated instances of shuffles
		// ---------------------------------------
		for (i = 0; i < 52; i++) {
			item = rand()%52;
			//     ---------
			if (item<26) c=item+'a';
			else         c=item-26+'A';
			printf("%c",c);
		}
		printf("\n");

		shuffleID = rand();
		for (i = 0; i < 52; i++) {
			item = MillerShuffleAlgo(i, shuffleID, 52);
			//     -----------------
			if (item<26) c=item+'a';
			else         c=item-26+'A';
			printf("%c",c);
		}

	}
	printf("\n\n* Note the repeated letters in the rand() generated \"shuffles\".\n");
	printf(" FisherYate's output would ~= MillerShuffle, but requires a persistent RAM array.\n\n");

#ifdef _DEBUG
	while(1); // so as to examine the stdout console, during dev.
#endif
	return(0);
}
