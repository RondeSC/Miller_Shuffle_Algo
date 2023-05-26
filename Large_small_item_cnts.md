
Supporting the shuffling of a larger number of items.  (moved from the general README.md file)
----------------------------------------------------
If you want or have a need to support many thousands of items then select your prime numbers to be above that.
The prime number values are not critical, but must be greater than any value of 'listsize' to be used.
In use, if a prime is used for modulo operations on a reduced range (1/3 or 1/2 #items) then the prime can also be reduced.
To help ensure pseudo randomness p2 should be ~= p1 times the golden ratio (1.618) or its square root. This appears more true for MSA-a & MSA-c than later variants.

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
