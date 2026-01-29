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
// Update June 2023: greatly increased the potential number of unique shuffle permutations for MSA_d & MSA_e
//    Also improved the randomness and permutations of MS_lite.
// Update Aug 2023: updated MillerShuffleAlgo-E to rely on 'Chinese remainder theorem' to ensure unique randomizing factors
// Update Feb 2024: simplified and greatly improved MS_lite and MS_xlite. Fixed possible MSA_e edge case issue.
// Update Jan 2025: rework calculation of Randomizing Constants, to eliminate possible range rollovers issues.
// Update Aug 2025: Improved MS_lite, removed MSA_d and MS_xlite in favor of MSA_e and MS_lite, removed MS_b as not recomended albeit interesting
// Update Jan 2026: Expanded the maxium # of items with MS_lite to 32,768 & MS_e to 65,536. 
//    Optimized the randomizing constants being utilized. And added a MS_Large algo that can shuffle >4 billion items.


#include <stdint.h>
#include "MillerShuffle.h"

// --------------------------------------------------------------
// the Miller Shuffle Algorithms
// produces a shuffled Index given a base Index, a shuffle ID "seed" and the length of the list being
// indexed. For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
// Utilizes minimal resources. 
// As such the Miller Shuffle algorithm is the better choice for a playlist shuffle.
//
// The 'shuffleID' is an unsigned 32bit value and can be selected by utilizing a PRNG.
// Each time you want another pseudo random index from a current shuffle (typically an increment of 'inx')
// you must be sure to pass in the "shuffleID" for that shuffle.
// Note that you can exceed the listSize with the input 'inx' value and get very good results,
// as the code effectively uses a subsequent shuffle, by way of using a 'working' modified value of the input shuffle ID.


// --------------------------------------------------------------
// Miller Shuffle Algorithm E variant
// Produces nearly prefectly random shuffles of 2 to 65536 items. Which, for 52 items, are unique for any given 'shuffleID'
// 
// 'inx'  arbitary reference index. Nomally 0 to listSize-1, for values beyond that subsequent shuffles are effectively accessed
// 'shuffleID' (unique 32bit value) identifies and dictates a specific shuffle of the items
// 'listSize' the number of logical indexes to be shuffled.  range: 1-65536
//
//  returns: a 'shuffled' index from 0 to listSize-1

