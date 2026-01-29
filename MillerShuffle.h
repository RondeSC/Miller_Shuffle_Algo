#include <stdint.h>

// Miller Shuffle Algorithm proto types
uint32_t MillerShuffleAlgo_e(uint32_t, uint32_t, uint32_t);
uint32_t MillerShuffle_Large(uint32_t, uint32_t, uint32_t);
unsigned short int MillerShuffle_lite(unsigned short, unsigned long, unsigned short);

unsigned int DDeck_Shuffle(unsigned int, unsigned int, unsigned int);

// perhaps better descriptive Names, given the application
#define MillerShuffle MillerShuffleAlgo_e
#define PRIG MillerShuffleAlgo_e
