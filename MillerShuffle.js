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
// Miller Shuffle Algorithm D variant
//    aka:   MillerShuffleAlgo_d
function MillerShuffle(inx, shuffleID, listSize) {
  var si, r1, r2, r3, r4, rx, rx2;
  const p1=24317;
  const p2=32141;
  const p3=63629;  // for shuffling 60,000+ indexes (only needs 32bit unsigned math)
  var randR;     //local randomizer copy

  randR=shuffleID+131*Math.floor(inx/listSize);  //  have inx overflow effect the mix
  si=(inx+randR)%listSize;

  r1=randR%p1+42;	// randomizing factors crafted empirically (by automated trial and error)
  r2=((randR*0x89)^r1)%p2;
  r3=(r1+r2+p3)%listSize;
  r4=r1^r2^r3;
  rx = Math.floor(randR/listSize) % listSize + 1;
  rx2 = Math.floor(randR/listSize/listSize) % listSize + 1;

  // perform conditional multi-faceted mathematical spin-mixing (on avg 2 1/3 shuffle ops done + 2 simple Xors)
  if (si%3==0) si=(((si/3)*p1+r1) % Math.floor((listSize+2)/3)) *3; // spin multiples of 3 
  if (si%2==0) si=(((si/2)*p2+r2) % Math.floor((listSize+1)/2)) *2; // spin multiples of 2 
  if (si<Math.floor(listSize/2)) si=(si*p3+r4) % Math.floor(listSize/2);

  if ((si^rx) < listSize)   si=si^rx;			// flip some bits with Xor
  si = (si*p3 + r3) % listSize;  // relatively prime gears turning operation
  if ((si^rx2) < listSize)  si=si^rx2;
  
  return(si);  // return 'Shuffled' index
}

// --------------------------------------------------------------
// Miller Shuffle Algorithm E variant
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


//===========================================================================================================
// Other Algorithms of possible interest

var opti=0; // global, OPTional return Index
// --------------------------------------------------------
// Algorithm B  (a non standard variation)
// With this algo there is not a 1:1 between inx IN and OUT repeatability due to the use of random().
// Better at randomixing pattern occurrences, providing very good sequence distribution over time.
// As coded here -b internally depends on a standard version of an MS Algo.
// Preferred for Shuffles used for dealing to Serious! competing players.
// 
// Use of MSA-b is not generally advised due to:  Besides this algo not being repeatable 1:1 between inx IN & OUT;
// This algo won't work for concurrent or nested shuffles !
// Further MSA-b shuffles must be started with inx=0 and end with inx=listSize-1.
//
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
  var si, r1, r2, r3, r4, rx;
  var p1=3343, p2=5413;
  var p3=9973;  // this prime must be > listSize used
  var randR, topEven, halfN; 

  randR=shuffleID+131*Math.floor(inx/listSize);  // have inx overflow effect the mix
  si=(inx+randR)%listSize;   // cut the deck

  topEven = listSize - (listSize & 1); // compute reference value  
  halfN = Math.floor((listSize+1) / 2);
  r1 = randR % 0xFFF + 1;		// improvemed randomizing values
  r2 = randR % 0x2FFF + 2; 
  r3 = Math.floor(randR/881) + 3;
  r4 = Math.floor(randR/listSize) % listSize + 1;
  rx = Math.floor(randR/listSize/listSize) % listSize + 1;

  // perform conditional multi-faceted mathematical mixing (on avg 1 5/6 shuffle ops + a simple Xor op)
  if (si%3==0) si=(((si/3)*p1+r1) % Math.floor((listSize+2)/3)) *3; // spin multiples of 3 
  if (si&1)    si=(2*r2 + topEven - si) % topEven;			// reverse+rotate flow of odd #s
  if ((si^rx)<listSize)  si=si^rx;							// flip some bits with Xor
  // mix Differently 1/2 the range one way & the other another way
  if (si<halfN) si = (si*p2 + r3) % halfN + halfN-(listSize & 1);
  else          si = ((si-halfN) * p3 + r4) % (listSize-halfN);
  
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
	  for (i=0; i<(2*lim); i++)  // '2*' in order to exercise input inx overflow feature
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