uint32_t MillerShuffleAlgo_e(uint32_t inx, uint32_t shuffleID, uint32_t listSize) {
  static uint32_t si, randR, r1, r2, r3, r4;
  static uint32_t lastLim = 0;
  uint32_t halfN, rx, rkey;
  const uint32_t p1= 41603, p2= 40751, p3= 98221;
  #define KEYCODE 0

  // handle errant edge cases
  if (listSize == 0) return 0;
  if (listSize <= 2) return (((shuffleID/(inx/2+1)) + inx) % listSize); // as shuffled as 2 can be

  shuffleID ^= 19 * (inx / listSize);   // have inx overflow effect the mix
  si=(inx + (shuffleID%listSize)) % listSize;    // carefully cut the deck

  if (shuffleID != randR || (listSize != lastLim)) { // compute fixed randomizing values as needed for a given shuffle
	  randR=shuffleID;   //local randomizer
	  lastLim = listSize;
	  r1 = randR % p3;
	  r2 = randR % p1; // per Chinese remainder theorem, (r1,r2,r3) will be a unique set
	  r3 = randR % p2; 
	  r4 = randR % 461;
  }
  halfN = listSize/2 + 1;
  rx = ((randR / listSize) ^ KEYCODE) % listSize + 1;		// shuffles can be customized by using a KEYCODE
  rkey = ((randR / listSize / listSize) ^ KEYCODE) % listSize + 1;
  
  // perform the conditional multi-faceted mathematical mixing
  if (si%3==0)       si = (((si/3)*p1+r1) % ((listSize+2)/3)) *3; // spin multiples of 3 
  if (si <= halfN)  {si = (si + r3) % (halfN + 1); si = halfN - si;}  // improves large permu distro
  if (si%2==0)       si = (((si/2)*p2+r2) % ((listSize+1)/2)) *2; // spin multiples of 2 
  if (si < halfN)    si = (si * p3 + r3) % halfN;

  if ((si^rx) < listSize)   si ^= rx;			// flip some bits with Xor
  si = ((uint64_t)si*p3 + r4) % listSize;  // a relatively prime gears churning operation
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
// For a quality shuffle this works really well; unlike using rand() which does not.
// This variation now performs on par with what MSA_d did. And is used by DDesk_Shuffle.
// Produces millions of unique shuffles (generally >100,000 before any get repeated).

// MS_Lite Jan 2026 last update
// For a given 'MixID'&'nlim' it will give out Pseudo Random Indexes of 0 to nlim-1 for provided 'inx's in the same range.
// Producing very highly random shuffles of 2 to 32768 items.
//
// 'inx'  arbitary reference index. Nomally 0 to nlim-1, for values beyond that subsequent shuffles are effectively accessed
// 'mixID' (unique 32bit value) identifies and dictates a specific shuffle of the items
// 'nlim' the number of logical indexes to be shuffled.  range: 1-32768
//
//  returns: a 'shuffled' index from 0 to nlim-1

unsigned short int MillerShuffle_lite(unsigned short inx, unsigned long mixID, unsigned short nlim) {
	unsigned short si;
	unsigned short r1, r2, rx, rx2;
	const unsigned short p1 = 52639, p2 = 33703;   // key prime #s > nlim

	if (nlim == 0 || nlim > 32768) return 0;
	mixID ^= 13 * (inx / nlim);          // have inx overflow effect the mix
	si = (inx + (mixID % nlim)) % nlim;  // carefully cut the deck

	r1 = mixID % 1063;	// set of fixed randomizing values
	r2 = mixID % 3631;
	rx = (mixID / nlim) % nlim + 1;
	rx2 = (mixID / 131) % nlim + 1;

	// perform conditional multi-faceted mathematical spin-mixing
	if (si % 3 == 0) si = (((long)(si / 3) * p1 + r1) % ((nlim + 2) / 3)) * 3; // spin multiples of 3 
	if (si % 2 == 0) si = (((long)(si / 2) * p2 + r2) % ((nlim + 1) / 2)) * 2; // spin multiples of 2 
	if ((si ^ rx2) < nlim) si ^= rx2;
	if (si < rx) si = ((long)(rx - si - 1) * p2 + r1 + r2) % rx;        // mix random half one way
	else         si = ((long)(si - rx) * p1 + r2) % (nlim - rx) + rx;  // and the other another

	return(si);  // return 'Shuffled' index
}

// --------------------------------------------------------
// Miller Shuffle Large,
// produces very highly random shuffles for up to a full count of 4,294,967,296 items.
// Produces <0.1 repeated shuffles in a million.
//
// 'inx'  arbitary reference index. Nomally 0 to nLimit-1, for values beyond that subsequent shuffles are effectively accessed
// 'mixID' (unique 32bit value) identifies and dictates a specific shuffle of the items
// 'nLimit' the number of logical indexes to be shuffled.  range: 0 to 2^32-1 (use nLim=0 to shuffle a full 2^32 items)
//
//  returns: a 'shuffled' index from 0 to nLimit-1

uint32_t MillerShuffle_Large(uint32_t inx, uint32_t mixID, uint32_t nLimit) {  // coded to shuffle item counts of 1 to 2^32 (4.3B)
	  																		  // use nLim=0 to shuffle a full 2^32 items
	uint32_t si;
	static uint32_t r1, r2, r3, r4, rx, rx2;
	static uint64_t p1 = 98323, p2 = 93251, p3 = 66107;   // prime #s  are uint64 so that all the * ops are done in 64 bit
	static uint32_t randR = 0;
	static uint32_t lastLim = 0;
	static uint64_t nlim = 0;

	nlim = (nLimit == 0) ? 4294967296ULL : nLimit;
	mixID ^= 19 * (inx / nlim);   // have inx overflow effect the mix

	si = ((uint64_t)inx + (mixID % nlim)) % nlim;    // carefully cut the deck

	if (mixID != randR || (nlim != lastLim)) { // compute fixed randomizing values as needed for a given shuffle
		randR = mixID;   //local randomizer
		lastLim = nlim;
		p1 = 98323; p2 = 93251; p3 = 66107; // re-establish the primary primes

		if (nlim % p1 == 0) p1 = p3;	// side step issues with nlim being a multiple of one of the prime #s
		if (nlim % p2 == 0) p2 = p3;
		r1 = mixID % p2;	// setup fixed randomizing values for this shuffle
		r2 = mixID % p1;
		r3 = mixID % 6983;
		r4 = mixID % 4793;
		rx = (mixID / nlim) % nlim + 1;
		rx2 = (mixID / 43) % nlim + 1;

		// ----- prevent any key values from being used as a modulo base which is applied to main prime multipler 
		if ((((uint64_t)nlim + 1) / 2) % p2 == 0) p2 = p3;   // handles 131197 *N    (the uint64_t is so as to support nlim upto 0xFFFFFFFF)
		if ((((uint64_t)nlim + 2) / 3) % p1 == 0) p1 = p3;   // handles 196609 *N
		if ((rx % p2) == 0) rx--;
		if ((nlim - rx) % p1 == 0) rx--;
	}

	// perform conditional multi-faceted mathematical spin-mixing
	if (si % 3 == 0) si = (((uint64_t)(si / 3) * p1 + (r1)) % (((uint64_t)nlim + 2) / 3)) * 3; // spin multiples of 3  
	if (si % 2 == 0) si = (((uint64_t)(si / 2) * p2 + r2) % (((uint64_t)nlim + 1) / 2)) * 2;   // spin multiples of 2 
	if (si < rx) si = ((uint64_t)(rx - si - 1) * p2 + (r3 + r1)) % rx;       // mix random half one way
	else         si = ((uint64_t)(si - rx) * p1 + (r3)) % (nlim - rx) + rx;  // and the other another
	if ((si ^ rx2) < nlim) si ^= rx2;
	si = ((uint64_t)(nlim - si) * p1 + r4) % nlim;  // extra large item piles need extra mixing

	return(si);  // return 'shuffled' index
}


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
// This is a adaptation of the Miller Shuffle Algo; it will provide 2 of each index ( <listSize ) 
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
