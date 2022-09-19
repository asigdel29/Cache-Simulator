//While Compiling link via -lm required for referencing log2 [newcache function]
//gcc main.c -lm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Line {
    unsigned short valid;
    unsigned long indexset;
    unsigned long tag;
    unsigned long block;
    unsigned int lasthit;
    unsigned int use;
} Line;

typedef struct Set {
    Line * lines;
} Set;

typedef struct Cache {
    unsigned int numset;
    unsigned int linesset;
    unsigned int blocksize;
    unsigned int addressbit;
    unsigned int setbit;
    unsigned int blockbit;
    unsigned int tagbit;
    unsigned int hitt;
    unsigned int misst;
    char replace[4];
    Set * sets;
} Cache;
Line * linecreate (unsigned int linesset) {
    Line * newline = (Line*) malloc(linesset * sizeof(Line));
    for (int i = 0; i < linesset; i++) {
        newline[i].valid = 0;
        newline[i].lasthit = 0;
        newline[i].use = 0;
    }
    return newline;
}
Set * setcreate(unsigned int numset, unsigned int linesset) {
    Set * newset = (Set*) malloc(numset * sizeof(Set));
    for (int i = 0; i < numset; i++) {
        newset[i].lines = linecreate(linesset);
    }
    return newset;
}
Cache * newcache (unsigned int numset, unsigned int linesset,unsigned int blocksize, unsigned int addressbit,char replace[4], unsigned int hitt, unsigned int misst) {
    Cache * ncache = (Cache*) malloc(sizeof(Cache));
    ncache->numset = numset;
    ncache->linesset = linesset;
    ncache->blocksize = blocksize;
    ncache->addressbit = addressbit;
    strcpy(ncache->replace, replace);
    ncache->hitt = hitt;
    ncache->misst = misst;
    ncache->sets = setcreate(numset, linesset);
    ncache->setbit = log2(numset);
    ncache->blockbit = log2(blocksize);
    ncache->tagbit = addressbit - (ncache->setbit + ncache->blockbit);
    return ncache;
}
unsigned int cacheread(int addressread, Cache * cache, unsigned int currenttime) {
    unsigned long address = addressread;
    unsigned long tag = address >> (cache->setbit + cache->blockbit);
    unsigned int numbits = sizeof(unsigned long long) * 8;
    unsigned long indexset = (address << (numbits - cache->addressbit + cache->tagbit)) >> (numbits - cache->setbit);
    unsigned long blockbit = (address << (numbits - cache->blockbit)) >> (numbits - cache->blockbit);
    for (int i = 0; i < cache->linesset; i++) {
        if(cache->sets[indexset].lines[i].valid == 1 && cache->sets[indexset].lines[i].tag == tag) {
            cache->sets[indexset].lines[i].lasthit = currenttime;
            cache->sets[indexset].lines[i].use++;
            return cache->hitt;
        }
    }
    int freeindex = -1;
    for(int i = 0; i < cache->linesset; i++) {
        if (cache->sets[indexset].lines[i].valid != 1) {
            cache->sets[indexset].lines[i].tag = tag;
            cache->sets[indexset].lines[i].indexset = indexset;
            cache->sets[indexset].lines[i].valid= 1;
            cache->sets[indexset].lines[i].block = blockbit;
            cache->sets[indexset].lines[i].lasthit = currenttime;
            cache->sets[indexset].lines[i].use = 1;
            return cache->misst;
        }
    }
    if (strcmp(cache->replace, "LRU") == 0) {
        int oldesttime = cache->sets[indexset].lines[0].lasthit;
        unsigned int oldindex = 0;

        for (int i = 0; i < cache->linesset; i++) {
            if (cache->sets[indexset].lines[i].lasthit < oldesttime) {
                oldesttime = cache->sets[indexset].lines[i].lasthit;
                oldindex = i;
            }
        }
        cache->sets[indexset].lines[oldindex].tag = tag;
        cache->sets[indexset].lines[oldindex].indexset = indexset;
        cache->sets[indexset].lines[oldindex].valid = 1;
        cache->sets[indexset].lines[oldindex].block = blockbit;
        cache->sets[indexset].lines[oldindex].lasthit = currenttime;
        cache->sets[indexset].lines[oldindex].use = 1;
        return cache->misst;
    } else if (strcmp(cache->replace, "LFU") == 0) {
        int leastused = cache->sets[indexset].lines[0].use;
        unsigned int lowestindex = 0;

        for (int i = 0; i < cache->linesset; i++) {
            if (cache->sets[indexset].lines[i].use < leastused) {
                leastused = cache->sets[indexset].lines[i].use;
                lowestindex = i;
            }
        }
        cache->sets[indexset].lines[lowestindex].tag = tag;
        cache->sets[indexset].lines[lowestindex].indexset = indexset;
        cache->sets[indexset].lines[lowestindex].valid = 1;
        cache->sets[indexset].lines[lowestindex].lasthit = currenttime;
        cache->sets[indexset].lines[lowestindex].use = 1;
        return cache->misst;
    }
}

int main() {
    unsigned int numset;
    unsigned int linesset;
    unsigned int blocksize;
    unsigned int addressbit;
    unsigned int hitt;
    unsigned int misst;
    char replace[4];
    printf("Enter Input Block\n");
    scanf("%d", &numset);
    scanf("%d", &linesset);
    scanf("%d", &blocksize);
    scanf("%d\n", &addressbit);
    scanf("%c%c%c", &replace[0], &replace[1], &replace[2]);
    replace[3] = '\0';
    scanf("%d", &hitt);
    scanf("%d", &misst);
    Cache *cache = newcache(numset, linesset, blocksize, addressbit, replace, hitt,misst);
    unsigned int time = 0;
    unsigned int hits = 0;
    unsigned int misses = 0;
    int address;
    unsigned int addresswidth = addressbit / 4;
    scanf("%x", &address);
    printf("\nOutput Generated for the corresponding input block:- \n");
    while (address != -1) {
        unsigned int readtime = cacheread(address, cache, time);
        time += readtime;
        if (addresswidth == 2) {
            printf("%02x ", address);
        } else if (addresswidth == 4) {
            printf("%04x ", address);
        } else if (addresswidth == 8) {
            printf("%08x ", address);
        } else {
            printf("%08x ", address);
        }
        if (readtime == hitt) {
            printf("H\n");
            hits++;
        } else if (readtime == misst) {
            printf("M\n");
            misses++;
        }
        scanf("%x", &address);
    }
    printf("%d %d\n\n", (int)((double) misses / (hits+misses) * 100), hits * cache->hitt + misses * (cache->hitt + cache->misst));
    return 0;
}
