#  the Miller Shuffle Algorithm

This is a new Shuffle Algorithm.

When implementing play-list shuffle algorithms, apparently some (even on big name electronics and streaming services), simply use an operation like songIndex=random(NumOfSongs). This use of a PRNG will give you a good mathematical randomness to what is played. But you will get many premature repetitions of song selection.

The Fisher-Yates (aka Knuth) algorithm has been a solution that fixes this unwanted repetition. The issue this algorithm does come with is the added burden of an array in RAM memory of 2 times the maximum number of songs (for up to 65,000 items 128KB of RAM is needed) being dedicated to shuffled indexes for the duration that access to additional items from the shuffle are desired. The array is normally maintained by the calling routine, and passed by reference to a function implementing the Fisher-Yates algorithm. This is a significant issue for a resource limited microprocessor application as well as for an online service with millions upon millions of shuffle lists to maintain.

The algorithm I present here (refered to as the Miller Shuffle algorithm) provides basically the same beneficial functionality with a comparable level of randomness, without the need of any array or upfront processing, and does not utilizing a PRNG. 
It reduces the algorithm's time complexity to O(1) from O(n) for Fisher-Yates and O(n^2) for naive implementations. It is essentially a [Pseudo Random **Index** Generator](https://en.wikipedia.org/wiki/Pseudorandom_index_generator) (**PRIG**).  
As defined herein, a Pseudo Random Index Generator (PRIG) returns each possible value in a range once and only once in a pseudo random order, with the input of a 'shuffleID' and a reference index (0 to N-1, generally used sequentially). 
Additionally when utilizing a PRIG, there is no need for an array to serve as a play history record. You can simply decrement the reference index to step back through the play history.

