/*
  Given a file of reads in fasta format, extract all minimizers of each read as seeds.
  Output pairs of reads together with the number of unique seeds they share.
  
  By: Ke@PSU
  Last edited: 11/17/2022
*/

#include "util.h"
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

static inline uint64_t hash64(kmer s, uint64_t mask)
{
    uint64_t key = (uint64_t) s;
    key = (~key + (key << 21)) & mask; // key = (key << 21) - key - 1;
    key = key ^ key >> 24;
    key = ((key + (key << 3)) + (key << 8)) & mask; // key * 265
    key = key ^ key >> 14;
    key = ((key + (key << 2)) + (key << 4)) & mask; // key * 21
    key = key ^ key >> 28;
    key = (key + (key << 31)) & mask;
    return key;
}

struct MMEntry{
    int ed;
    uint64_t hash;
    kmer seed;
    int ct; //number of consecutive windows that have this kmer as minimizer
    MMEntry(int ed, uint64_t hash, kmer seed): ed(ed), hash(hash), seed(seed), ct(0) {}
};

static inline void addSeedToTree(map<kmer, vector<size_t> > &all_seeds,
				 const size_t read_idx, kmer seed){
    auto result = all_seeds.insert(pair<kmer, vector<size_t> >(seed, vector<size_t>(1, read_idx)));
    if(result.second == false && result.first->second.back() < read_idx){
	result.first->second.push_back(read_idx);
    }
}

static inline void addSeedToCandidates(list<MMEntry> &candidates,
				       int ed, int min_ed,
				       kmer seed, uint64_t mask,
				       const size_t read_idx,
				       map<kmer, vector<size_t> > &all_seeds){
    if(!candidates.empty()){
	MMEntry &first = candidates.front();
	if(first.ed < min_ed){//out of window
	    addSeedToTree(all_seeds, read_idx, first.seed);
	    candidates.pop_front();
	}
    }
    uint64_t hash = hash64(seed, mask);
    MMEntry cur(ed, hash, seed);
    while(!candidates.empty()){
	MMEntry &last = candidates.back();
	if(last.hash >= hash){
	    if(last.ct > 0){//is minimizer for some previous windows
		addSeedToTree(all_seeds, read_idx, last.seed);
	    }
	    candidates.pop_back();
	}else break;
    }

    candidates.push_back(cur);
}

void getAllMinimizers(const string &read, const size_t read_idx, const int w,
		      const int k, map<kmer, vector<size_t> > &all_seeds){
    kmer mask = (1ULL<<(k<<1)) - 1;
    int len = read.length();
    int i;
    char cur[k];
    kmer seed;
    list<MMEntry> candidates;
    int j = len - k + 1;
    j = (j < w ? j : w);
    j += k-1;
    
    //first window
    read.copy(cur, k, 0);
    seed = encode(cur, k);
    addSeedToCandidates(candidates, k-1, k-1, seed, (uint64_t)mask, read_idx, all_seeds);
    for(i=k; i<j; ++i){
	seed = (seed << 2) & mask; 
	seed |= alphabetIndex(read[i]);
	addSeedToCandidates(candidates, i, k-1, seed, (uint64_t)mask, read_idx, all_seeds);
    }
    candidates.front().ct = 1;//minimizer of first window
    
    for(i=j; i<len; ++i){
	j = i - w + 1;
	seed = (seed << 2) & mask; 
	seed |= alphabetIndex(read[i]);
	addSeedToCandidates(candidates, i, j, seed, (uint64_t)mask, read_idx, all_seeds);
	candidates.front().ct += 1;
    }

    addSeedToTree(all_seeds, read_idx, candidates.front().seed);
}

int main(int argc, const char * argv[])
{    
    if(argc != 4){
	printf("usage: bucketByMinimizer.out readFile w k\n");
	return 1;
    }

    //char *reads_file = argv[1];
    int w = atoi(argv[2]);
    int k = atoi(argv[3]);

    ifstream fin(argv[1], ifstream::in);

    string read;
    size_t read_idx = 0;
    map<kmer, vector<size_t>> all_seeds;

    while(fin.get() == '>'){
	//skip the header
	fin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(fin, read);
	++ read_idx;
	getAllMinimizers(read, read_idx, w, k, all_seeds);
    }

    Table share_ct(read_idx);
    int a, b, c, i, j;
    
    for(auto& seed : all_seeds){
	c = seed.second.size();
	for(i=0; i<c; ++i){
	    a = seed.second[i];
	    for(j=i+1; j<c; ++j){
		b = seed.second[j];
		++ share_ct.access(a, b);
		/*
		if(share_ct.access(a, b) == threshold){
		    fprintf(fout, "%d %d\n", a, b);
		}
		*/
	    }
	}
    }

    char output_file[200];
    int file_len = strstr(argv[1], ".efa") - argv[1];
    file_len = sprintf(output_file, "%.*s-bucketMinimizer-w%d-k%d.all-pair",
		       file_len, argv[1], w, k);
    share_ct.saveNoneZeroEntries(output_file);
    
    return 0;
}
