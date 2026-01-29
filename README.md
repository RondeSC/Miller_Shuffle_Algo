#  the Miller Shuffle Algorithm

This is a new Shuffle Algorithm.

When implementing play-list shuffle algorithms, apparently some (even on big name electronics and streaming services), simply use an operation like songIndex=random(NumOfSongs). This use of a PRNG will give you a good mathematical randomness to what is played. But you will get many premature repetitions of song selections.

The Fisher-Yates (aka Knuth) algorithm has been a solution that fixes this unwanted repetition. The issue this algorithm does come with is the added burden of an array in RAM memory of 2 times the maximum number of songs (for up to 65,000 items 128KB of RAM is needed) being dedicated to shuffled indexes for the duration that access to additional items from the shuffle are desired. The array is normally maintained by the calling routine, and passed by reference to a function implementing the Fisher-Yates algorithm. This is a significant issue for a resource limited microprocessor application as well as for an online service with millions upon millions of shuffle lists to maintain.

The algorithm I present here (refered to as the Miller Shuffle Algorithm) provides basically the same beneficial functionality with a comparable level of randomness, without the need of any array or upfront processing, and does not utilizing a PRNG. 
It reduces the algorithm's time complexity to O(1) from O(n) for Fisher-Yates and O(n^2) for naive implementations. It is essentially a [Pseudo Random **Index** Generator](https://docs.google.com/document/d/1UOzZNXHsaTuRHNFvPH_tQwVWfTXUj9xP) (**PRIG**).  
As defined herein, a Pseudo Random Index Generator (PRIG) returns each possible value in a range once and only once in a pseudo random order, with the input of a 'shuffleID' and a reference index (0 to N-1, generally used sequentially). 
Additionally when utilizing a PRIG, there is no need for an array to serve as a play history record. You can simply decrement the reference index to step back through the play history.

