#include "util.h"

kmer encode(const char* s, const int k){
    kmer enc = 0lu;
    int i;
    for(i=0; i<k; i+=1){
	enc = (enc << 2) | alphabetIndex(s[i]);
    }
    return enc;
}

char* decode(const kmer enc, const int k, char* str){
    if(str == NULL){
	str = (char*)malloc(sizeof *str *k);
    }
    kmer enc_copy = enc;
    int i;
    for(i=k-1; i>=0; i-=1){
	str[i] = ALPHABET[enc_copy & 3];
	enc_copy >>= 2;
    }
    return str;
}


void initRandTable(const int b, const int p, void *tp){
    RandTableCell (*table)[ALPHABETSIZE] = (RandTableCell (*)[ALPHABETSIZE])tp;
    std::random_device rd;
    std::vector<int> pos;
    std::vector<int> possign;

    int i, j, q;
    for(i=0; i<ALPHABETSIZE; i+=1){
	possign.push_back(i);
    }
    for(i=0; i<p; i+=1){
	pos.push_back(i);
    }

    unsigned seed;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution((int64_t)1<<30, (int64_t)1<<31);

    for(i=0; i<b; i+=1)
    {
	for(j=0; j<ALPHABETSIZE; j+=1){
	    for(q=0; q<p; q+=1){
		table[i][j].word[q] = distribution(generator);
	    }
	}


	seed = unsigned(std::chrono::system_clock::now().time_since_epoch().count());
	shuffle(pos.begin(), pos.end(), std::default_random_engine(seed));

	for(j=0; j<ALPHABETSIZE; j+=1){
	    table[i][j].sign = pos[j];
	}


	for(q=0; q<p; q+=1)
	{
	    seed = unsigned(std::chrono::system_clock::now().time_since_epoch().count());
	    std::shuffle(possign.begin(), possign.end(), std::default_random_engine(seed));
	    for(j=0; j<ALPHABETSIZE; j+=1)
	    {
		table[i][j].sign1[q] = (possign[j] % 2) ? 1: -1;
		table[i][j].sign2[q] = (possign[j] / 2) ? 1: -1;
	    }
	}
    }
}

