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
// Miller Shuffle Algorithm D variant
unsigned int MillerShuffleAlgo_d(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int si, r1, r2, r3, r4, rx, rx2;
  const unsigned int p1=24317, p2=32141, p3=63629;  // for shuffling 60,000+ indexes

  shuffleID ^= (inx/listSize);  // have inx overflow effect the mix
  si=(inx+(shuffleID & 0x7FFFFFFF))%listSize;    // cut the deck

  r1=shuffleID%p1+42;   // randomizing factors crafted empirically (by automated trial and error)
  r2=((shuffleID/0x89)^r1)%p2;
  r3=(r1+r2+p3)%listSize;
  r4=r1^r2^r3;
  rx = (shuffleID/listSize) % listSize + 1;
  rx2 = ((shuffleID/listSize/listSize)) % listSize + 1;

  // perform conditional multi-faceted mathematical spin-mixing (on avg 2 1/3 shuffle ops done + 2 simple Xors)
  if (si%3==0) si=(((unsigned long)(si/3)*p1+r1) % ((listSize+2)/3)) *3; // spin multiples of 3 
  if (si%2==0) si=(((unsigned long)(si/2)*p2+r2) % ((listSize+1)/2)) *2; // spin multiples of 2 
  if (si<listSize/2) si=((unsigned long)si*p3+r4) % (listSize/2);

  if ((si^rx) < listSize)   si ^= rx;			// flip some bits with Xor
  si = ((unsigned long)si*p3 + r3) % listSize;  // relatively prime gears turning operation
  if ((si^rx2) < listSize)  si ^= rx2;
	
  return(si);  // return 'Shuffled' index
}


// --------------------------------------------------------------
// Miller Shuffle Algorithm E variant
// Produces nearly the same randomness within the shuffles as MSA-d,
// with changes and added code to increase the potential number of shuffle permutations generated. 
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


//===========================================================================================================
// Other Algorithms of possible interest


// --------------------------------------------------------
// Shuffle Algorithm B  (a non standard variation)
// Demoted from "MSA_b" but it remains as an interesting benchmark.
// With this algo there is not a 1:1 between inx IN and OUT repeatability due to the use of rand().
// ~Better at randomixing pattern occurrences, providing very good sequence distribution over time.
// As coded here -b internally depends on a standard version of an MSA Algo.
// 
// Use of SA-b is not generally advised due to:  Besides this algo not being repeatable 1:1 between inx IN & OUT;
// This algo won't work for concurrent or nested shuffles !
// Further SA-b shuffles must be started with inx=0 and end with inx=listSize-1.
//
unsigned int ShuffleAlgo_b(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int xi,si;
  static unsigned int opti=-1; // opt inx for swapping
  int seed=0;

  xi = (inx%listSize);      // allow an over zealous inx
  shuffleID ^= (inx/listSize);  // & have it effect the mix
  
  if (opti==-1 || inx==0) {seed=1;}
  do {
      si = (seed)? listSize-1 : xi;
    
      si = MillerShuffle(si, shuffleID, listSize); // use a regular MSA
       
      if (seed) {opti=si;}
  } while (seed--);
  
  if (xi==(listSize-1)) {
    si=opti;
    opti=-1;
  } else {
    if (rand()%3==1) {  // ~1/3 time return saved opt.inx
       xi=si;
       si=opti % listSize;  // be sure # is in current range
       opti=xi;
    }
  }
  
  return(si);  // return 'Shuffled' index
}


// --------------------------------------------------------
// Miller Shuffle lite and extra lite, 
// produces a shuffled Index given a base Index, a random seed and the length of the list being indexed
// for each inx: 0 to list Size -1, unique indexes are returned in a pseudo "random" order.
// 
// This variation of the Miller Shuffle algorithm is for when you need/want minimal coding and processing, 
// to achieve excellent randomness along with desirable shuffle characteristics. (eg: in an 8-bit MCU project)
// For a simple shuffle this works really well; unlike using rand() which does not. (used by DDesk_Shuffle)
// These variations (_xlite & _lite) now perform much better than MSA_c did.

// Both updated feb 2024 to be simpler (less code & memory use) but with better performance test results.
// MS_xlite produces 10s of million of unique permutations while MS_lite does over 99/100 unique million


// MS x-lite (2024)   [Much better than MSA_c was or the old MS_lite]
// For a given 'MixID'&'nlim' it will give out Pseudo Random Indexes of 0 to nlim-1 for provided 'inx's in the same range.
// 'mixID' is a fixed 32bit random # used for a given shuffle.  'nlim' must be <p1
short int MillerShuffle_xlite(short inx, unsigned long mixID, short nlim) {
	const short p1 = 9949, p2 = 9973;  // limits nlim to < 9949
	short si, r1, r2, rx;

	mixID ^= (inx / nlim);  // have inx overflow effect the mix
	si = (inx + (mixID & 0x7FFFFFFF)) % nlim;    // cut the deck

	r1 = mixID % p2;	// set of fixed randomizing values
	r2 = mixID % p1;
	rx = (mixID / nlim) % nlim + 1;

	// perform conditional multi-faceted mathematical spin-mixing  (average # of si recalcs performed is ~1.5)
	if (si % 3 == 0) si = (((long)(si / 3) * p1 + r1) % ((nlim + 2) / 3)) * 3; // spin multiples of 3 
	if (si % 2 == 0) si = (((long)(si / 2) * p2 + r2) % ((nlim + 1) / 2)) * 2; // spin multiples of 2 
	if (si < rx) si = ((long)si * p2 + r1) % rx;                       // mix random half one way
	else         si = ((long)(si - rx) * p1 + r2) % (nlim - rx) + rx;  // and the other another

	return(si);  // return 'Shuffled' index
}

// MS Lite 2024 Update    [Now default PRIG() function in my game software]
// For a given 'MixID'&'nlim' it will give out Pseudo Random Indexes of 0 to nlim-1 for provided 'inx's in the same range.
// 'mixID' is a fixed 32bit random # used for a given shuffle.  'nlim' must be <p2
short int MillerShuffle_lite(short inx, unsigned long mixID, short nlim) {
	const short p1 = 3343, p2 = 9949, p3 = 9973;   // limits nlim to < 9949
	short si, r1, r2, r3, rx;

	mixID ^= (inx / nlim);  // have inx overflow effect the mix
	si = (inx + (mixID & 0x7FFFFFFF)) % nlim;    // cut the deck

	r1 = mixID % p3;	// set of fixed randomizing values
	r2 = mixID % p1;
	r3 = mixID % p2;
	rx = (mixID/nlim) % nlim + 1;

	// perform conditional multi-faceted mathematical spin-mixing 
	if (si % 3 == 0) si = (((long)(si / 3) * p1 + r1) % ((nlim + 2) / 3)) * 3; // spin-mix multiples of 3 
	if (si % 2 == 0) si = (((long)(si / 2) * p2 + r2) % ((nlim + 1) / 2)) * 2; // spin-mix multiples of 2 
	if (si < rx) si = ((long)si * p3 + r2) % rx;                       // mix random half one way
	else         si = ((long)(si - rx) * p2 + r1) % (nlim - rx) + rx;  // and the other another
	si = ((long)si * p3 + r3) % nlim;		// relatively prime gears churning operation

	return(si);  // return 'Shuffled' index
}


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