Characteristics of the Miller Shuffle algorithm
  * Provides a pseudo random, yet unique, index within a given stated range and a reference value from that range. 
  * does Not require RAM memory for an array (saves 2 * size of the # of indexes, over F-Y algo)
  * No upfront processing. Minimal processing to generate any shuffled index on the fly.
  * Deterministic. Does not need a record of past plays in order to go back through selections (like using random() would)
  * Not dependent on a system PRNG.

The way the algorithm works its magic is by utilizing multiple curated computations which are ‘symmetrical’, in that the range of values which go in are the same values which come out albeit in a different order. Conceptually each computation {e.g.  si=(i+K) mod N } stirs or scatters about the values within its pot (aka: range 0 to N-1) in a different way such that the combined result is a well randomized shuffle of the values within the range.
This is achieved without the processing of intermediate “candidates” which are redundant or out of range values (unlike with the use of a PRNG or LFSR) which would cause a geometrically increasing inefficiency, due to the overhead of retries.


Note that while you don't get any repeats within a Fisher-Yates shuffle, new session reshuffles (done with FY) result in session to session repeats. Further these unrequested re-shuffles result in loss of play history.
When utilizing a Miller Shuffle algorithm, a logical reference index and a shuffleID are all that needs to be retained in order to continue where one left off.
Examples of when some code might fetch a new Shuffle Index (si) from a preexisting shuffle: ...when a blackJack player says “hit me”, or draws a couple of Dominoes or Scrabble tiles, or needs the next song from their shuffled playlist. A perfectly suited case: would be to select a daily Wordle solution word from a curated dictionary, knowing that it couldn’t have been used in the past months or even years.

Miller Shuffle Algos' statistical behavior have been extensively tested, honed and validated over time.
For details on initial early development, randomness statistics and efficacy analysis, of the Miller Shuffle Algo see:
https://www.instructables.com/Miller-Shuffle-Algorithm/

In this repository there is a simple executable program "exampleShuffles" using MillerShuffleAlgo() in a comparison with the use of rand() shuffling a set of items. There is also a Javascript implementation of the algorithums.

To get a visual feel for the randomness of a given shuffle I have done scatter charts spacially mapping consecutive selection pairs. Where bunching occurs it might correlate to going between the same two groups of items (e.g. suits, card face values or albums). [Take a look.](https://docs.google.com/spreadsheets/d/1n-cfXohH4p2NeRkCWs8eUEnNjbuzbWRlPC8en-Ht3qM/edit?usp=sharing)  It’s not that these events are nessesarially happening close in time (the charts have no time axis) but do reflect a pattern. 

 - - -
Details on earilier (2022-2024) variant changes and history of randomness test results were moved to: Variant_Evolution.md and are of little to no value now, and can be miss-informative.

**Notable updates from 2024:**

You may be interested to know that I have developed a [Super-Shuffle](https://github.com/RondeSC/Super_Shuffle) for the prevention of **inter shuffle** bunching of like items.

Added MillerShuffle_Demo.html an interactive web page which I think is fun and hopefully illustrative of the randomness nature of the Miller Shuffle Algo and others. Within the JavaScript code three use cases are presented.

<img width="1530" height="868" alt="MSA_Demo_image" src="https://github.com/user-attachments/assets/25b83b9a-ec6d-47a7-93e4-9e4c5bd09202" /> 
<br/><br/>

**Aug 2025 Update:**

MSA-d and MS_xlite have been removed, in favor of an updated MS_lite. The updated MS_lite performs statistically on par with the MSA-d, and recommended in any application not requiring the extreme item counts or the billions of consecutive unique shuffles of MSA-e.

The interactive web page "MillerShuffle_Demo.html" has been significantly expanded. Data sets of shuffles are now characterized in three ways (with values as originally done, delta values and delta index offsets), which are measured statically and drive plots. Additional types of statistical analysis are printed out, and there are two new plots: a histogram of values and that of colorized accumulative grid point (x,y) occurrences.

Here are some samples of interesting plots (BTW: better shuffles tend to give boring plots).
![Shuffle_plots](https://github.com/user-attachments/assets/73fadddc-3a74-4244-8447-fb85cde185b9)

 - - -
The file Large_small_item_cnts.md has been removed, as currently mostly rerevalent.

 Jan 2026 Update
----------------
With added interest in support for larger item counts, I expanded MS_lite to handle the full range of a 'c' standard integer ( up to 32767, similar to that of the rand() function). MSA_e was given optimized prime constants so as to support shuffling up to 65536 items.
The selection of constants to use in order to enable expansion of the number of items that can be properly shuffled was optimized by rigorous automatic performance evaluation consuming weeks of 24/7 multi-core testing.

I set out to choose a pair of primary primes to use, from a list of ~3000 prime values. The number of possible sets was 9,000,000. I measured the quality of the shuffles produced with a given pair with a battery of test results, of various natures. To test one pair took ~20secs (looking for duplicates over several millions of different sized shuffles took most of the time). To test them all would take 5.7 years of processing for me. So I did a fraction of that. But I did not want to do a fenced in region of the whole field, and I didn't want to waste time on repeated pairs due to the  use of a PRNG. I realized that using a PRIG could give me the pair selections I needed and wanted. Considering a unique pair as a two digit 3000,3000 number I could use a  PRIG to give out instances of the 9 million and then let p1=iNum/3000 and p2=iNum%3000. Hence I found myself needing a PRIG that could handle counts of 9 million.
So I created a new PRIG version: MS_Large. It can handle items numbering over 4 billion (up to 4,294,967,296). Special care had to be taken (resulting in added complexity) as well as exhaustive testing had to be done. I tested MS_Large for proper shuffles with item counts from 1 up to 1 billion, including boundary testing at 2^32-1 and 2^32.

Results: 

In the end both MS-lite and MSA-e yielded notably better results in ~ 20 of 30 performance test measurements and gave ~ = results in all the rest. The new MS-Large's randomness and unique generation statistics are ~ ½ way between that of MS-lite and MSA-e.
BTW, the updated stats for 52 item shuffle uniqueness are:
The 'lite' Miller Shuffle PRIG algo will generally give you 100-200K unique shuffles prior to a repeated one. 
The new MS-Large gets on average <0.1 repeated shuffles in a million.
The MSA-e algo will produce a unique shuffle (of 52 items) for each of the >4 billion possible ShuffleID values.
All 3 algos now support shuffling item counts down to 2, even 1 ;~}



