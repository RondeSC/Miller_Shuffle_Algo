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

// --------------------------------------------------------
// Miller Shuffle, produces a shuffled Index given a base Index, a shuffle ID value and the length of the list being indexed.
// For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
// As such this Miller Shuffle algorithm is the better choice for a playlist shuffle.
//
// The 'shuffleID' is a 32bit value and be set by utilizing a PRNG. These bit determin the "random" shuffle.
// Each time you want another pseudo random index from a current shuffle (incrementing 'inx')
// you must be sure to pass in the "shuffleID" for that shuffle.
// Note that you can exceed the listSize with the input 'inx' value and get very good results,
// as the code effectively uses a secondary shuffle by way of using a 'working' modified value of the input shuffle ID.


// --------------------------------------------------------------
// Miller Shuffle Algorithm E variant
// Produces nearly the same randomness within the shuffles as did MSA-d,
// with changes and added code to significantly increase the potential number of shuffle permutations generated.
//
//    aka:   MillerShuffleAlgo_e
function MillerShuffle(inx, shuffleID, listSize) {
  var si, r1, r2, r3, r4;
  const p1=24317;
  const p2=32141;
  const p3=63629;  // for shuffling 60,000+ indexes (only needs 32bit unsigned math)
  var halfN, randR;  //local randomizer copy

  randR = Number(BigInt.asUintN(32, BigInt(shuffleID) ^ BigInt(Math.floor(inx/listSize))));  //  have inx overflow effect the mix
  si = (randR % listSize + inx) % listSize;    // cut the deck

  r1 = randR % p3;
  r2 = randR % p1; // Now, per Chinese remainder theorem, (r1,r2,r3) will be a unique set
  r3 = randR % p2; 
  r4 = randR % 2749;
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


// ---------------------- MS Lite Aug 2025 lastest & best yet,   [Now default PRIG() function in my game software]
// Miller Shuffle lite, 
// produces a shuffled Index given a base Index, a random seed and the length of the list being indexed
// for each inx: 0 to nlim-1, unique indexes are returned in a pseudo "random" order.
// 
// This variation of the Miller Shuffle algorithm is for when you need/want to use minimal resources, 
// to acheive very good randomness along with desirable shuffle characteristics & many millions of permutations.
// For a any shuffle this works really well; unlike using rand() which does not.  (used by DDesk_Shuffle)
function MillerShuffle_lite(inx, mixID, nlim) {
  var si, r1, r2, rx, rx2;
  const p1 = 9949, p2 = 9973;  // nlim must be smaller than these primes
  var randR;

  randR = Number(BigInt.asUintN(32, BigInt(mixID) ^ (BigInt(13 * Math.floor(inx / nlim)))));   //  have inx overflow effect the mix
  si = (Math.floor(randR / 11) + inx) % nlim;   // cut the deck

  r1 = randR % p2;	// set of fixed randomizing values
  r2 = randR % p1;
  rx = Math.floor(randR / nlim) % nlim + 1;
  rx2 = Math.floor(randR / 131) % nlim + 1;

  // perform conditional multi-faceted mathematical spin-mixing  (average # of si recalcs performed is ~1.5)
  if (si % 2 == 0) si = (((si / 2) * p2 + r2) % Math.floor((nlim + 1) / 2)) * 2; // spin multiples of 2
  if ((si ^ rx2) < nlim) si = si ^ rx2;
  if (si < rx) si = ((rx - si - 1) * p2 + r1 + r2) % rx;       // mix random half one way
  else si = ((si - rx) * p1 + r2) % (nlim - rx) + rx;          // and the other another
  if (si % 3 == 0) si = (((si / 3) * p1 + r1) % Math.floor((nlim + 2) / 3)) * 3; // spin multiples of 3 

  return (si);  // return 'Shuffled' index
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
function algoChkSum(algo, randCut) {  // does a Simple Shifting Check Sum (both value and sequence dependant)
  var nlimit=256;
  var i, item, csum, lim;
  var sh;
  
  //srand(314159); // set for repeatabilty on use of rand()
  csum=sh=0;
  for (lim=nlimit; lim<(nlimit+3); lim++) {
	  for (i=0; i<(2*lim); i++)  // '2*' in order to exercise input inx overflow feature
	  {
		//if (algo==1)      item = MillerShuffleAlgo_a(i, randCut, lim); 
		//if (algo==2) item = MillerShuffleAlgo_b(i, randCut, lim); 
		//else if (algo==3) item = MillerShuffleAlgo_c(i, randCut, lim); 
		//else if (algo==4) item = MillerShuffleAlgo_d(i, randCut, lim);
		if (algo==5) item = MillerShuffle(i, randCut, lim);		// currently = MSA_e
		else if (algo==6) item = MillerShuffle_lite(i, randCut, lim); 
		csum += (item<<sh);
		if (++sh==8) sh=0;
	  }
  }
  
  //printf("   Algorithm %d chkSum: %d\n",algo,csum);
  return (csum);
}
