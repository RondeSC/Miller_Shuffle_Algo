// ================================================================
// the Miller Shuffle Algorithm
//
// source: https://github.com/RondeSC/Miller_Shuffle_Algo
// license: Attribution-NonCommercial-ShareAlike
// Copyright 2022 Ronald R. Miller
// http://www.apache.org/licenses/LICENSE-2.0
//
// 29 Aug 22 Updated and added MillerShuffle_c(), also a _lite version
//   the NEW Algo-C is by & large suitable to replace both algo-A and -B
// Update April 2023 added Miller Shuffle Algo-D
//   The NEW Algo-D performs superiorly to earlier variants (~Fisher-Yates statistics).
//   Optimized so as to generate greater permutations of possible shuffles over time.
//   It is the perferred variant over algo_a _b & _c
// Update May 2023 added Miller Shuffle Algo-E
//   Removed MSA-A and MSA-C as obsolite, having no use cases not better served by MS-lite or MSA-D
// Update June 2023: greatly increased the potential number by unique shuffle permutations for MSA_d & MSA_e
//    Also improved the randomness and permutations of MS_lite.
// Update Aug 2023: updated MillerShuffleAlgo-E to rely on 'Chinese remainder theorem' to ensure unique randomizing factors
// Update Feb 2024: simplified and improved MS_lite
// Update Jan 2025: reworked Randomizing Constants, to stop range rollovers and javascript miss-handlings.
// Update Aug 2025: Improved MS-lite, removed MSA_d in favor of MAS_e and MS_lite, remove MS_b as not recomended albeit interesting
// Update Jan 2026: Expanded the maxium # of items with MS_lite to 32,768 & MS_e to 65,536. 
//    Optimized the randomizing constants being utilized. And added a MS_Large algo that can shuffle >4 billion items.

// --------------------------------------------------------
// Miller Shuffle, produces a shuffled Index given a base Index, a shuffle ID value and the length of the list being indexed.
// For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
// As such this Miller Shuffle algorithm is the better choice for a playlist shuffle.
//
// The 'shuffleID' is a 32bit value and be set by utilizing a PRNG. These bit determin the "random" shuffle.
// Each time you want another pseudo random index from a current shuffle (e.g. incrementing 'inx')
// you must be sure to pass in the "shuffleID" for that shuffle.
// Note that you can exceed the listSize with the input 'inx' value and get very good results,
// as the code effectively uses a secondary shuffle by way of using a 'working' modified value of the input shuffle ID.


// --------------------------------------------------------------
// Miller Shuffle Algorithm E variant
// Produces nearly prefectly random shuffles of 2 to 65536 items. Which, for 52 items, are unique for any given 'shuffleID'
//
// 'inx'  arbitary reference index. Nomally 0 to listSize-1, for values beyond that subsequent shuffles are effectively accessed
// 'shuffleID' (unique 32bit value) identifies and dictates a specific shuffle of the items
// 'listSize' the number of logical indexes to be shuffled.  range: 1-65536
//
//  returns: a 'shuffled' index from 0 to listSize-1
//
function MillerShuffle(inx, shuffleID, listSize) {  //    aka:   MillerShuffleAlgo_e
  var si, r1, r2, r3, r4;
  const p1=41603, p2=40751, p3=98221;  // for shuffling up to 65536 items
  var halfN, randR;  //local randomizer copy

  // handle errant edge cases
  if (listSize == 0) return 0;
  if (listSize <= 2) return ((Math.floor(shuffleID / (Math.floor(inx / 2) + 1)) + inx) % listSize); // as shuffled as 2 can be

  randR = Number(BigInt.asUintN(32, BigInt(shuffleID) ^ BigInt(19 * Math.floor(inx/listSize))));  //  have inx overflow effect the mix
  si = (randR % listSize + inx) % listSize;    // cut the deck

  r1 = randR % p3;
  r2 = randR % p1; // per Chinese remainder theorem, (r1,r2,r3) will be a unique set
  r3 = randR % p2; 
  r4 = randR % 461;
  halfN = Math.floor(listSize/2)+1;
  rx = Math.floor(randR/listSize) % listSize + 1;
  rkey = Math.floor(randR/listSize / listSize) % listSize + 1;

  // perform the conditional multi-faceted mathematical mixing (on avg 2 5/6 shuffle ops done + 2 simple Xors)
  if (si%3==0)      si = (((si/3)*p1+r1) % Math.floor((listSize+2)/3)) *3; // spin multiples of 3 
  if (si <= halfN) {si = (si + r3) % (halfN + 1); si = halfN - si;}  // improves large permu distro
  if (si%2==0)      si = (((si/2)*p2+r2) % Math.floor((listSize+1)/2)) *2; // spin multiples of 2 
  if (si < halfN)   si = (si * p3 + r3) % halfN;
  
  if ((si^rx) < listSize)   si ^= rx;			// flip some bits with Xor
  si = (si*p3 + r4) % listSize;  // relatively prime gears churning operation
  if ((si^rkey) < listSize)  si ^= rkey;
 
  return(si);  // return 'Shuffled' index
}


