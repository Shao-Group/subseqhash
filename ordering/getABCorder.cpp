#include <iostream>
#include <vector>
#include <set>
#include <chrono>
#include <climits>
#include <random>
#include <map>
#include <algorithm>


using namespace std;


random_device rd;

map<char, int> dict;

double A[100][4][100];
int B1[100][4][100];
int B2[100][4][100];
int C[100][4];


void init(int k, int d, string path)
{
	dict['A'] = 0;
	dict['C'] = 1;
	dict['G'] = 2;
	dict['T'] = 3;

	vector<int> sign;
	for(int i = 0; i < 4; i++)
		sign.push_back(i);

	vector<int> pos;
	for(int i = 0; i < d; i++)
		pos.push_back(i);

	unsigned seed;
  	default_random_engine generator(rd());
	uniform_real_distribution<double> distribution((int64_t)1<<30, (int64_t)1<<31);

	for(int i = 0; i < k; i++)
	{
		for(int j = 0; j < 4; j++)
			for(int q = 0; q < d; q++)
				A[i][j][q] = distribution(generator);


		seed = unsigned(chrono::system_clock::now().time_since_epoch().count());
		shuffle(pos.begin(), pos.end(), default_random_engine(seed));

		for(int j = 0; j < 4; j++)
			C[i][j] = pos[j];


		for(int q = 0; q < d; q++)
		{
			seed = unsigned(chrono::system_clock::now().time_since_epoch().count());
			shuffle(sign.begin(), sign.end(), default_random_engine(seed));
			for(int j = 0; j < 4; j++)
			{
				B1[i][j][q] = (sign[j] % 2) ? 1: -1;
				B2[i][j][q] = (sign[j] / 2) ? 1: -1;
			}
		}
	}

	string ss = path;
	FILE *file = fopen(ss.c_str(), "w");

	fprintf(file, "%d\n", d);


	for(int i = 0; i < k; i++)
	{
		fprintf(file, "%d \n", i);
		for(int q = 0; q < d; q++)
		{
			for(int j = 0; j < 4; j++)
				fprintf(file, "%.2lf ", A[i][j][q]);
			fprintf(file, "\n");
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\n");

	for(int i = 0; i < k; i++)
	{
		fprintf(file, "%d \n", i);
		for(int q = 0; q < d; q++)
		{
			for(int j = 0; j < 4; j++)
				fprintf(file, "%d,%d\t", B1[i][j][q], B2[i][j][q]);
			fprintf(file, "\n");
		}
		fprintf(file, "\n");	
	}
	fprintf(file, "\n");

	for(int i = 0; i < k; i++)
	{
		for(int j = 0; j < 4; j++)
			fprintf(file, "%d\t", C[i][j]);
		fprintf(file, "\n");	
	}
	fprintf(file, "\n");
}

string int2kmer(long long x, int k)
{
    string s;

    for(int i = 0; i < k; i++)
    {
        int y = x & 3;
        x = (x>>2);

        if(y == 0)
            s = 'A' + s;
        if(y == 1)
            s = 'C' + s;
        if(y == 2)
            s = 'G' + s;
        if(y == 3)
            s = 'T' + s;
    }

    return s;
}

pair<double, int> get(string s, int k, int d)
{
	double w = 0;
	int phi = 0;

	for(int i = 0; i < k; i++)
	{
		phi = (phi + C[i][dict[s[i]]]) % d;
		w = w * B1[i][dict[s[i]]][phi] + A[i][dict[s[i]]][phi] * B2[i][dict[s[i]]][phi];
	}

	return make_pair(w, phi);
}

bool cmp(pair<string, pair<double, int>> a, pair<string, pair<double, int>> b)
{
	return a.second.second < b.second.second || (a.second.second == b.second.second && fabs(a.second.first) > fabs(b.second.first));
}

vector<pair<string, pair<double, int>>> bmers;
int main(int argc, const char * argv[])
{
	int k = stoi(argv[1]);
	int d = stoi(argv[2]);
	int m = stoi(argv[3]);
	string save = argv[4];
	string savetable = argv[5];

	init(k, d, savetable);

	for(int i = 0; i < (1<<(2*k)); i++)
	{
		string s = int2kmer(i, k);
		pair<double, int> v = get(s, k, d);
		bmers.push_back(make_pair(s, v));
	}

	sort(bmers.begin(), bmers.end(), cmp);

	FILE *file = fopen(save.c_str(), "w");

	int num = 0;

	for(auto i: bmers)
	{
		num++;
		if(num > m)
			break;
		fprintf(file, "%s\t%.6lf\t%d\n", i.first.c_str(), i.second.first, i.second.second);
	}
	fclose(file);
	
	return 0;
}