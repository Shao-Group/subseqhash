/*
  Given a file of reads in fasta format, extract all kmers of each read as seeds.
  Output pairs of reads together with the number of unique seeds they share.
  
  By: Ke@PSU
  Last edited: 10/20/2022
*/

#include "util.h"
#include <iostream>
#include <fstream>

using namespace std;

void getAllKMers(const string &read, const size_t read_idx, const int k,
		 map<kmer, vector<size_t> > &all_seeds){
    int len = read.length();
    int i;
    char cur[k];
    kmer seed;
    for(i=0; i<=len-k; ++i){
	read.copy(cur, k, i);
	seed = encode(cur, k);
	auto result = all_seeds.insert(pair<kmer, vector<size_t> >(seed, vector<size_t>(1, read_idx)));
	if(result.second == false && result.first->second.back() < read_idx){
	    result.first->second.push_back(read_idx);
	}
    }
}

int main(int argc, const char * argv[])
{    
    if(argc != 3){
	printf("usage: bucketByKMer.out readFile k\n");
	return 1;
    }

    //char *reads_file = argv[1];
    int k = atoi(argv[2]);

    ifstream fin(argv[1], ifstream::in);

    string read;
    size_t read_idx = 0;
    map<kmer, vector<size_t>> all_seeds;

    while(fin.get() == '>'){
	//skip the header
	fin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(fin, read);
	++ read_idx;
	getAllKMers(read, read_idx, k, all_seeds);
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
    file_len = sprintf(output_file, "%.*s-bucketKMer-k%d.all-pair",
		       file_len, argv[1], k);
    share_ct.saveNoneZeroEntries(output_file);
    
    return 0;
}