// ---------------------- MS-Lite     Jan 2026 last update
// Miller Shuffle lite, 
// produces a shuffled Index given a base Index, a random seed and the length of the list being indexed
// for each inx: 0 to nlim-1, unique indexes are returned in a pseudo "random" order.
// 
// This variation of the Miller Shuffle algorithm is for when you need/want to use minimal resources, 
// to acheive excellent randomness along with very desirable shuffle characteristics & millions of permutations.

// Produces very highly random shuffles of 2 to 32768 items.
// 'inx'  arbitary reference index. Nomally 0 to nlim-1, for values beyond that subsequent shuffles are effectively accessed
// 'mixID' (unique 32bit value) identifies and dictates a specific shuffle of the items
// 'nlim' the number of logical indexes to be shuffled.  range: 1-32768
//
//  returns: a 'shuffled' index from 0 to nlim-1

function MillerShuffle_lite(inx, mixID, nlim) {
  var si, r1, r2, rx, rx2;
  const p1 = 52639, p2 = 33703;
  var randR;

  if (nlim>32768) return 0;
  randR = Number(BigInt.asUintN(32, BigInt(mixID) ^ (BigInt(13 * Math.floor(inx / nlim)))));   //  have inx overflow effect the mix
  si = ((randR % nlim) + inx) % nlim;    // carefully cut the deck

  r1 = randR % 1063;	// set of fixed randomizing values for this shuffle
  r2 = randR % 3631;
  rx = Math.floor(randR / nlim) % nlim + 1;
  rx2 = Math.floor(randR / 131) % nlim + 1;

  // perform conditional multi-faceted mathematical spin-mixing  (average # of si recalcs performed is ~1.5)
  if (si % 3 == 0) si = (((si / 3) * p1 + r1) % Math.floor((nlim + 2) / 3)) * 3; // spin multiples of 3 
  if (si % 2 == 0) si = (((si / 2) * p2 + r2) % Math.floor((nlim + 1) / 2)) * 2; // spin multiples of 2
  if ((si ^ rx2) < nlim) si = si ^ rx2;
  if (si < rx) si = ((rx - si - 1) * p2 + r1 + r2) % rx;       // mix random half one way
  else si = ((si - rx) * p1 + r2) % (nlim - rx) + rx;          // and the other another

  return (si);  // return 'Shuffled' index
}


// ---------------------- MS-LARGE
// Miller Shuffle Large 

let randR = 0;              // these vars persists across function calls
let nlim = 0, lastLim = 0;
var si, r1, r2, r3, r4, rx, rx2;
let p1 = 98323, p2 = 93251, p3 = 66107;   // key prime #s

