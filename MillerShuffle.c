// ================================================================
// the Miller Shuffle Algorithm
//
// source: https://github.com/RondeSC/Miller_Shuffle_Algo
// license: Attribution-NonCommercial-ShareAlike
// Copyright 2022 Ronald R. Miller
// http://www.apache.org/licenses/LICENSE-2.0
//
// Update Aug 2022 added Miller Shuffle Algo-C (also a _lite version)
//   the Algo-C is by & large suitable to replace both algo-A and -B
// Update April 2023 added Miller Shuffle Algo-D 
//   The NEW Algo-D performs superiorly to earlier variants (~Fisher-Yates statistics).
//   Optimized so as to generate greater permutations of possible shuffles over time.
//   A -Max variant was also added, pushing (obsessively?) random-permutations to the limit.
// Update May 2023 added Miller Shuffle Algo-E and improved MS-lite.
//   Removed MSA-A and MSA-C as obsolite, having no use cases not better served by MS-lite or MSA-D
// Update June 2023: greatly increased the potential number by unique shuffle permutations for MSA_d & MSA_e
//    Also improved the randomness and permutations of MS_lite.
// Update Aug 2023: updated MillerShuffleAlgo-E to rely on 'Chinese remainder theorem' to ensure unique randomizing factors
// Update Feb 2024: simplified and greatly improved MS_lite and MS_xlite. Fixed possible MSA_e edge case issue.
// Update Jan 2025: rework calculation of Randomizing Constants, to eliminate possible range rollovers issues.
// Update Aug 2025: Improved MS_lite, removed MSA_d and MS_xlite in favor of MSA_e and MS_lite, removed MS_b as not recomended albeit interesting

#include "MillerShuffle.h"

// --------------------------------------------------------------
// the Miller Shuffle Algorithms
// produces a shuffled Index given a base Index, a shuffle ID "seed" and the length of the list being
// indexed. For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
// Utilizes minimum resources. 
// As such the Miller Shuffle algorithm is the better choice for a playlist shuffle.
//
// The 'shuffleID' is an unsigned 32bit value and can be selected by utilizing a PRNG.
// Each time you want another pseudo random index from a current shuffle (incrementing 'inx')
// you must be sure to pass in the "shuffleID" for that shuffle.
// Note that you can exceed the listSize with the input 'inx' value and get very good results,
// as the code effectively uses a secondary shuffle by way of using a 'working' modified value of the input shuffle ID.


// --------------------------------------------------------------
// Miller Shuffle Algorithm E variant
// Produces nearly the same randomness within the shuffles as MSA-d did,
// with changes and added code to significantly increase the potential number of shuffle permutations generated. 
// 

unsigned int MillerShuffleAlgo_e(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  static unsigned int si, randR, r1, r2, r3, r4;
  unsigned int halfN, rx, rkey;
  const unsigned int p1=24317, p2=32141, p3=63629; // good for shuffling >60,000 indexes
  #define KEYCODE 0

  shuffleID ^= (inx/listSize);  // have inx overflow effect the mix
  si=(inx + (shuffleID%listSize)) % listSize;    // cut the deck

  if (shuffleID!=randR) { // compute fixed randomizing values once for a given shuffle
	  randR=shuffleID;   //local randomizer
	  r1 = randR % p3;
	  r2 = randR % p1; // Now, per Chinese remainder theorem, (r1,r2,r3) will be a unique set
	  r3 = randR % p2; 
	  r4 = randR % 2749;
  }
  halfN = listSize/2 + 1;
  rx = ((randR / listSize) ^ KEYCODE) % listSize + 1;		// shuffles can be customized by using a KEYCODE
  rkey = ((randR / listSize / listSize) ^ KEYCODE) % listSize + 1;
  
  // perform the conditional multi-faceted mathematical mixing (on avg 2 5/6 shuffle ops done + 2 simple Xors)
  if (si%3==0)       si = (((unsigned long)(si/3)*p1+r1) % ((listSize+2)/3)) *3; // spin multiples of 3 
  if (si <= halfN)  {si = (si + r3) % (halfN + 1); si = halfN - si;}  // improves large permu distro
  if (si%2==0)       si = (((unsigned long)(si/2)*p2+r2) % ((listSize+1)/2)) *2; // spin multiples of 2 
  if (si < halfN)    si = ((unsigned long)si * p3 + r3) % halfN;

  if ((si^rx) < listSize)   si ^= rx;			// flip some bits with Xor
  si = ((unsigned long)si*p3 + r4) % listSize;  // a relatively prime gears churning operation
  if ((si^rkey) < listSize)  si ^= rkey;
  
  return(si);  // return 'Shuffled' index
}

