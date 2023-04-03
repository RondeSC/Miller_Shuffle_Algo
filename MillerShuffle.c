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

// --------------------------------------------------------------
// the Miller Shuffle Algorithm (latest variant aka: MillerShuffleAlgo_d)
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
// Miller Shuffle Algorithm D variant     NEW April 2023
//    aka:   MillerShuffleAlgo_d
unsigned int MillerShuffle(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int si, r1, r2, r3, r4;
  const unsigned int p1=24317;
  const unsigned int p2=32141;
  const unsigned int p3=63629;  // for shuffling 60,000+ indexes (only needs 32bit unsigned math)

  shuffleID+=131*(inx/listSize);  // have inx overflow effect the mix
  si=(inx+shuffleID)%listSize;

  r1=shuffleID%p1;   // shuffle rx fixed values are not super important
  r2=(r1+shuffleID)%p2;
  r3=(r1+r2+p3)%listSize;
  r4=r1^r2^r3;

  // perform the conditional multi-faceted mathematical spin-mixing
  if (si%3==0) si=(((unsigned long)(si/3)*p1+r1) % ((listSize+2)/3)) *3; // spin multiples of 3 
  if (si%2==0) si=(((unsigned long)(si/2)*p2+r2) % ((listSize+1)/2)) *2; // spin multiples of 2 
  if (si<listSize/2) si=(si*p3+r4) % (listSize/2);
  si = ((unsigned long)si*p3 + r3) % listSize;  // relatively prime gears turning operation
  
  return(si);  // return 'Shuffled' index
}


/***************    previous variants (quite usable but generally not preferred)
unsigned int MillerShuffleAlgo_a(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int p1=16183;   // arbitrary prime #s  must be > listSize
  unsigned int p2=6197;   // p1~=2.618*p2 (not critical) 
  unsigned int si, r1,r2; // randomizing factors, in combination provide ~million different shuffles
  unsigned int maxBin, halfBin, xorFlip;
  unsigned int topEven;

  // compute reference values for later
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;
  halfBin=maxBin/2; // limit field effected for greater variation
  xorFlip=0x5555 & halfBin;  // choose 5555 empirically
  topEven = listSize - (listSize & 1);
  
                             // allow an over zealous inx
  shuffleID+=(inx/listSize); // & have it effect the mix
  r1=shuffleID%1009;  // constant exact value is not super important
  r2=((shuffleID%1019)*p2)%listSize; // consecutive shuffleIDs now make more varied shuffles
  si=(inx+shuffleID)%listSize;
  
  // **** Heart of the Algorithm  *****
  si = ((long)si*p1 + r1) % listSize;   // relatively prime gears turning operation
  if (si<=maxBin) {
    if (listSize>=128) si = (si & 0xFF99) | ((si&0x60)>>4) | ((si&0x06)<<4); // swap some bits
    if (si<=halfBin) si=si ^ xorFlip;   // flip some bits operation  
  }
  if (si&1) si=topEven-si;              // reverse flow of odd #s  
  si = ((long)si*p2 + r2) % listSize;   // more prime gear turning

  return(si);  // return 'Shuffled' index
}

// --------------------------------------------------------
// Algorithm B
// Produces a shuffled Index given a base Index, a shuffle identifier and the length of the list being
// indexed. For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
// With this algo there is not a 1:1 between inx IN and OUT
// Better at randomixing pattern occurrences, providing very good sequence distribution over time.
// Preferred for Shuffles used for dealing to competing players.
unsigned int MillerShuffleAlgo_b(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int xi,si;
  unsigned int p1=16183;  // arbitrary prime #s  must be > listSize
  unsigned int p2=6197;  // ~= p1*0.618
  unsigned int r1,r2;    // randomizers
  unsigned int xorFlip, maxBin, halfBin;
  unsigned int topEven;
  static unsigned int opti=-1; // opt inx for swapping
  int seed=0;

  // compute reference values for later
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;
  halfBin=maxBin/2; // limit field effected for greater variation
  xorFlip=0x5555 & halfBin;  // choose 5555 empirically
  topEven = listSize - (listSize & 1);

  xi = (inx%listSize);      // allow an over zealous inx
  shuffleID+=(inx/listSize);  // & have it effect the mix

  r1=shuffleID%1009;
  r2=((long)(shuffleID%listSize)*p2)%listSize; // consecutive shuffleIDs now make more varied shuffles
  
  if (opti==-1 || inx==0) {seed=1;}
  do {
      si = (seed)? listSize-1 : xi;
    
      si = ((long)si*p1 + r1) % listSize;   // relatively prime gears turning operation
      if (si<=halfBin) si=si ^ xorFlip;    // flip some bits operation
      if (si&1) si=topEven-si;             // reverse flow of odd #s
      si = ((long)si*p2 + r2) % listSize;  // more prime gear turning
       
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
***/

