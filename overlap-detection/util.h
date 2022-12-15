/*
  Utility functions for the overlap detection with SubseqHash project.

  By: Ke@PSU
  Last edited: 09/30/2022
*/

#ifndef _UTIL_H
#define _UTIL_H 1

#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>


/*
  Each k-mer is represented by a long unsigned int 
  with the encoding A-00, C-01, G-10, T-11.
*/
typedef __uint128_t kmer;


#define ALPHABETSIZE 4
const char ALPHABET[ALPHABETSIZE] = {'A', 'C', 'G', 'T'};

static inline int alphabetIndex(char c){
    return 3 & ((c>>2) ^ (c>>1));
}

/*
  Encode the string representation of a k-mer.
*/
kmer encode(const char* s, const int k);

/*
  Decode an k-mer into its string representation.
  If str is not null, it is used to store the resulting string; 
  otherwise a new char array is allocated.
*/
char* decode(const kmer enc, const int k, char* str);


/*
  Following definitions and functions are related to the dynamic programming 
  procedure for computing a smallest bmer out of a given kmer.
  See the manuscript for more details.
*/

#define MAXK 50
#define MAXB 50
// max value of p used in the subseqhash dynamic programming process
#define MAXP 100

/*
  A cell in the DP table
*/
typedef struct{
    double f_max;
    double f_min;
    char c_max[MAXB];
    char c_min[MAXB];
    bool h;
} DPCell;
//extern DPCell dp[MAXK][MAXB][MAXP];

/*
  The set of random tables determining a total order on all bmers.
*/
typedef struct{
    double word[MAXP];
    int sign;
    int sign1[MAXP];
    int sign2[MAXP];
} RandTableCell;
//extern RandTableCell table[MAXB][ALPHABETSIZE];

/*
  Initialize the random tables:
  --tp is RandTableCell[b][ALPHABETSIZE];
  --word[b][ALPHABETSIZE][p] take double values between 2^{30} and 2^{31};
  --sign[b][ALPHABETSIZE] take int values between 0 and p-1 such that 
  there is no repeat number within each sign[i] vector;
  --sign1[b][ALPHABETSIZE][p] and sign2[b][ALPHABETSIZE][p] take values +/-1
  such that (sign1[i][][j], sign[i][][j]) is a permutation of {(+1,+1), 
  (+1,-1), (-1,+1), (-1,-1)}.
 */
void initRandTable(const int b, const int p, void* tp);

/*
  Save/load a set of random tables to file.
  --tp is assumed to have been declared as RandTableCell[b][ALPHABETSIZE].
*/
void saveRandTable(const char* filename, const int b, const void* tp);
void loadRandTable(const char* filename, const int b, void* tp);
void printRandTable(const int b, const int p, const void* tp);


/*
  Given a char-representation of a kmer s and a set of random tables,
  return the smallest length b subsequence (as an encoded bmer) of s 
  according to the total order defined by the random tables.
  --tp is RandTableCell[b][ALPHABETSIZE];
  --dpp is DPCell[k+1][b+1][p].
*/
kmer minSubseq(const char* s, const int k, const int b, const int p,
	       const void* tp, void* dpp);

/*
  Given a char-representation of a read and a set of random tables, 
  calculate and store all of its minSubseq seeds in a map.
*/
void getSubseqSeeds(const std::string &read,
		    const int k, const int b, const int p,
		    const void* tp,
		    std::map<kmer, int> &seeds_tree);


/*
  Save seeds of a read to file, the map structure is discarded.
  The seeds are saved in ascending order of their encoding, the number
  of occurrences of a seed in the read is also stored.
*/
void saveSubseqSeeds(const char* filename, const std::map<kmer, int> seeds_tree);

/*
  Read seeds of a read from file, merge them into a map where the seed
  is key and value is a vector of read ids.
  The seed occurrence info is currently unused.
  ********
  This method is assumed to be called in ascending order of read_id so
  that each vector (associated to a seed) is in sorted order.
*/
void loadSubseqSeeds(const char* filename, const int read_id,
		     std::map<kmer, std::vector<int> > &all_seeds);


/*
  An upper diagonal matrix without the main diagonal,
  valid indices for Table::access(i, j) are 1<=i<j<=n
*/
class Table{
    size_t n;
    unsigned int* arr;

public:
    Table(size_t n);
    ~Table();
    unsigned int& access(size_t i, size_t j);
    void saveNoneZeroEntries(const char* filename);
};

#endif // util.h