// Produces very highly random shuffles for up to a full count of 4,294,967,296 items.
// Produces <0.1 repeated shuffles in a million.
//
// 'inx'  arbitary reference index. Nomally 0 to nLimit-1, for values beyond that subsequent shuffles are effectively accessed
// 'mixID' (unique 32bit value) identifies and dictates a specific shuffle of the items
// 'nLimit' the number of logical indexes to be shuffled.  range: 0 to 2^32-1 (use nLim=0 to shuffle a full 2^32 items)
//
//  returns: a 'shuffled' index from 0 to nLimit-1
function MillerShuffle_Large(inx, mixID, nLimit) {  // coded to shuffle item counts of 1 to 2^32 (4.3B)													
    nlim = (nLimit == 0) ? 4294967296 : nLimit;  // with nLim = 0 a shuffle of 2 ^ 32 items is done
    
    mixID = Number(BigInt.asUintN(32, BigInt(mixID) ^ (BigInt(19 * Math.floor(inx / nlim)))));   //  have inx overflow effect the mix
	si = (inx + (mixID % nlim)) % nlim;    // carefully cut the deck

    if (mixID != randR || (nlim != lastLim)) { // compute fixed randomizing values once for a given shuffle
		p1 = 98323; p2 = 93251; p3 = 66107; // re-establish the primary primes

		randR = mixID;   //local randomizer
		lastLim = nlim;

		if (nlim % p1 == 0) p1 = p3;	// side step issues with nlim being a multiple of one of the prime #s
		if (nlim % p2 == 0) p2 = p3;
		r1 = mixID % p2;	// setup fixed randomizing values for this shuffle
		r2 = mixID % p1;
		r3 = (mixID % 6983);
		r4 = mixID % 4793;
        rx = Math.floor(mixID / nlim) % nlim + 1;
        rx2 = Math.floor(mixID / 43) % nlim + 1;

		// ----- prevent any key values from being used as a modulo base which is applied to main prime multipler
		if (Math.floor((nlim + 1) / 2) % p2 == 0) p2 = p3;   // handles 131197 *N    (the uint64_t is so as to support nlim upto 0xFFFFFFFF)
		if (Math.floor((nlim + 2) / 3) % p1 == 0) p1 = p3;   // handles 196609 *N
		if ((rx % p2) == 0) rx--;
		if ((nlim - rx) % p1 == 0) rx--;
	}

	// perform conditional multi-faceted mathematical spin-mixing
	if (si % 3 == 0) si = (((si / 3) * p1 + (r1)) % Math.floor((nlim + 2) / 3)) * 3; // spin multiples of 3  
	if (si % 2 == 0) si = (((si / 2) * p2 + r2) % Math.floor((nlim + 1) / 2)) * 2;   // spin multiples of 2 
	if (si < rx) si = ((rx - si - 1) * p2 + (r3 + r1)) % rx;       // mix random half one way
	else         si = ((si - rx) * p1 + (r3)) % (nlim - rx) + rx;  // and the other another
	if ((si ^ rx2) < nlim) si ^= rx2;
	si = ((nlim - si) * p1 + r4) % nlim;  // extra large item piles need extra mixing

	return(si);  // return 'shuffled' index

 return(0);
}


//===========================================================================================================
// Other Algorithms of possible interest


// --------------------------------------------------------
// DOUBLE DECK Shuffle
// generates shuffle indexes, in a similar fashion as dealing from a Double Deck where you get 1 of 52
// unique cards yet can get the same card a second time, but not three or more of any one.
// Dealing out 1/2 the 'listSize' you get ~1/2 as many repeats 1/2 as often, compared to using random()
// This utilizes the Miller Shuffle Algo; it will provide 2 of each inx (<listSize) in 2*listSize calls,
// with a great, yet moderated, random behavior. 
// Preferred for doling out some exercise or test items (of eg: Morse code).
// When using it for Black-Jack similar can be done to emulate the use of 4 or 6 decks.
var shuffleID = 314159; // global

function DDeck_Shuffle(inx, listSize) {
  if (inx==0) shuffleID=Math.floor(Math.random() * 1000000000); // reshuffle  
  var si=MillerShuffle_lite(inx, shuffleID, 2*listSize);
  return (Math.floor(si/2));  // return a single desk 'Shuffled' index
}


// ------------------------------------
function algoChkSum(algo, randCut, nlimit) {  // does a Simple Shifting Check Sum (both value and sequence dependant)
  var i, item, csum, lim;
  var sh, shufs;
  
  shufs = 512 / nlimit;
  if (shufs < 2) shufs = 2;
  if (nlimit > 2000000000) shufs = 1;
  
  csum=sh=0;
  for (lim=nlimit; lim<(nlimit+3); lim++) {
	  for (i=0; i<(shufs*lim); i++)  // i goes > lim, in order to exercise input inx overflow support feature
	  {
		if (algo==5) item = MillerShuffle(i, randCut, lim);		// currently = MSA_e
		else if (algo==6) item = MillerShuffle_lite(i, randCut, lim); 
		else if (algo==7) item = MillerShuffle_Large(i, randCut, lim); 
	
		csum = Number(BigInt.asUintN(32, BigInt(csum + (item<<sh))));  // limit to 32 bits as a std
		if (++sh==8) sh=0;
	  }
	  if (nlimit>256 || shufs==1) break;
  }
  
  // Note: Shuffles are not checked for correct means, sums of the squares or range.
  // Only requiring that the chksum results agree with those from the 'c' version where those tests were done.
  return (csum);
}
