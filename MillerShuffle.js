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


// --------------------------------------------------------
// Miller Shuffle Algo-A, produces a shuffled Index given a base Index, a shuffle ID value and the length of the list being indexed.
// For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order, utilizing minimum resources.
// As such this Miller Shuffle algorithm is the better choice for a playlist shuffle.
//
// The 'shuffleID' might be in a range of 0-999999 and be set by utilizing a PRNG.
// Each time you want another pseudo random index from a current shuffle (incrementing 'inx')
// you must be sure to pass in the "shuffleID" for that shuffle.
// Note that you can exceed the listSize with the input 'inx' value and get very good results,
// as the code effectively uses a secondary shuffle by way of using a 'working' modified value of the input shuffle ID.

function MillerShuffleAlgo_a(inx, shuffleID, listSize) {
  var si,r1,r2;  // randomizing factors, in combination provide ~million different shuffles
  var p1=16183;
  var p2=6197; 
  var maxBin, halfBin,xorFlip;
  var topEven;
  var randR;     //local randomizer copy

  randR = shuffleID;
  // compute reference values for later
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;
  halfBin=Math.floor(maxBin/2);         // limit field effected for great variation
  xorFlip=0x5555 & halfBin;
  topEven = listSize - (listSize & 1);
  
                                    // allow an over zealous inx
  randR+=Math.floor(inx/listSize);  // & have it effect the mix
  r1=randR%1009;  // constant value is not super important
  r2=((randR%1019)*p2)%listSize; // consecutive shuffleIDs now make more varied shuffles
  si=(inx+shuffleID)%listSize;

  /**** Heart of the Algorithm  *****/
  si = (si*p1 + r1) % listSize;        // spin prime gears
  if (si<=maxBin) {
    if (listSize>=128) si = (si & 0xFF99) | ((si&0x60)>>4) | ((si&0x06)<<4);  // swap some bits
    if (si<=halfBin) si=si ^ xorFlip;                                         // flip some bits  
  }
  if (si&1) si=topEven-si;            // reverse flow of odd #s  
  si = (si*p2 + r2) % listSize;       // more prime gears spinning

  return(si);              // return 'Shuffled' index
}

var opti=0; // global, OPTional return Index
// --------------------------------------------------------
// Miller Shuffle Algo-B, produces a shuffled Index given a base Index, a shuffle ID value and the length of the list being indexed.
// For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order, with very good sequence distribution over time.
// Preferred for Shuffles used for dealing to competing players.
function MillerShuffleAlgo_b(inx, shuffleID, listSize) {
  var xi,si;
  if ((inx%listSize)==0) opti=MillerShuffle(listSize-1, shuffleID, listSize);
 
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
// Miller Shuffle Algorithm C variant   NEW Aug 2022
// Combines virturally all the good qualities of both MSA_a and MSA_b (per extensive testing),
// for randomness as well as desired even handed shuffle distribution characteristics.
// 
// Produces a shuffled Index given a base Index, a random seed and the length of the list being
// indexed. For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
//       MillerShuffleAlgo_c
function MillerShuffle(inx, shuffleID, listSize) {
  var si,r1,r2;
  var p1=3251;
  var p2=5261; 
  var maxBin, sh;
  var randR;     //local copy

  randR = shuffleID;
  // compute reference values for later
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;
  
                                    // allow an over zealous inx
  randR+=Math.floor(inx/listSize);  // & have it effect the mix
  r1=shuffleID%1009;   // constant values are not super important
  r2=((shuffleID%1019)*p2)%listSize; // consecutive shuffleIDs now make more varied shuffles
  si=(inx+shuffleID)%listSize;

  /**** Heart of the Algorithm  *****/
  si = (si*p1 + r1) % listSize;  // relatively prime gears turning operation
  if (si<=maxBin && si!=0) {  // This one middle action does the added mix up.
    // ---- now for the secret sauce of MSA_c ...
    for (sh=0; (maxBin>>sh)>=si; sh++) ;  // set to do smart masked XOR operation
    si=si^((0x5555&maxBin)>>sh);      // note: operator order is important
  }
  if (si%3==0) si=(((si/3)*p1+r1) % ((listSize+2)/3)) *3; // spin multiples of 3 
  si = (si*p2 + r2) % listSize; // turn more prime wheels

  return(si);              // return 'Shuffled' index
}
// Note: for MillerShuffleAlgo_c I found that by adding another 1/3 spin 
// or swithing to a bit rotation operation over the smart XOR
// could improve the general byte ChiSq but worsened the album shuffle distribution ChiSq.


// --------------------------------------------------------
// Miller Shuffle lite, 
// produces a shuffled Index given a base Index, a random seed and the length of the list being indexed
// for each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
// 
// This variation of the Miller Shuffle algorithm is for when you need/want minimal coding and processing, 
// to acheive good randomness along with desirable shuffle characteristics. (used by DDesk_Shuffle)
// Generally for a shuffle this works really well; unlike using rand() which does not.
function MillerShuffle_lite(inx, shuffleID, listSize) {
  var si, r1, r2;
  var p1=3251;  // prime #s  must be > listSize
  var p2=5261, p3=4493;  // p2 ~= p1*1.618
  var topEven; 

  topEven = listSize - (listSize & 1); // compute reference value  
  shuffleID+=Math.floor(inx/listSize);  // & have it effect the mix
  r1=shuffleID%1009;   // constant values are not super important
  r2=shuffleID%1019;
  si=(inx+shuffleID)%listSize;

  si = (si*p1 + r1) % listSize;  // relatively prime gears turning operation
  if (si%3==0) si=((Math.floor(si/3)*p3+r1) % Math.floor((listSize+2)/3)) *3; // spin multiples of 3 
  if (si&1)    si=topEven-si;    // reverse flow of odd #s
  si = (si*p2 + r2) % listSize;  // turn more prime wheels
  
  return(si);  // return 'Shuffled' index
}


// --------------------------------------------------------
// DOUBLE DECK Shuffle
// generates shuffle indexes, in a similar fashion as dealing from a Double Deck where you get 1 of 52 cards and
// can get the same card a second time, but not three or more.
// Dealing out 1/2 the 'listSize' you get ~1/2 as many repeats 1/2 as often, compared to using random()
// This utilizes the Miller Shuffle Algo; it will provide 2 of each inx (<listSize) in 2*listSize calls,
// with a great, yet moderated, random behavior. 
// Preferred for doling out some exercise or test items (of eg: Morse code).
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
  var i, item, csum;
  var sh;
  
  //srand(314159); // set for repeatabilty on use of rand()
  randCut = 314159;   //
  csum=sh=0;
  for (i=0; i<nlimit; i++) 
  {
    if (algo==1)      item = MillerShuffleAlgo_a(i, randCut, nlimit); 
    else if (algo==2) item = MillerShuffleAlgo_b(i, randCut, nlimit); 
    else if (algo==3) item = MillerShuffle(i, randCut, nlimit); 
    else if (algo==4) item = MillerShuffle_lite(i, randCut, nlimit); 
    csum += (item<<sh);
    if (++sh==8) sh=0;
  }
  
  //printf("   Algorithm %d chkSum: %d\n",algo,csum);
  return (csum);
}
