# Cache-Simulator
Implemented a cache simulator given various parameters, and then watching it perform under different circumstances. 

When simulator starts, it takes from stdin (so no files are opened/closed/used in the project code) input in the following format:

The first four lines contain the fundamental parameters of the cache, one per line. All four are integers:
S
E
B
m

The next line contains the replacement policy. The only ones implemented are LFU and LRU. The replacement policy will be entered as either LFU or LRU, so read three chars here.

The next two lines contains the fundamental parameters of the hardware: the hit time (h) and the miss penalty (p). Both are integers:
h
p

That was the input. The runtime information follows immediately. All further lines contain integer addresses that your simulation attempts to read from main memory through the cache, one per line. The simulation stops when the "address" you read is -1.

It then outputs all information to stdout.
