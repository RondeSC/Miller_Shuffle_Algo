#  the Miller Shuffle Algorithm

This is a new Shuffle Algorithm, based on a new approach, with new benefits.

When implementing play-list shuffle algorithms, apparently some (even on big name electronics and streaming services), simply use an operation like songIndex=random(NumOfSongs). This use of a PRNG will give you a good mathematical randomness to what is played. But you will get many premature repetitions of song selections.

The Fisher-Yates (aka Knuth) algorithm has been a solution that fixes this unwanted repetition. The issue this algorithm does come with is the added burden of an array in RAM memory of 2 times the maximum number of songs (for up to 65,000 items 128KB of RAM is needed) being dedicated to shuffled indexes for the duration that access to additional items from the shuffle are desired. The array is normally maintained by the calling routine, and passed by reference to a function implementing the Fisher-Yates algorithm. This is a significant issue for a resource limited microprocessor application as well as for an online service with millions upon millions of shuffle lists to maintain.

The algorithm I present here (refered to as the Miller Shuffle algorithm) provides basically the same beneficial functionality with a comparable level of randomness, without the need of any array or upfront processing, and does not utilizing a PRNG. 
It reduces the algorithm's time complexity to O(1) from O(n) for Fisher-Yates and O(n^2) for naive implementations. It is essentially a [Pseudo Random **Index** Generator](https://en.wikipedia.org/wiki/Pseudorandom_index_generator) (**PRIG**).  
As defined herein, a Pseudo Random Index Generator (PRIG) returns each possible value in a range once and only once in a pseudo random order, with the input of a 'shuffleID' and a reference index (0 to N-1, generally used sequentially). 
Additionally when utilizing a PRIG, there is no need for an array to serve as a play history record. You can simply decrement the reference index to step back through the play history.

