## Hashing functions analysis

### Description
To maximize performance of hash table it is important to implement a good hash function.
By *good* I mean function with the flattest distribution over hash table length.  

### Functions comparations
I created a small set of tools to compute hash values from words and generate *.csv* file and create a histogram.  
```
word-counter -q -d book.txt | ./main.out > comparation.csv
python3 plot.py comparation.csv
```
Where  
`word-counter` is a program that generates a list of words from a book without repetition ([word-counter](https://github.com/kolkil/word-counter)).  
`book.txt` is any book in raw *.txt* format (I used George Orwell's Year 1984, it hash 18444 unique words).  
`main.out` is a computing three different hash functions from words given on its stdin.

### Compared functions
```c
#define PRIME_LENGTH 1021

uint16_t hash_0(char *key) //default hash
{
    int len = strlen(key);
    uint32_t hash = 0;

    for (int i = 0; i < len; ++i)
        hash += key[i] * 10ul + i + 1;

    return hash % PRIME_LENGTH;
}

uint16_t hash_1(char *key) //some random function
{
    int len = strlen(key);
    uint32_t hash = 0;

    for (int i = 0; i < len; ++i)
        hash += (key[i] << 16) | ((key[i] << 8) + i + key[i]);

    return hash % PRIME_LENGTH;
}

uint16_t hash_2(char *key) //https://en.wikipedia.org/wiki/Jenkins_hash_function
{
    int len = strlen(key);
    size_t i = 0;
    uint32_t hash = 0;
    while (i != len)
    {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash % PRIME_LENGTH;
}
```

### The plot
![alt text](https://github.com/kolkil/jmcache/blob/dev/analysis/hashing_functions/plot.png "Logo Title Text 1")
### Summary
As we might expect, the [Jenkins hash function](https://en.wikipedia.org/wiki/Jenkins_hash_function) turned out to be the best
