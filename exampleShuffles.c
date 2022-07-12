
#include <stdio.h>
#include <stdlib.h>
#include "MillerShuffle.h"

/*------------------------------------------------------------------*/
/*  Generates samples of shuffling 52 letters a-zA-Z (~songs)       */
/*                                                                  */

int main(int argc, char **argv)
{
	int i, n, item;
	char c,rep;
	char reps[53];
	char seen[52];
	int shuffleID;
   
	reps[52]=0;
	/* Intializes random number generator */
	srand((unsigned) time(NULL));

    	for (n=0; n<3; n++) {
	    shuffleID = rand();
	    for (i = 0; i < 52; i++) seen[i]=0;

	    printf("\n\n  Shuffles using rand() vs Miller_Shuffle_Algo.\n");
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
		printf("\nrepeats: %s\n         ",reps); // highlight the repeated selections

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