Characteristics of the Miller Shuffle algorithm
  * Provides a pseudo random, yet unique, index within a given stated range and a value from that range. 
  * does Not require RAM memory for an array (saves 2 * size of the # of indexes, over FYA)
  * No upfront processing. Minimal processing to generate any shuffled index on the fly.
  * Deterministic (except B variant). Does not need a record of past plays in order to go back through selections (like using random() would)
  * Not dependent on a system PRNG (except B variant).

The way the algorithm works its magic is by utilizing multiple curated computations which are ‘symmetrical’, in that the range of values which go in are the same values which come out albeit in a different order. Conceptually each computation {e.g.  si=(i+K) mod N } stirs or scatters about the values within its pot (aka: range 0 to N-1) in a different way such that the combined result is a well randomized shuffle of the values within the range.
This is achieved without the processing of intermediate “candidates” which are redundant or out of range values (unlike with the use of a PRNG or LFSR) which would cause a geometrically increasing inefficiency, due to the overhead of retries.

In applications where an even distribution of expected patterns like a given pair of cards from a 'shuffled' deck is near esseniencial there is room for improvement. To handle this, I devised the Miller Shuffle Algo-b *(no longer necessary given algo-D or later)*, which adds in a little use of a PRNG function. I can only see where this could be considered to be earnestly needed is where money is involved, like in a casino gaming machine.

Note that while you don't get any repeats within a Fisher-Yates shuffle, new session reshuffles (done with FY) result in session to session repeats. Further these unrequested re-shuffles result in loss of play history.
When utilizing a Miller Shuffle algorithm, a logical reference index and a shuffleID are all that needs to be retained in order to continue where one left off.

Miller Shuffle Algos' statistical behavior have been extensively tested, honed and validated.
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
MS_d     1.11    257   1.58%   16.6    38.6     4.6   17.6   added April 2023
MS_e     0.96    262   1.93%   18.6    44.2     5.0   17.0   added May 2023
MSlite   4.55    463   4.85%   14.3    40.5     0.6   24.8   updated "  "
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

April 2023 Update:
------------------
Added ***Miller Shuffle algo-D***
There was some interest in maximizing the total number of shuffle permutations generated by the algorithm over a vast number of uses. Not being able to calculate this directly from the mathematical operations involved, I created a couple of tests to provide measurable indications for comparison. 

First, simulated dealing 5 cards (from a deck of 52) and tracked the combination of their suits. The possibilities are 4x4x4x4x4 equaling 1024. The results are sort of a hybrid between a combination and a permutation, as the number available cards of a suit for selection reduces as they get selected. Nonetheless, over 102,400 samples the number of occurrences for each possible outcome should be in the order of a nice bell curve distribution about a value of 100.
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
MSA-Max will retain its advantage over FY in regards to memory requirements and setup, but it will likely consume more processing time if one iterates over the full range of possible values. Yet in a case where you only utilize 100 selections out of a song library of 1000, you would continue to save significantly in overall processing time with MSA-Max.

The files MillerShuffle.c & .h have accordingly been updated. Only MSA-d has been added to the JavaScript source. 

May 2023 Update:
----------------
With further academic interest in shuffle algorithms’ capacity to produce high numbers of shuffle permutations, I added testing for larger permutation spaces and then run automatic testing of combinations of potential (> a dozen) shuffle operations. This resulted in the ***Miller Shuffle algo-E*** variant code, which easily generates all possible permutations of the 'permu17k' test. You can see in these statistics that it performs nearly equivalent to F-Y in the extent of exhaustive permutation generation. (also improved MS-lite so as to be better than MSA-a was)

```
   algo: MSA-c
'rand'      	Perm24: 1.54   	bChi2: 217  devi: 15.0%   Chi2: 367 (~255)  ENT err: 1.67%
'deal2'     	Instances of Zero/One: 39,8	   avg deviation: 79.9%   L/H: -100.0% / +799%
'permu5k'   	Instances of Zero/One: 4670,0  avg deviation: 186.1%   L/H: -100.0% / +5092%
'permu17k'  	Instances of Zero/One: 16406,0 avg deviation: 191.4%   L/H: -100.0% / +10362%
   algo: MSA-d
'rand'      	Perm24: 1.11    bChi2: 237  devi: 9.6%    Chi2: 256 (~255)  ENT err: 1.58%
'deal2'     	Instances of Zero/One: 0,0	   avg deviation: 31.0%   L/H: -68.6% / +157%
'permu5k'   	Instances of Zero/One: 1070,0  avg deviation: 73.8%   L/H: -100.0% / +740%
'permu17k'  	Instances of Zero/One: 2444,0  avg deviation: 59.6%   L/H: -100.0% / +448%
   algo: Fisher-Yates
'rand'      	Perm24: 1.23    bChi2: 238  devi: 12.5%   Chi2: 249 (~255)  ENT err: 0.92%
'deal2'     	Instances of Zero/One: 0,0	avg deviation: 13.4%   L/H: -55.0% / +43%
'permu5k'   	Instances of Zero/One: 0,0	avg deviation: 12.1%   L/H: -47.6% / +54%
'permu17k'  	Instances of Zero/One: 0,0	avg deviation: 11.5%   L/H: -49.6% / +61%
   algo: MSA-E 
'rand'          Perm24: 0.75    bChi2: 229  devi: 11.3%   Chi2: 262 (~255)  ENT err: 1.85%
'deal2'         Instances of Zero/One: 0,0  avg deviation: 20.2%   L/H: -69.2% / +88% 
'permu5k'       Instances of Zero/One: 0,0  avg deviation: 39.0%   L/H: -90.4% / +412%
'permu17k'      Instances of Zero/One: 0,0  avg deviation: 28.9%   L/H: -90.4% / +169%
   from using Radiation Data
'rand'     	 	Perm24: 1.50    bChi2: 252  devi: 5.2%    Chi2: 252 (~255)  ENT err: 2.50%
'deal2'     	Instances of Zero/One: 0,0	avg deviation: 10.1%   L/H: -31.2% / +33%
'permu5k'   	Instances of Zero/One: 0,0	avg deviation: 48.5%   L/H: -81.6% / +1185%
'permu17k'  	Instances of Zero/One: 0,0	avg deviation: 45.3%   L/H: -75.2% / +1455%
```

Trade-off: Compared to MSA-d the items within a shuffle are no better randomly arranged. Meanwhile note that on average it does an additional shuffle operation ½ the time (BTW MSA-Max takes +4 shuffle operations, to get but a little more Fisher-Yates like results). 

Determining the relative importance of the various metrics of randomness is an art of evaluation and judgment.

To get a visual feel for the randomness of a given shuffle I have done shatter charts spacially mapping consecutive selection pairs. Where bunching occurs it might correlate to going between the same two groups of items (e.g. suits, card face values or albums). [Take a look](https://docs.google.com/spreadsheets/d/1n-cfXohH4p2NeRkCWs8eUEnNjbuzbWRlPC8en-Ht3qM/edit?usp=sharing)

Comments on supporting very large and small listsizes has been moved to file: Large_small_item_cnts.md
