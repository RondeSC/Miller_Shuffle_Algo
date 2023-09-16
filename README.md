#  the Miller Shuffle Algorithm

This is a new Shuffle Algorithm.

When implementing play-list shuffle algorithms, apparently some (even on big name electronics and streaming services), simply use an operation like songIndex=random(NumOfSongs). This use of a PRNG will give you a good mathematical randomness to what is played. But you will get many premature repetitions of song selections.

The Fisher-Yates (aka Knuth) algorithm has been a solution that fixes this unwanted repetition. The issue this algorithm does come with is the added burden of an array in RAM memory of 2 times the maximum number of songs (for up to 65,000 items 128KB of RAM is needed) being dedicated to shuffled indexes for the duration that access to additional items from the shuffle are desired. The array is normally maintained by the calling routine, and passed by reference to a function implementing the Fisher-Yates algorithm. This is a significant issue for a resource limited microprocessor application as well as for an online service with millions upon millions of shuffle lists to maintain.

The algorithm I present here (refered to as the Miller Shuffle algorithm) provides basically the same beneficial functionality with a comparable level of randomness, without the need of any array or upfront processing, and does not utilizing a PRNG. 
It reduces the algorithm's time complexity to O(1) from O(n) for Fisher-Yates and O(n^2) for naive implementations. It is essentially a [Pseudo Random **Index** Generator](https://docs.google.com/document/d/1UOzZNXHsaTuRHNFvPH_tQwVWfTXUj9xP) (**PRIG**).  
As defined herein, a Pseudo Random Index Generator (PRIG) returns each possible value in a range once and only once in a pseudo random order, with the input of a 'shuffleID' and a reference index (0 to N-1, generally used sequentially). 
Additionally when utilizing a PRIG, there is no need for an array to serve as a play history record. You can simply decrement the reference index to step back through the play history.