// --------------------------------------------------------
// Miller Shuffle lite, 
// produces a shuffled Index given a base Index, a random seed and the length of the list being indexed
// for each inx: 0 to list Size -1, unique indexes are returned in a pseudo "random" order.
// 
// This variation of the Miller Shuffle algorithm is for when you need/want to use minimal resources, 
// to achieve excellent randomness along with desirable shuffle characteristics. (eg: in an 8-bit MCU project)
// For a simple shuffle this works really well; unlike using rand() which does not. (used by DDesk_Shuffle)
// This variation now performs on par this what MSA_d did.
// Produces over 99 million unique permutations out of 100 million shuffles (only 60 repeats out of the first million).

// MS Lite Aug 2025 lastest & bet yet,    [Now default PRIG() function in my game software]
// For a given 'MixID'&'nlim' it will give out Pseudo Random Indexes of 0 to nlim-1 for provided 'inx's in the same range.
// 'mixID' is a fixed 32bit random # used for a given shuffle.  'nlim' must be <p1
short int MillerShuffle_lite(short inx, unsigned long mixID, short nlim) {
	unsigned short si;
	static unsigned short r1, r2, rx, rx2;
	const unsigned short p1 = 9949, p2 = 9973;   // prime #s  must be > nlim
	static unsigned long randR;

	mixID ^= 13 * (inx / nlim);   // have inx overflow effect the mix
	si = ((mixID / 11) + inx) % nlim;   // cut the deck

	if (mixID != randR) { // compute fixed randomizing values once for a given shuffle
		randR = mixID;   //local randomizer
		r1 = mixID % p2;	// set of fixed randomizing values
		r2 = mixID % p1;
		rx = (mixID / nlim) % nlim + 1;
		rx2 = (mixID / 131) % nlim + 1;
	}

	// perform conditional multi-faceted mathematical spin-mixing
	if (si % 2 == 0) si = (((long)(si / 2) * p2 + r2) % ((nlim + 1) / 2)) * 2; // spin multiples of 2 
	if ((si ^ rx2) < nlim) si ^= rx2;
	if (si < rx) si = ((long)(rx - si - 1) * p2 + r1 + r2) % rx;        // mix random half one way
	else         si = ((long)(si - rx) * p1 + r2) % (nlim - rx) + rx;  // and the other another
	if (si % 3 == 0) si = (((long)(si / 3) * p1 + r1) % ((nlim + 2) / 3)) * 3; // spin multiples of 3 

	return(si);  // return 'Shuffled' index
}

// _xlite ? notablly above lite algo could be abridged ...
//                                   ___ Permus ___
//							Chi2    missed  avg devi  Repeats/1M
// from the 5 lines above   255		0		  12.0	  62   R/1M  (~= MSA-d)
// with 4 lines 1,2,3,5     256		36  	  20.0	  1800 R/1M  better than MS_Xlite
// with 3 lines 1,2,3:      270		477  	  59.0	  21K  R/1M
// with 2 lines 1,3:        285		3638	  94.4	  269K R/1M  better than MSA-c was


//===========================================================================================================
// Other Algorithms of possible interest

// here is a little fun aside
// If one wanted a 'shuffle' that behaved between rand() and either FY or MSA you could use the following ...
// --------------------------------------------------------
// DOUBLE DECK Shuffle
// generates shuffled indexes, in a similar fashion as dealing from a Double Deck where you get 1 of upto 52
// unique cards yet can get the same card a second time, but not three or more of any one.
// Dealing out 1/2 the 'listSize' you get ~1/2 as many repeats 1/2 as often, compared to using random()
//
// This is a variation of the Miller Shuffle Algo; it will provide 2 of each index ( <listSize ) 
// in 2*listSize calls (with inx of 0 to 2*listSize-1), with a great, yet moderated, random behavior.
// Preferred for doling out some exercise or test items (of eg: Morse code).
// When using it for Black-Jack you may want to change nDecks to 4 or 6.
unsigned int DDeck_Shuffle(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int si;
  const nDecks=2;

  listSize=listSize * nDecks; // Double the processing range
  
  si = MillerShuffle_lite(inx, shuffleID, listSize);

  return(si/nDecks);  // return a single desk 'Shuffled' index
}
// of course, given any MillerShuffle(), this stand alone function could have been implemented in-line like this:
// 
//      item = MillerShuffle(inx, RandizeR, 2*listSize)/2;
//
