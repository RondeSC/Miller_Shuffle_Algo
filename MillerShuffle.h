// Miller Shuffle Algorithm proto types
//unsigned int ShuffleAlgo_b(unsigned int, unsigned int, unsigned int);
unsigned int MillerShuffleAlgo_d(unsigned int, unsigned int, unsigned int);
unsigned int MillerShuffleAlgo_e(unsigned int, unsigned int, unsigned int);
short int MillerShuffle_lite (short, unsigned long, short);
short int MillerShuffle_xlite(short, unsigned long, short);

unsigned int DDeck_Shuffle      (unsigned int, unsigned int, unsigned int);

// perhaps better descriptive Names, given the application
#define MillerShuffle MillerShuffleAlgo_e
#define PRIG MillerShuffleAlgo_e
