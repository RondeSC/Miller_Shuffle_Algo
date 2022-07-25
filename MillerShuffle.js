// ================================================================
// the Miller Shuffle Algorithm
//
// source: https://github.com/RondeSC/Miller_Shuffle_Algo
// license: Attribution-NonCommercial-ShareAlike
// Copyright 2022 Ronald R. Miller
// http://www.apache.org/licenses/LICENSE-2.0


// --------------------------------------------------------
// Miller Shuffle Algo-A, produces a shuffled Index given a base Index, a shuffle ID value and the length of the list being indexed.
// For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order, utilizing minimum resources.
// As such this Miller Shuffle algorithm is the best choice for a playlist shuffle.
function MillerShuffle(inx, shuffleID, listSize) {
  var si,r1,r2;
  var p=16183;
  var p2=6197; 
  var maxBin, halfBin,xorFlip;
  var evenTop;
  var randR;     //local randomizer

  randR = shuffleID;
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;
  halfBin=Math.floor(maxBin/2);         // limit field effected for great variation
  xorFlip=0x5555 & halfBin;
  
  //si = (inx%listSize);            // allow an over zealous inx
  randR+=Math.floor(inx/listSize);  // & have it effect the mix
  r1=randR%1009;
  r2=randR%listSize;

  si = inx;
  si = (si*p + r1) % listSize;        // spin prime gears

  if (si<=maxBin) {
    if (listSize>=128) si = (si & 0xFF99) | ((si&0x60)>>4) | ((si&0x06)<<4);  // swap some bits
    if (si<=halfBin) si=si ^ xorFlip;                                         // flip some bits  
  }
  evenTop = listSize - (listSize & 1);
  if (si&1) si=evenTop-si;            // reverse flow of odd #s  

  si = (si*p2 + r2) % listSize;       // more prime gears spinning

  return(si);              // return 'Shuffled' index
}

var opti=0; // global
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
// DOUBLE DECK Shuffle
// generates shuffle indexes, in a similar fashion as dealing from a Double Deck where you get 1 of 52 cards and
// can get the same card a second time, but not three or more.
// Dealing out 1/2 the 'listSize' you get ~1/2 as many repeats 1/2 as often, compared to using random()
// This utilizes the Miller Shuffle Algo; it will provide 2 of each inx (<listSize) in 2*listSize calls,
// with a great, yet moderated, random behavior. 
// Preferred for doling out some exercise or test items (of eg: Morse code).
var shuffleID = 314159; // global
function DDeck_Shuffle(inx, listSize) {
  if (inx==0) suffleID=Math.floor(Math.random() * 1000000); // reshuffle  
  var si=MillerShuffle(inx, shuffleID, 2*listSize);
  return (Math.floor(si/2));  // return a single desk 'Shuffled' index
}