Characteristics of the Miller Shuffle algorithm (not variant -B)
  * Provides a pseudo random, yet unique, index within a given stated range and a value from that range. 
  * does Not require RAM memory for an array (saves 2 * size of the # of indexes, over FYA)
  * No upfront processing. Minimal processing to generate any shuffled index on the fly.
  * Deterministic. Does not need a record of past plays in order to go back through selections (like using random() would)
  * Not dependent on a system PRNG.

The way the algorithm works its magic is by utilizing multiple curated computations which are ‘symmetrical’, in that the range of values which go in are the same values which come out albeit in a different order. Conceptually each computation {e.g.  si=(i+K) mod N } stirs or scatters about the values within its pot (aka: range 0 to N-1) in a different way such that the combined result is like a well randomized shuffle of the values within the range.
This is achieved without the processing of intermediate “candidates” which are redundant or out of range values (unlike with the use of a PRNG or LFSR) which would cause a geometrically increasing inefficiency, due to the overhead of retries.

In applications where an even distribution of expected patterns like a given pair of cards from a 'shuffled' deck is near esseniencial there is room for improvement. To handle this, I devised the Miller Shuffle Algo-b. I can only see where this could be considered to be earnestly needed is where money is involved, like in a casino gaming machine.

Characteristics of the Miller Shuffle algorithm B
  * Provides a pseudo random, yet unique, index within a given stated range and a value from that range.
  * does Not require RAM memory for an array (2 * size of the # of indexes, like FYA)
  * Any given combinations occur as Expected over a period of time.
  * Non-deterministic. If there is a functional need to back track history, a record would need to be kept. In the foreseen application this would not be needed. (Diagnostic recreation is though possible by way of re-seeding the system pseudo random generator and replaying from the beginning of a session.)
  * does utilize system's Pseudo Random Number Generator

Note that while you don't get any repeats within a Fisher-Yates shuffle, new session reshuffles (done with FY) result in session to session repeats. Further these unrequested re-shuffles result in loss of play history.
When utilizing a Miller Shuffle algorithm, a logical reference index and a shuffleID are all that needs to be retained in order to continue where one left off.

Their statistical behavior has been extensively tested, honed and validated.
For more details, randomness statistics and efficacy analysis, of the Miller Shuffle Algo see:
https://www.instructables.com/Miller-Shuffle-Algorithm/

In this repository there is a simple executable program "exampleShuffles" using MillerShuffleAlgo() in a comparison with the use of rand() shuffling a set of items. There is also a Javascript implementation .

29 Aug 22 Update:
-----------------
Added ***Miller Shuffle algo-C*** with a Combo of A&Bs advantages (algo_A features + algo_B's pair distribution)
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
MS_d     1.11    257   1.58%   16.6    38.6     4.6   17.6   *** New April 2023
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

As if pseudo randomness had not been covered enough...

Having developed Signal Processing Software in the past, I thought to do some frequency analysis on the shuffle index data.
Knowing that true random data should contain minimal signal levels at any one frequency
(see google sheet: https://docs.google.com/spreadsheets/d/1RVwz7cU0CNlTK5enxMkLoOTPnRUN5b5pSYV5Rjto1h8/edit?usp=sharing )

April 2023 Update:
------------------

Added ***Miller Shuffle algo-D***
There was some interest in maximizing the total number of shuffle permutations generated by the algorithm over a vast number of uses. Not being able to calculate this directly from the mathematical operations involved, I created a couple of tests to provide measurable indications for comparison. 

First simulated dealing 5 cards (from a deck of 52) and tracked the combination of their suits. The possibilities are 4x4x4x4x4 equaling 1024. The results are sort of a hybrid between a combination and a permutation, as the number available cards of a suit for selection reduces as they get selected. Nonetheless, over 102,400 samples the number of occurrences for each possible outcome should be in the order of a nice bell curve distribution about a value of 100.
Next I tested the shuffle algorithms selecting 4 items out of 10.   This produces 5040 possibilities, a permutation of 10x9x8x7. I did this over 250,000 samples and checked for an even distribution across all the possibilities.
I also added a randomness test on the delta of sequential values generated.

These tests (‘deal2’, ‘permu5k’ & ’-dv rand’) then guided the architecting of the  MSA -d variant, which on these new measures performs much better than the -c variant algorithm and on par with Fisher-Yates.

If you would like to compare the randomness and distribution test results, you can see them here [Statistics of Shuffle algorithms](https://docs.google.com/document/d/1tHUKb0QNdGcvMp39ofUcqJBImnKHFtMtx7zCVNSsNMU/edit). I don't think understanding of the internal test processes is needed. Simply use the results from Fisher-Yates and radiation data as references. Take into consideration that in general on re-running any test the results will possibly vary +/- 10%.  Where the results from using radiation data differ significantly with both MSA-d and Fisher-Yates is due to its randomness without the constraints of giving unique values over a range.

**Later**, I tested using consecutive shuffleIDs instead of random ones and found that MillerShuffleAlgo-D continued to produce good random shuffles, but the distribution of shuffle permutations over time suffered some. I managed to resolve this with a simple change to the calculation of the ‘r2’ random factor (see code). Now, with this change, there is not even a need to use a PRNG function for the selection of shuffleIDs. When you’re ready for a new shuffle you can simply increment (shuffleID++).

***In the spirit of the 500+ horse-power commuter car, a solution looking for a problem:***  
For general game and media play, I feel that MSA-d goes beyond what's needed in the way of random behavior. But for those who feel they need or want to take things to the max, beyond dwindling returns, I brought together a larger collection of various mathematical means to stir, mix, switch about, fold and resequence (“randomizing”) the index values in the creation of the MSA -Max variant. Its statistical performance is almost indistinguishable from that of Fisher-Yates.
Note though, that the same level of results could have been obtained by using MSA-d twice.
```
    si = MSA_d( MSA_d(i,sid,n), sid+p, n);   // where p is a prime #
```

MSA-Max will retain its advantage over FY in regards to memory requirements and setup, but it will likely consume more processing time if one iterates over the full range of possible values. Yet in a case where you only utilize 100 selections out of a song library of 1000, you would save significantly in overall processing time with MSA-Max.

The files MillerShuffle.c & .h have accordingly been updated. Only MSA-d has been added to the JavaScript source. 

Supporting the shuffling of a larger number of items.
----------------------------------------------------
If you want or have a need to support many thousands of items then select prime numbers for p1&p2 above that.
The prime number values are not critical, but must be greater than any value of 'listsize' to be used.
To help ensure pseudo randomness p2 should be ~= p1 times the golden ratio (1.618) or its square root.

With the prime values below (used in MSA-C) I tested with every value of 'listsize' from 2 to p1-1.
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
Application note for use of small list sizes:
------------------
For very small values of "listSize" some randomness factors (e.g. distribution of permutations) are tough, and MS-a, MS-b and somewhat MS-c are not the best. The latest variant, **MS-d does perform the best** over any earlier variant (=F-Y). MS-lite is as acceptable as it ever is.

Albeit, I don't think anyone generally could notice the difference, without doing a statistical analysis of a large sample.
An example use case is where I selected 3 unique moles out of 5 in a whack-a-mole game:
```
   mole1 = prig(i++, 5);  // get 3 unique nums (0-4) from a PRIG function
   mole2 = prig(i++, 5);  // without any re-iterative testing & handling for repeats
   mole3 = prig(i++, 5);
```
Here, I renamed the MS-lite function to prig(), and separately randomized a "shuffleID".
