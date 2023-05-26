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
//


// --------------------------------------------------------
// Miller Shuffle, produces a shuffled Index given a base Index, a shuffle ID value and the length of the list being indexed.
// For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order, utilizing minimum resources.
// As such this Miller Shuffle algorithm is the better choice for a playlist shuffle.
//
// The 'shuffleID' is a 32bit value and be set by utilizing a PRNG. These bit determin the "random" shuffle.
// Each time you want another pseudo random index from a current shuffle (incrementing 'inx')
// you must be sure to pass in the "shuffleID" for that shuffle.
// Note that you can exceed the listSize with the input 'inx' value and get very good results,
// as the code effectively uses a secondary shuffle by way of using a 'working' modified value of the input shuffle ID.

// --------------------------------------------------------------
// Miller Shuffle Algorithm D variant      April 2023
//    aka:   MillerShuffleAlgo_d
function MillerShuffle(inx, shuffleID, listSize) {
  var si, r1, r2, r3, r4;
  const p1=24317;
  const p2=32141;
  const p3=63629;  // for shuffling 60,000+ indexes (only needs 32bit unsigned math)
  var randR;     //local randomizer copy

  randR=shuffleID+131*Math.floor(inx/listSize);  //  have inx overflow effect the mix
  si=(inx+randR)%listSize;

  r1=randR%p1;   // shuffle rx fixed values are not super important
  r2=((randR*0x89)^r1)%p2;
  r3=(r1+r2+p3)%listSize;
  r4=r1^r2^r3;

  // perform the conditional multi-faceted mathematical spin-mixing
  if (si%3==0) si=(((si/3)*p1+r1) % Math.floor((listSize+2)/3)) *3; // spin multiples of 3 
  if (si%2==0) si=(((si/2)*p2+r2) % Math.floor((listSize+1)/2)) *2; // spin multiples of 2 
  if (si<Math.floor(listSize/2)) si=(si*p3+r4) % Math.floor(listSize/2);
  si = (si*p3 + r3) % listSize;  // relatively prime gears turning operation
  
  return(si);  // return 'Shuffled' index
}

// --------------------------------------------------------------
// Miller Shuffle Algorithm E variant     May 2023
// Produces nearly the same randomness within the shuffles as MSA-d,
// with changes and added code to increase the potential number of shuffle permutations generated. 
// 
function MillerShuffleAlgo_e(inx, shuffleID, listSize) {
  var si, r1, r2, r3, r4;
  const p1=24317;
  const p2=32141;
  const p3=63629;  // for shuffling 60,000+ indexes (only needs 32bit unsigned math)
  var halfN, randR;  //local randomizer copy

  randR=shuffleID+131*Math.floor(inx/listSize);  //  have inx overflow effect the mix
  si=(inx+randR)%listSize;    // cut the deck

  // compute fixed randomizing values once for a given shuffle
  r1 = randR % 0x5FFF;
  r2 = (randR+r1) % p1;
  r3 = randR % p2;
  r4 = ((randR * 0x89) ^ r3) % p3;
  halfN = Math.floor(listSize/2)+1;

  // perform the conditional multi-faceted mathematical spin-mixing (on avg 2 5/6 shuffle ops done)
  if (si%3==0)      si = (((si/3)*p1+r1) % Math.floor((listSize+2)/3)) *3; // spin multiples of 3 
  if (si <= halfN) {si = (si + r3) % (halfN + 1); si = halfN - si;}  // improves large permu distro
  if (si%2==0)      si = (((si/2)*p2+r2) % Math.floor((listSize+1)/2)) *2; // spin multiples of 2 
  if (si < halfN)   si = (si * p3 + r3) % halfN;
  si = (si*p3 + r4) % listSize;  // relatively prime gears churning operation
  
  return(si);  // return 'Shuffled' index
}


//===========================================================================================================
// Other Algorithms of possible interest

var opti=0; // global, OPTional return Index
// --------------------------------------------------------
// Algorithm B  (a non standard variation)
// With this algo there is not a 1:1 between inx IN and OUT repeatability due to the use of random().
// Better at randomixing pattern occurrences, providing very good sequence distribution over time.
// As coded here -b internally depends on a standard version of an MS Algo.
// Preferred for Shuffles used for dealing to Serious! competing players.
function MillerShuffleAlgo_b(inx, shuffleID, listSize) {
  var xi,si;
  if ((inx%listSize)==0) opti=MillerShuffle(inx+listSize-1, shuffleID, listSize);
 
  si=MillerShuffle(inx, shuffleID, listSize);

  if ((inx%listSize)==(listSize-1)) {
    si=opti;
  } else if (Math.floor(Math.random() * 3)==1) {
    xi=si;
    si=opti % listSize;  // swap it & be sure # is in current range
    opti=xi;
  }
  return(si);
}

// --------------------------------------------------------
// Miller Shuffle lite, 
// produces a shuffled Index given a base Index, a random seed and the length of the list being indexed
// for each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
// 
// This variation of the Miller Shuffle algorithm is for when you need/want minimal coding and processing, 
// to acheive good randomness along with desirable shuffle characteristics. (eg: in an 8-bit MCU project)
// For a simple shuffle this works really well; unlike using rand() which does not. (used by DDesk_Shuffle)
function MillerShuffle_lite(inx, shuffleID, listSize) {
  var si, r1, r2;
  var p1=3343;
  var p2=9973;  // this prime must be > listSize used
  var topEven; 

  shuffleID+=131*Math.floor(inx/listSize);  // have inx overflow effect the mix
  si=(inx+shuffleID)%listSize;   // cut the deck

  topEven = listSize - (listSize & 1); // compute reference value  
  r1 = shuffleID % 0xFFF;		// improvemed randomizing values
  r2 = shuffleID % 0x3FFF ^ r1; 

  // note: the next line operates only 1/3 the time, the following line 1/2
  if (si%3==0) si=((Math.floor(si/3)*p1+r1) % Math.floor((listSize+2)/3)) *3; // spin multiples of 3 
  if (si&1)    si=topEven-si;    // reverse flow of odd #s
  si = (si*p2 + r2) % listSize;  // turn more prime wheels
  
  return(si);  // return 'Shuffled' index
}


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
  if (inx==0) shuffleID=Math.floor(Math.random() * 1000000); // reshuffle  
  var si=MillerShuffle_lite(inx, shuffleID, 2*listSize);
  return (Math.floor(si/2));  // return a single desk 'Shuffled' index
}


// ------------------------------------
function algoChkSum(algo) {  // does a Simple Shifting Check Sum (both value and sequence dependant)
  var nlimit=256;
  var randCut;
  var i, item, csum, lim;
  var sh;
  
  //srand(314159); // set for repeatabilty on use of rand()
  randCut = 314159;   //
  csum=sh=0;
  for (lim=nlimit; lim<(nlimit+3); lim++) {
	  for (i=0; i<(2*lim); i++) 
	  {
		//if (algo==1)      item = MillerShuffleAlgo_a(i, randCut, lim); 
		if (algo==2) item = MillerShuffleAlgo_b(i, randCut, lim); 
		//else if (algo==3) item = MillerShuffleAlgo_c(i, randCut, lim); 
		else if (algo==4) item = MillerShuffle(i, randCut, lim);		// currently = MSA_d
		else if (algo==5) item = MillerShuffleAlgo_e(i, randCut, lim);
		else if (algo==6) item = MillerShuffle_lite(i, randCut, lim); 
		csum += (item<<sh);
		if (++sh==8) sh=0;
	  }
  }
  
  //printf("   Algorithm %d chkSum: %d\n",algo,csum);
  return (csum);
}
