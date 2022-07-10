// ================================================================
// the Miller Shuffle Algorithm
//
// license: Attribution-NonCommercial-ShareAlike
// Copyright 2022 Ronald R. Miller
// http://www.apache.org/licenses/LICENSE-2.0

// --------------------------------------------------------
// the Miller Shuffle Algorithm (aka: MillerShuffleAlgo_a )
// produces a shuffled Index given a base Index, a random seed and the length of the list being
// indexed. For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
unsigned int MillerShuffleAlgo(unsigned int inx, unsigned int RandizeR, unsigned int listSize) {
  unsigned int p=16183;   // arbitrary prime #s  must be > listSize
  unsigned int p2=6197;   // p~=2.618p2 (not critical) 
  unsigned int si, r1,r2;
  unsigned int maxBin, halfBin, xorFlip;
  unsigned int evenTop;
 
  // compute reference values for later
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;
  halfBin=maxBin/2; // limit field effected for great variation
  xorFlip=0x5555 & halfBin;  // choose 5555 empirically
  evenTop = listSize - (listSize & 1);
  
  //si = (inx%listSize);    // allow an over zealous inx
  RandizeR+=(inx/listSize); // & have it effect the mix
  r1=RandizeR%1009;
  r2=RandizeR%listSize;

  si = inx;
  /**** Heart of the Algorithm  *****/
  si = ((long)si*p + r1) % listSize;   // relatively prime gears turning operation
  if (si<=maxBin) {
    if (listSize>=128) si = (si & 0xFF99) | ((si&0x60)>>4) | ((si&0x06)<<4); // swap some bits
    if (si<=halfBin) si=si ^ xorFlip;   // flip some bits operation  
  }
  if (si&1) si=evenTop-si;              // reverse flow of odd #s  
  si = ((long)si*p2 + r2) % listSize;   // more prime gear turning

  return(si);  // return 'Shuffled' index
}

// --------------------------------------------------------
// Produce a shuffled Index given a base Index, a random seed and the length of the list being
// indexed. For each inx: 0 to listSize-1, unique indexes are returned in a pseudo "random" order.
// With this algo there is not a 1:1 between inx IN and OUT
// better at randomixing pattern occurrences, as seen in cardDeal_Test()
unsigned int MillerShuffleAlgo_b(unsigned int inx, unsigned int RandizeR, unsigned int listSize) {
  unsigned int xi,si;
  unsigned int p=16183;  // arbitrary prime #s  must be > listSize
  unsigned int p2=6197;  // ~= p*0.618
  unsigned int r1,r2;
  unsigned int xorFlip, maxBin, halfBin;
  unsigned int topEven;
  static int opti=-1; // opt inx for swapping
  int seed=0;

  // compute reference values for later
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;
  halfBin=maxBin/2; // limit field effected for great variation
  xorFlip=0x5555 & halfBin;  // choose 5555 empirically
  topEven = listSize - (listSize & 1);

  xi = (inx%listSize);      // allow an over zealous inx
  RandizeR+=(inx/listSize);  // & have it effect the mix

  r1=RandizeR%1009;
  r2=RandizeR%listSize;
  
  if (opti==-1 || inx==0) {seed=1;}
  do {
      si = (seed)? listSize-1 : xi;
    
      si = ((long)si*p + r1) % listSize;   // relatively prime gears turning operation
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


//===========================================================================================================

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
unsigned int DDeck_Shuffle(unsigned int inx, unsigned int RandizeR, unsigned int listSize) {
  unsigned int si, r1, r2;
  unsigned int p1=16183;  // arbitrary prime #s  must be > listSize
  unsigned int p2=6197;  // ~= p*0.618*0.618
  unsigned int maxBin, topEven;

  listSize=listSize*2; // Double the processing range
  maxBin=1;
  while ((2*maxBin+1)<listSize) maxBin=2*maxBin+1;
  topEven = listSize - (listSize & 1);

  si = (inx%listSize);       // allow an over zealous inx
  RandizeR+=(inx/listSize);  // & have it effect the mix
  r1=RandizeR%1009;
  r2=RandizeR%listSize;
  
  si = ((long)si*p1 + r1) % listSize;  // relatively prime gears turning operation
  if (si&1) si=topEven-si;                   // reverse flow of odd #s
  if (si<=maxBin) si=si ^ (0xAAAA & maxBin); // Xor flip some bits operation  
  si = ((long)si*p2 + r2) % listSize;  // spin more prime gears

  return(si/2);  // return a single desk 'Shuffled' index
}
// of course, given MillerShuffleAlgo(), this stand alone function could have been implemented in-line like this:
// 
//      item = MillerShuffleAlgo(inx, RandizeR, 2*listSize)/2;
//