// --------------------------------------------------------
// Miller Shuffle Algorithm C variant     NEW Aug 2022
// Combines virturally all the good qualities of both MSA_a and MSA_b (per extensive testing),
// for randomness as well as desired even handed shuffle distribution characteristics.
// 
// Produces a shuffled Index given a base Index, a random seed and the length of the list being
// indexed. For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
//    aka:   MillerShuffleAlgo_c
unsigned int MillerShuffleAlgo_c(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int si, r1, r2;
  unsigned int p1=50021;  // for shuffling upto 50,000 indexes (only needs 32bit unsigned math)
  unsigned int p2=63629;  // ~= p1 * SQR(1.618)
  unsigned int maxBin;
  int sh;
  
  // compute reference values for later
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;

                              // allow an over zealous inx
  shuffleID+=131*(inx/listSize);  // & have it effect the mix
  r1=shuffleID%1009;   // constant values are not super important
  r2=shuffleID%1019;
  si=(inx+shuffleID)%listSize;

  //**** Heart of the Algorithm  *****
  si = ((unsigned long)si*p1 + r1) % listSize;  // relatively prime gears turning operation
  if (si<=maxBin && si!=0) {  // This one middle action does the added mix up.
    // --- now for the secret sauce of MSA_c ...
    for (sh=0; (maxBin>>sh)>=si; sh++) ;  // set to do smart masked XOR operation
    si=si^((0x5555&maxBin)>>sh);      // note: operator order is important
  }
  if (si%3==0) si=(((unsigned long)(si/3)*p1+r1) % ((listSize+2)/3)) *3; // spin multiples of 3 
  si = ((unsigned long)si*p2 + r2) % listSize; // turn more prime wheels

  return(si);  // return 'Shuffled' index
}
// Note: for MillerShuffleAlgo_c I found that by adding another 1/3 spin 
// or swithing to a bit rotation operation over the smart XOR
// could improve the general byte ChiSq but worsened the album shuffle distribution ChiSq.


//===========================================================================================================
// Other Algorithms of possible interest


// --------------------------------------------------------
// Miller Shuffle lite, 
// produces a shuffled Index given a base Index, a random seed and the length of the list being indexed
// for each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
// 
// This variation of the Miller Shuffle algorithm is for when you need/want minimal coding and processing, 
// to acheive good randomness along with desirable shuffle characteristics. (eg: in an 8-bit MCU project)
// For a simple shuffle this works really well; unlike using rand() which does not. (used by DDesk_Shuffle)
unsigned int MillerShuffle_lite(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int si, r1, r2, topEven;

  topEven = listSize - (listSize & 1); // compute reference value  

  shuffleID+=131*(inx/listSize);  // have inx overflow effect the mix
  r1=shuffleID%1009;   // constant values are not super important
  r2=shuffleID%1019;
  si=(inx+shuffleID)%listSize;

  si = ((long)si*3251 + r1) % listSize;  // relatively prime gears turning operation
  // note: the next line operates only 1/3 the time, the following line 1/2
  if (si%3==0) si=(((long)(si/3)*4493+r1) % ((listSize+2)/3)) *3; // spin multiples of 3 
  if (si&1)    si=topEven-si;          // reverse flow of odd #s
  si = ((long)si*5261 + r2) % listSize;  // turn more prime wheels
  
  return(si);  // return 'Shuffled' index
}