kmer minSubseq(const char* s, const int k, const int b, const int p,
	       const void* tp, void* dpp){
    RandTableCell (*table)[ALPHABETSIZE] = (RandTableCell (*)[ALPHABETSIZE])tp;
    DPCell (*dp)[b+1][p] = (DPCell (*)[b+1][p]) dpp;
    int del = k-b, i, j, q;    
    
    memset(dp, 0, sizeof(DPCell)*(k+1)*(b+1)*p);

    //double init = 0;

    for(i=0; i<=k; i+=1){
	//dp[i][0][0].f_max = dp[i][0][0].f_min = init;
	dp[i][0][0].h = true;
    }

    q = alphabetIndex(s[0]);
    DPCell &cur = dp[1][1][table[0][q].sign];
    cur.f_min = cur.f_max = //init * table[0][q].sign1[table[0][q].sign] + 
	table[0][q].sign2[table[0][q].sign]
	* table[0][q].word[table[0][q].sign];
    cur.c_min[0] = cur.c_max[0] = s[0];
    cur.h = true;

    double v1, v2;
    int minj, maxj, now, v, z;
    for(i=2; i<=k; i+=1)
    {
	minj = std::max(1, i - del);
	maxj = std::min(i, b);

	for(j=minj; j<=maxj; j+=1)
	{
	    now = alphabetIndex(s[i - 1]);
	    v = table[j-1][now].sign;

	    for(q=0; q<p; q+=1)
	    {
		z = (q + v) % p;
		if(!dp[i][j][z].h){
		    dp[i][j][z].f_min = 1e15;
		    dp[i][j][z].f_max = -1e15;
		}
		
		if(dp[i-1][j][z].h)
		{
		    if(dp[i][j][z].f_min > dp[i-1][j][z].f_min){
			dp[i][j][z].f_min = dp[i-1][j][z].f_min;
			memcpy(dp[i][j][z].c_min, dp[i-1][j][z].c_min, j);
		    }
		    if(dp[i][j][z].f_max < dp[i-1][j][z].f_max){
			dp[i][j][z].f_max = dp[i-1][j][z].f_max;
			memcpy(dp[i][j][z].c_max, dp[i-1][j][z].c_max, j);
		    }
		    dp[i][j][z].h = true;
		}

		if(dp[i-1][j-1][q].h)
		{
		    // v1 = dp[i-1][j-1][q].f_min * table[j-1][now].sign1[z]
		    // 	+ table[j-1][now].word[z] * table[j-1][now].sign2[z];
		    // v2 = dp[i-1][j-1][q].f_max * table[j-1][now].sign1[z]
		    // 	+ table[j-1][now].word[z] * table[j-1][now].sign2[z];

		    if(table[j-1][now].sign1[z] < 0){
			v1 = -dp[i-1][j-1][q].f_min;
			v2 = -dp[i-1][j-1][q].f_max;
		    }else{
			v1 = dp[i-1][j-1][q].f_min;
			v2 = dp[i-1][j-1][q].f_max;
		    }

		    if(table[j-1][now].sign2[z] < 0){
			v1 -= table[j-1][now].word[z];
			v2 -= table[j-1][now].word[z];
		    }else{
			v1 += table[j-1][now].word[z];
			v2 += table[j-1][now].word[z];
		    }
		    

		    if(v1 < v2){
			if(v1 < dp[i][j][z].f_min){
			    dp[i][j][z].f_min = v1;
			    memcpy(dp[i][j][z].c_min, dp[i-1][j-1][q].c_min, j-1);
			    dp[i][j][z].c_min[j-1] = s[i-1];
			}
			if(v2 > dp[i][j][z].f_max){
			    dp[i][j][z].f_max = v2;
			    memcpy(dp[i][j][z].c_max, dp[i-1][j-1][q].c_max, j-1);
			    dp[i][j][z].c_max[j-1] = s[i-1];
			}
		    }else{
			if(v2 < dp[i][j][z].f_min){
			    dp[i][j][z].f_min = v2;
			    memcpy(dp[i][j][z].c_min, dp[i-1][j-1][q].c_max, j-1);
			    dp[i][j][z].c_min[j-1] = s[i-1];
			}
			if(v1 > dp[i][j][z].f_max){
			    dp[i][j][z].f_max = v1;
			    memcpy(dp[i][j][z].c_max, dp[i-1][j-1][q].c_min, j-1);
			    dp[i][j][z].c_max[j-1] = s[i-1];
			}
		    }
		    
		    dp[i][j][z].h = true;
		}
	    }
	}
    }

    double ans = 0;
    char seed[b];
    for(i=0; i<p; ++i)
    {
	if(dp[k][b][i].h){
	    //ans = max(ans, fabs(f_min[k][b][i]));
	    if(ans < fabs(dp[k][b][i].f_min)){
		ans = fabs(dp[k][b][i].f_min);
		memcpy(seed, dp[k][b][i].c_min, b);
	    }

	    //ans = max(ans, f_max[k][b][i]);
	    if(ans < dp[k][b][i].f_max){
		ans = dp[k][b][i].f_max;
		memcpy(seed, dp[k][b][i].c_max, b);
	    }


	    if(ans > 0){
		//return ans;

		//test if recorded seed is correct
		/*
		  double v = 0;
		  int angle = 0;
		  for(j=0; j<b; j+=1){
		  now = alphabetIndex(seed[j]);
		  v = (v + table[j][now].word[angle] * table[j][now].sign1[angle]) * table[j][now].sign2[angle];
		  angle = (angle + table[j][now].sign) % p;
		  }
		  v = fabs(v);
		  if(ans != v){
		  printf("%.*s %.*s %.10f %.10f\n", k, s, b, seed, ans, v);
		  }
		*/
		return encode(seed, b);
	    }
	}
    }

    return encode(seed, b);
}

