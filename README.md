#  the Miller Shuffle Algorithm

This is a new Shuffle Algorithm.

When implementing play-list shuffle algorithms, apparently some (even on big name electronics and streaming services), simply use an operation like songIndex=random(NumOfSongs). This use of a PRNG will give you a good mathematical randomness to what is played. But you will get many premature repetitions of song selection.

The Fisher-Yates (aka Knuth) algorithm has been a solution that fixes this unwanted repetition. The issue this algorithm does come with is the added burden of an array in RAM memory of 2 times the maximum number of songs (for up to 65,000 items 128KB of RAM is needed) being dedicated to shuffled indexes for the duration that access to additional items from the shuffle are desired. This is a significant issue for a resource limited microprocessor application as well as for an online service with millions upon millions of shuffle lists to maintain.

The algorithm I present here (refered to as the Miller Shuffle algorithm) provides basically the same beneficial functionality with a comparable level of randomness, without the need of any array or upfront processing, and does not utilizing a PRNG. It essentially is what I would call a Pseudo Random Index Generator (PRIG).

Characteristics of the Miller Shuffle algorithm
  * Provides a pseudo random, yet unique, index within a given stated range and a value from that range. 
  * does Not require RAM memory for an array (saves 2 * size of the # of indexes, over FYA)
  * No upfront processing. Minimal processing to generate any shuffled index on the fly.
  * does Not need a record of past plays in order to go back through selections (like using random() would)
  * Not dependent on a system PRNG.

In applications where an even distribution of expected patterns like a given pair of cards from a 'shuffled' deck is near esseniencial there is room for improvement. To handle this, I devised the Miller Shuffle Algo-b. I can only see where this could be considered to be earnestly needed is where money is involved, like in a casino gaming machine.

Characteristics of the Miller Shuffle algorithm B
  * Provides a pseudo random, yet unique, index within a given stated range and a value from that range.
  * does Not require RAM memory for an array (2 * size of the # of indexes, like FYA)
  * Any given combinations occur as Expected over a period of time.
  * If there is a functional need to back track history, a record would need to be kept. In the foreseen application this would not be needed. (Diagnostic recreation is though possible by way of re-seeding the system pseudo random generator and replaying from the beginning of a session.)

Their statistical behavior has been extensively tested, honed and validated.
For more details and analysis of the Miller Shuffle Algo see:
https://www.instructables.com/Miller-Shuffle-Algorithm/

In this repository there is a simple executable program "exampleShuffles" using MillerShuffleAlgo() in a comparison with the use of rand() shuffling a set of items. There is also a Javascript implementation .