Characteristics of the Miller Shuffle algorithm
  * Provides a pseudo random, yet unique, index within a given stated range and a reference value from that range. 
  * does Not require RAM memory for an array (saves 2 * size of the # of indexes, over F-Y algo)
  * No upfront processing. Minimal processing to generate any shuffled index on the fly.
  * Deterministic (except B variant). Does not need a record of past plays in order to go back through selections (like using random() would)
  * Not dependent on a system PRNG (except B variant).

The way the algorithm works its magic is by utilizing multiple curated computations which are ‘symmetrical’, in that the range of values which go in are the same values which come out albeit in a different order. Conceptually each computation {e.g.  si=(i+K) mod N } stirs or scatters about the values within its pot (aka: range 0 to N-1) in a different way such that the combined result is a well randomized shuffle of the values within the range.
This is achieved without the processing of intermediate “candidates” which are redundant or out of range values (unlike with the use of a PRNG or LFSR) which would cause a geometrically increasing inefficiency, due to the overhead of retries.

In applications where an even distribution of expected patterns like a given pair of cards from a 'shuffled' deck is near esseniencial there is room for improvement. To handle this, I devised the Miller Shuffle Algo-b *(no longer necessary given algo-D or later)*, which adds in a little use of a PRNG function. I can only see where this could be considered to be earnestly needed is where money is involved, like in a casino gaming machine.

Note that while you don't get any repeats within a Fisher-Yates shuffle, new session reshuffles (done with FY) result in session to session repeats. Further these unrequested re-shuffles result in loss of play history.
When utilizing a Miller Shuffle algorithm, a logical reference index and a shuffleID are all that needs to be retained in order to continue where one left off.
Examples of when some code might fetch a new Shuffle Index (si) from a shuffle: ...when a blackJack player says “hit me”, or draws a couple of Dominoes or Scrabble tiles, or needs the next song from their shuffled playlist. A perfectly suited case: would be to select a daily Wordle solution word from a curated dictionary, knowing that it couldn’t have been used in the past months or even years.

Miller Shuffle Algos' statistical behavior have been extensively tested, honed and validated over time.
For details, on initial development, randomness statistics and efficacy analysis, of the Miller Shuffle Algo see:
https://www.instructables.com/Miller-Shuffle-Algorithm/

In this repository there is a simple executable program "exampleShuffles" using MillerShuffleAlgo() in a comparison with the use of rand() shuffling a set of items. There is also a Javascript implementation .

To get a visual feel for the randomness of a given shuffle I have done shatter charts spacially mapping consecutive selection pairs. Where bunching occurs it might correlate to going between the same two groups of items (e.g. suits, card face values or albums). [Take a look.](https://docs.google.com/spreadsheets/d/1n-cfXohH4p2NeRkCWs8eUEnNjbuzbWRlPC8en-Ht3qM/edit?usp=sharing)  It’s not that these events are happening close in time as the charts have no time axis. 

Details on earilier variant changes and history of randomness test results were moved to: Variant_Evolution.md  
Comments on supporting very large and small listsizes has been moved to file: Large_small_item_cnts.md

June 2023 Update:
-----------------
MillerShuffleAlgo_e (& MSA_d) have had two simple XOR operations added. While not especially improving the randomness of items found within a shuffle, the ability to produce a multitude (billions) of shuffle permutations is greatly enhanced. The algorithm's likelihood of giving a repeated permutation goes from ~1:1000 to ~1:100000 (maybe even much better than that, I am still having a little hard time separating out repeated ShuffleIDs from repeated shuffle permutations). I have expanded and improved my testing suite, aiding in the algorithms’ improvement. MSA_d has apparently slightly more randomness with in a shuffle while MSA_e is better at producing unique shuffles.
MS_lite also was updated and now is better than MSA_a, MSA_c and the original MSA_b were. The checksum values for the algorithms were accordingly updated.

```
                        dv     Geo.  Permu   repeated-shuffles   r2D
                ChiSq  ChiSq   err    devi   /million  first@    Mark
MSlite
    prior:       489    546   4.24%   60.1     903k      493     1.95
    updated:     283    365   1.82%   37.6      250    62722     0.88
MSA_d
    prior:       257    351   1.64%   58.6    32968     2107     0.84
    updated:     254    282   1.50%   29.1      29    127601     0.65
MSA_e
    prior:       263    363   1.28%   41.5     290     43742     0.68
    updated:     261    303   1.27%   27.4      10    196568     0.68
	
Nominal:         255    255     <<     <        <<       >>       <
	
  Figures here are not directly comparable with earilier results,
  due to testing and shuffleID generation changes.
```
The repeated permutation statistics are from shuffling 52 items. As the number of items being shuffled goes above 200 the number of repeated shuffles per million approaches zero, for all current MillerShuffle variations including the MS eXtra lite version.

If really needed, or for academic pursuit, the potential numbers of unique shuffle permutations could be improved by orders of magnitude by increasing the shuffleID beyond 32 bits and using those bits to increase the algorithm’s randomizing factors. Alternatively a second 32bit input value could be utilized. One easy way  to effectively do this is to do back to back shuffles ie: myItem = MShuffle(MShuffle(i,sID,n), sID2,n). With two randomly generated 32bit shuffleIDs. I found the results, doing the latter, indistinguishable from those of Fisher-Yates; and generated 0 repeated shuffles while testing the generation of billions of unique shuffle permutations (a billion-billions may be possible).

If you don’t mind giving up the deterministic feature, you can alternatively utilize MSA_b instead, for the improvement in permutation generation (of ~ x1000). It is likly that 99.9% of all applications could be best served by either MS_lite, MSA_d or MSA_e shuffle algorithms.

I have included a function: random() to give 32bit pseudo random values (>4 billion) suitable for use as shuffleIDs. Using the C function rand(), as is, to set ‘shuffleID’s will yield about 32K unique shuffles, and may very well suit your needs.
 - - -
Up until recently I have been getting tens of repeats/million from my 32b random function. Previous to that, I had been dealing with 100s/million.
I tried almost everything and got nowhere improving on this, until I did. I am glad to report that I updated my random() function and now it will yield over 500 million unique values in a row. This is as coded, compiled with VS-C 2022, and ran on my PC; and is dependent on the associated rand() implementation. You may get different results with other development stacks. 
So now the testing of MSA_d reports only 12 repeats/million, and MSA_e reports 0/million. Further testing of **MSA_e results in Zero repeated shuffles** out of the 500 million generated. No telling how many billions of 52 item shuffle permutations it could potentially provide.  
Note: Due to the hash table based process I use for testing, there may actually be many times more unique shuffles being generated, and there is theoretically some chance that there are duplicate shuffles that are not detected.
 - - -
Prior to **Aug 2023**, the randomizing factors (r1-rx) were set empirically for best test results. Now setting them to modulo values of different primes, where the primes multiplied together is greater than the SID 32bit value range ensures that the r-factors (e.g. r1,r2,r3) are a unique set per a corollary to the Chinese remainder theorem.  
Doing this for the r-factors maximizes the potential shuffle permutations generated. This has little effect on the random nature of a given shuffle. Further I don’t consider that using different r-factors constitutes a different Miller Shuffle variant. The heart of a variant is determined by the algorithm making up its combined shuffle operations. This is also what predominantly determines the random nature of the shuffles generated.  
The latest MillerShuffleAlgo -E has been updated accordingly.

