/*
  Given a set of seed files (readable by loadSubseqSeeds), output pairs of reads
  together with the number of unique seeds they share.
  
  By: Ke@PSU
  Last edited: 10/01/2022
*/

#include "util.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, const char * argv[])    
{   
    if(argc != 3){
	printf("usage: bucketBySeeds.out seedsDir numFiles\n");
	return 1;
    }

    int n = atoi(argv[2]);
    //int threshold = atoi(argv[3]);


    char filename[500];
    int i = strlen(argv[1]);
    memcpy(filename, argv[1], i);
    if(filename[i-1] != '/'){
	filename[i] = '/';
	++i;
    }
    
    map<kmer, vector<int> > all_seeds;
    int j;
    
    struct stat test_file;
    for(j=1; j<=n; j+=1){
	sprintf(filename+i, "%d.subseqseed", j);
	if(stat(filename, &test_file) != 0){//seed file does not exist
	    fprintf(stderr, "Stopped, cannot find file %d.subseqseed\n", j);
	    break;
	}
	loadSubseqSeeds(filename, j, all_seeds);
    }

    sprintf(filename+i, "bucket-n%d.all-pair", n);

    Table share_ct(n);

    int a, b, c;
    
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

    share_ct.saveNoneZeroEntries(filename);
    
    return 0;
}