void getSubseqSeeds(const std::string &read,
		    const int k, const int b, const int p,
		    const void* tp,
		    std::map<kmer, int> &seeds_tree){
    int len = read.length();
    int i;
    char cur[k];
    kmer seed;
    DPCell dp[k+1][b+1][p];

    for(i=0; i<=len-k; i+=1){
	read.copy(cur, k, i);
	seed = minSubseq(cur, k, b, p, tp, dp);

	auto result = seeds_tree.insert(std::pair<kmer, int>(seed, 1));
	if(result.second == false){
	    result.first->second += 1;
	}
    }	
}

void saveRandTable(const char* filename, const int b, const void* tp){
    FILE* fout = fopen(filename, "wb");
    fwrite(tp, sizeof(RandTableCell), b * ALPHABETSIZE, fout);
    fclose(fout);
}

void loadRandTable(const char* filename, const int b, void* tp){
    FILE* fin = fopen(filename, "rb");
    size_t expected = b * ALPHABETSIZE;
    size_t ret = fread(tp, sizeof(RandTableCell), expected, fin);
    if(ret != expected){
	if(feof(fin)) fprintf(stderr, "Rand tables in %s are too small\n", filename);
	else if(ferror(fin)) fprintf(stderr, "Error reading %s\n", filename);
    }
    fclose(fin);
}

void printRandTable(const int b, const int p, const void* tp){
    RandTableCell (*table)[ALPHABETSIZE] = (RandTableCell (*)[ALPHABETSIZE])tp;
    int i, j, q;
    for(q=0; q<p; q+=1){
	printf("p=%d\n", q);
	for(i=0; i<b; i+=1){
	    for(j=0; j<ALPHABETSIZE; j+=1){
		printf("%.6lf ", table[i][j].word[q]);
	    }
	    printf("\n");
	}
	printf("\n");

	for(i=0; i<b; i+=1){
	    for(j=0; j<ALPHABETSIZE; j+=1){
		printf("%+d,%+d ", table[i][j].sign1[q], table[i][j].sign2[q]);
	    }
	    printf("\n");
	}
	printf("\n");

	for(i=0; i<b; i+=1){
	    for(j=0; j<ALPHABETSIZE; j+=1){
		printf("%d ", table[i][j].sign);
	    }
	    printf("\n");
	}
	printf("\n");
    }
}

void saveSubseqSeeds(const char* filename, const std::map<kmer, int> seeds_tree){
    FILE* fout = fopen(filename, "wb");
    for(std::pair<kmer, int> seed : seeds_tree){
	fwrite(&(seed.first), sizeof(kmer), 1, fout);
	fwrite(&(seed.second), sizeof(int), 1, fout);
    }
    fclose(fout);
}

void loadSubseqSeeds(const char* filename, const int read_id,
		     std::map<kmer, std::vector<int> > &all_seeds){
    FILE* fin = fopen(filename, "rb");
    size_t ret = 1;
    kmer seed;
    int ct;
    while(ret == 1){
	ret = fread(&seed, sizeof(kmer), 1, fin);
	fread(&ct, sizeof(int), 1, fin);
	auto result = all_seeds.insert(std::pair<kmer, std::vector<int> >(seed, std::vector<int>(1, read_id)));
	if(result.second == false && result.first->second.back() < read_id){
	    result.first->second.push_back(read_id);
	}
    }
    if(ferror(fin)){
	fprintf(stderr, "Error reading %s\n", filename);
    }
    fclose(fin);
}

Table::Table(size_t n): n(n){
    size_t size = (n*(n-1))>>1;
    arr = new unsigned int[size];
    memset(arr, 0, sizeof *arr * size);
}

Table::~Table(){
    delete[] arr;
}

unsigned int& Table::access(size_t i, size_t j){
    return arr[((((n<<1)-i)*(i-1))>>1)+j-i-1];
}

void Table::saveNoneZeroEntries(const char* filename){
    FILE* fout = fopen(filename, "w");

    size_t i, j, k, size=(n*(n-1))>>1;
    for(k=0, i=1, j=2; k<size; ++k){
	if(arr[k] > 0){
	    fprintf(fout, "%zu %zu %u\n", i, j, arr[k]);
	}
	j += 1;
	if(j > n){
	    i += 1;
	    j = i + 1;
	}
    }
    
    fclose(fout);
}
