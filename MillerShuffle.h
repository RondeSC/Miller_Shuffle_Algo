
// Miller Shuffle Algorithm proto types
//unsigned int MillerShuffleAlgo_a(unsigned int, unsigned int, unsigned int);
//unsigned int MillerShuffleAlgo_b(unsigned int, unsigned int, unsigned int);
unsigned int MillerShuffleAlgo_d(unsigned int, unsigned int, unsigned int);
unsigned int MillerShuffleAlgo_e(unsigned int, unsigned int, unsigned int);
unsigned int MillerShuffle_lite (unsigned int, unsigned int, unsigned int);
//unsigned int MillerShuffle_Max  (unsigned int, unsigned int, unsigned int);
unsigned int DDeck_Shuffle      (unsigned int, unsigned int, unsigned int);

// perhaps better descriptive Names, based on application
#define MillerShuffle MillerShuffleAlgo_d
#define PRIG MillerShuffleAlgo_d