// new Algo: taking the MillerShuffle to the MAX, doing whatever improved random permutations (even a little)
//                      ************************
// note performance statistics are very = to Fisher-Yates  or  when double calling MillerShuffleAlgo_d ie:
//      si = MSA_d( MSA_d(i,sid,lim), sid+p, lim);   // where p is a prime #

unsigned int MillerShuffle_Max(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int si, xi, r1, r2, r3, r4;
  unsigned int maxBin, topEven;
  int s, sh;
  const unsigned int p1=39323;
  const unsigned int p2=50021;
  const unsigned int p3=63629;
  int a,b,c;

  // compute reference values for later
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;
  topEven = listSize - (listSize & 1); // compute reference value
  // set a,b&c to 1,2 or 3 exclusively, based on the shuffle randomizer (aka: shuffleID)
  a=shuffleID%3;
  b=(a+(shuffleID%2+1))%3;
  a++; b++; c=6-a-b;

  shuffleID+=131*(inx/listSize);  // have inx overflow effect the mix
  si=(inx+shuffleID)%listSize;
  
  r1=shuffleID%p1;   // fixed shuffle values are not super important
  r2 = (r1+shuffleID)%p2;
  r3=(r1+r2+p3)%listSize;
  r4=r1^r2^r3;

  if (si&1)  si=topEven-si;   // reverse flow of odd #s

  if (si%a==0) si=(((unsigned long)(si/a)*p1+r3) % ((listSize+a-1)/a)) *a;	// spin multiples of 1,2 or 3 
  if (si<=maxBin && si!=0) {	// In this case use bit rotation to mix em' up.
    if (si<=maxBin) {
      xi=si; si=0;
	  for (s=0; (maxBin>>s)>0; s++)  {si=(si<<1)+(xi&1);  xi=xi>>1;}
    }
  }
  if (si<listSize/2) si=(si*40037+r4) % (listSize/2);

  if (si%b==0) si=(((unsigned long)(si/b)*p2+r1) % ((listSize+b-1)/b)) *b;	 // second spin
  if (si<=maxBin && si!=0) {  // This section does smart masked XOR operations
	for (sh=0; (maxBin>>sh)>=si; sh++) ;
	si=si^((0x5555&maxBin)>>sh);      // note: operator order is important
  }
  if (si<=maxBin) si=si^(r4&maxBin);  // limited masked XOR operation

  if (si%c==0) si=(((unsigned long)(si/c)*p3+r2) % ((listSize+c-1)/c)) *c;   // third spin

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
unsigned int DDeck_Shuffle(unsigned int inx, unsigned int shuffleID, unsigned int listSize) {
  unsigned int si, r1, r2;
  unsigned int p1=3251;  // prime #s  must be > listSize
  unsigned int p2=5261;  // ~= p1 * 1.618
  unsigned int topEven;

  listSize=listSize*2; // Double the processing range
  
  topEven = listSize - (listSize & 1); // compute reference value  
  shuffleID+=131*(inx/listSize);  // have inx overflow effect the mix
  r1=shuffleID%1009;   // constant values are not super important
  r2=shuffleID%1019;
  si=(inx+shuffleID)%listSize;

  si = ((long)si*p1 + r1) % listSize;  // relatively prime gears turning operation
  if (si%3==0) si=(((long)(si/3)*p1+r1) % ((listSize+2)/3)) *3; // spin multiples of 3 
  if (si&1)    si=topEven-si;          // reverse flow of odd #s
  si = ((long)si*p2 + r2) % listSize;  // turn more prime wheels

  return(si/2);  // return a single desk 'Shuffled' index
}
// of course, given any MillerShuffle(), this stand alone function could have been implemented in-line like this:
// 
//      item = MillerShuffle(inx, RandizeR, 2*listSize)/2;
//
