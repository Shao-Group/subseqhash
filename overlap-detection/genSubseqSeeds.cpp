/*
  Given a fasta read file, for each read, generate subseq seeds and save them
  in a file. The seeds are generated with parameters n, k, d, and a set of
  random tables. The given table is loaded if it exists, otherwise a new set
  of random tables is generated and saved with the provided name. (See the 
  manuscript for more info on the algorithm of generating subseq seeds).

  The seeds are generated in parallel with NUMTHREADS threads.

  By: Ke@PSU
  Last edited: 10/01/2022
*/

#include "util.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>

#define NUMTHREADS 1

using namespace std;

struct Read{
    string seq;
    size_t idx;

    Read(string&& s, size_t i): seq(s), idx(i){}
};

class SeedFactory{
    const int k;
    const int b;
    const int p;
    const void* table;
    const char* output_dir;
    const int dir_len;
    
    queue<Read> jobs;
    vector<thread> minions;
    bool done;
    mutex door;
    condition_variable trumpet;

    void getAndSaveSubseqSeeds(const Read &r){
	map<kmer, int> seeds_tree;
	getSubseqSeeds(r.seq, k, b, p, table, seeds_tree);

	char output_filename[200];
	sprintf(output_filename, "%.*s/%zu.subseqseed",
		dir_len, output_dir, r.idx);
	
	saveSubseqSeeds(output_filename, seeds_tree);
    }

    void atWork(int x){
	unique_lock<mutex> lock(door, defer_lock);
	while(true){
	    lock.lock();
	    while(!done && jobs.empty()){
		trumpet.wait(lock);
	    }
	    if(!jobs.empty()){
		Read r = move(jobs.front());
		jobs.pop();
		lock.unlock();
		getAndSaveSubseqSeeds(r);
	    }else{
		return;
	    }
	}
    }

public:
    SeedFactory(int k, int b, int p, const void* table,
		const char* output_dir, int dir_len):
	k(k), b(b), p(p), table(table),
	output_dir(output_dir), dir_len(dir_len),
	done(false){
	
	minions.reserve(NUMTHREADS);
	for(int i=0; i<NUMTHREADS; ++i){
	    minions.emplace_back(bind(&SeedFactory::atWork, this, i));
	}
    }

    ~SeedFactory(){
	unique_lock<mutex> lock(door);
	done = true;
	lock.unlock();
	trumpet.notify_all();

	for(auto& x : minions){
	    x.join();
	}
    }

    void addJob(Read&& r){
	unique_lock<mutex> lock(door);
	jobs.emplace(r);
	trumpet.notify_one();
    }
};

int main(int argc, const char * argv[])
{    
    if(argc != 6){
	printf("usage: genSubseqSeeds.out readFile n k d randTableFile\n");
	return 1;
    }

    //srand(time(0));
    //char *reads_file = argv[1];
    int k = atoi(argv[2]);
    int b = atoi(argv[3]);
    int p = atoi(argv[4]);

    RandTableCell table[b][ALPHABETSIZE];
    char table_filename[200];
    sprintf(table_filename, "%s-n%d-k%d-d%d", argv[5], k, b, p);

    struct stat test_table;
    if(stat(table_filename, &test_table) == 0){//file exists
	loadRandTable(table_filename, b, table);
    }else{
	initRandTable(b, p, table);
	saveRandTable(table_filename, b, table);
    }

    ifstream fin(argv[1], ifstream::in);

    char output_dir[200];
    int dir_len = strstr(argv[1], ".efa") - argv[1];
    int tablename_st = strlen(argv[5]) - 1;
    for(; tablename_st>=0; --tablename_st){
	if(argv[5][tablename_st]=='/'){
	    break;
	}
    }
    ++tablename_st;
    dir_len = sprintf(output_dir, "%.*s-seeds-%s",
		      dir_len, argv[1], table_filename+tablename_st);
    mkdir(output_dir, 0744);

    SeedFactory factory(k, b, p, table, output_dir, dir_len);

    string read;
    size_t read_idx = 0;

    while(fin.get() == '>'){
	//skip the header
	fin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(fin, read);
	++ read_idx;
	factory.addJob(Read(move(read), read_idx));
    }
    
    //printf("%s %s %d %d %d %s done\n", argv[0], argv[1], k, b, p, argv[5]);
    return 0;
}
