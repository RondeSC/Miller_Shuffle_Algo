#  the Miller Shuffle Algorithm

This is a new Shuffle Algorithm.

When implementing play-list shuffle algorithms, apparently some (even on big name electronics and streaming services), simply use an operation like songIndex=random(NumOfSongs). This use of a PRNG will give you a good mathematical randomness to what is played. But you will get many premature repetitions of song selection.

The Fisher-Yates (aka Knuth) algorithm has been a solution that fixes this unwanted repetition. The issue this algorithm does come with is the added burden of an array in RAM memory of 2 times the maximum number of songs (for up to 65,000 items 128KB of RAM is needed) being dedicated to shuffled indexes for the duration that access to additional items from the shuffle are desired. This is a significant issue for a resource limited microprocessor application as well as for an online service with millions upon millions of shuffle lists to maintain.

The algorithm I present here (refered to as the Miller Shuffle algorithm) provides basically the same beneficial functionality with a comparable level of randomness, without the need of any array or upfront processing, and does not utilizing a PRNG. It essentially is what I would call a Pseudo Random Index Generator (PRIG).

Characteristics of the Miller Shuffle algorithm (not variant -B)
  * Provides a pseudo random, yet unique, index within a given stated range and a value from that range. 
  * does Not require RAM memory for an array (saves 2 * size of the # of indexes, over FYA)
  * No upfront processing. Minimal processing to generate any shuffled index on the fly.
  * Deterministic. Does not need a record of past plays in order to go back through selections (like using random() would)
  * Not dependent on a system PRNG.

In applications where an even distribution of expected patterns like a given pair of cards from a 'shuffled' deck is near esseniencial there is room for improvement. To handle this, I devised the Miller Shuffle Algo-b. I can only see where this could be considered to be earnestly needed is where money is involved, like in a casino gaming machine.

Characteristics of the Miller Shuffle algorithm B
  * Provides a pseudo random, yet unique, index within a given stated range and a value from that range.
  * does Not require RAM memory for an array (2 * size of the # of indexes, like FYA)
  * Any given combinations occur as Expected over a period of time.
  * Non-deterministic. If there is a functional need to back track history, a record would need to be kept. In the foreseen application this would not be needed. (Diagnostic recreation is though possible by way of re-seeding the system pseudo random generator and replaying from the beginning of a session.)
  * does utilize system's Pseudo Random Number Generator

Their statistical behavior has been extensively tested, honed and validated.
For more details, randomness statistics and efficacy analysis, of the Miller Shuffle Algo see:
https://www.instructables.com/Miller-Shuffle-Algorithm/

In this repository there is a simple executable program "exampleShuffles" using MillerShuffleAlgo() in a comparison with the use of rand() shuffling a set of items. There is also a Javascript implementation .

29 Aug 22 Update:
-----------------
Added Miller Shuffle algo-C with a Combo of A&Bs advantages (algo_A features + algo_B's pair distribution)
Also added a lite variation which for many applications will serve just fine.

I have improved and expanded my testing, and added a sequence permutation test (akind to "Diehard test")
For the most part these figures are average results of 10 arbitary runs of each test of >1000 data points. 
Except test for pairs dealt was 5 runs of >26,000 shuffles. Here are some representative results:

```
                        avg.   album   early   dealt pair avg.
         Permu  ChiSq   err    ChiSq  repeats   min    Max
Fisher   1.79    250   1.01%   17.3    45.5     4.6   14.2
MS_a     3.16   1008   0.94%   50.5    32.8     0.4   27.8
MS_b     3.12    426   1.37%   99.4    45.8     3.4   23.2
MS_c     1.50    398   1.18%   40.6    43.3     4.6   21.0
MSlite   6.94    649   3.61%   29.0    39.1     1.0   28.2
using Radiation Data:
         1.64    246   2.15%   24.2    48.2     5.0   20.0
Nominal:   <     253     <     38.3    <50       -  10  +

Permu:    deviation from expected distribution based on sequence permutations of sets of 4 numbers
ChiSq:    ~byte 8-bit Chi-Sq
avg err:  average %err from a 2D-Pi,3D-Pi, Golden Ratio and a mean calculation
album ChiSq: of album distrution (tested as 40 albums of 13 songs each)
early repeats: of an album ('early' value set arbitrarily)
dealt pair: average min&max counts of a pair (eg: AceSpades+JackHearts) dealt to one of 10 players out of >26,000 shuffles dealt
```
Supporting the shuffling of a larger number of items.
----------------------------------------------------
If you want or have a need to support many thousands of items then select prime numbers for p1&p2 above that.
The prime number values are not critical, but must be greater than any value of 'listsize' to be used.
To help ensure pseudo randomness p2 should be ~= p1 times the golden ratio (1.618) or its square root.

With the prime values below (now used in MSA-C) I tested with every value of 'listsize' from 2 to p1-1.
These are about as large as these can be, such that the variables in the algorithm only need to be 16bit unsigned, and the math 32 bits unsigned (I think, the easiest way to insure that is to use 32bit unsigned variables).
Given that the unsigned int variables are implemented as 32 bits, the (unsigned long) type casts in the code are unnecessary (they are needed for 8bit Arduinos).
```
    p1=50021;  // for shuffling upto 50,000 indexes (with only 32bit unsigned math support)
    p2=63629;  
```

If you use much bigger primes you risk overflowing the supported range of a variable type.
To support very large shuffle index ranges into the billions you must make sure that the internal math is done in 64 bits. 
This is cared for with the (long) type casting within the operations that can produce extra large numbers (see code).
Your compiler may use 32 bits for a 'long', in which case you may need to use (long long) or (__int64) type casts.

I have extensively tested with listsize upto 10 million using these primes:
```
    p1=10000019;
    p2=16180369; // ~= p1*the Golden Ratio 1.618034  (the value is key, but not critical)
```

I also did several successful tests upto listsize of ~ 1 billion using these primes:
```
    p1=1000000007;
    p2=1272019667; // ~= p1*SQR(1.618034)

    This can handle the whole Spotify and Apple Music song catalogs put together.
```
