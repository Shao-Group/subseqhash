#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <chrono>
#include <climits>
#include <map>
#include <cstring>
#include <unordered_set>

using namespace std;
random_device rd;
map<char, int> dict;

double word[100][4][100];
int sign[100][4];
int sign1[100][4][100];
int sign2[100][4][100];

void init(int blen, int p, string path)
{
	dict['A'] = 0;
	dict['C'] = 1;
	dict['G'] = 2;
	dict['T'] = 3;

	vector<int> pos;
	vector<int> possign;

	for(int i = 0; i < 4; i++)
		possign.push_back(i);

	for(int i = 0; i < p; i++)
		pos.push_back(i);

	unsigned seed;
  	default_random_engine generator(rd());
	uniform_real_distribution<double> distribution((int64_t)1<<30, (int64_t)1<<31);


	for(int i = 0; i < blen; i++)
	{
		for(int j = 0; j < 4; j++)
			for(int q = 0; q < p; q++)
				word[i][j][q] = distribution(generator);

		seed = unsigned(chrono::system_clock::now().time_since_epoch().count());
		shuffle(pos.begin(), pos.end(), default_random_engine(seed));

		for(int j = 0; j < 4; j++)
			sign[i][j] = pos[j];

		for(int q = 0; q < p; q++)
		{
			seed = unsigned(chrono::system_clock::now().time_since_epoch().count());
			shuffle(possign.begin(), possign.end(), default_random_engine(seed));

			for(int j = 0; j < 4; j++)
			{
				sign1[i][j][q] = (possign[j] % 2) ? 1: -1;
				sign2[i][j][q] = (possign[j] / 2) ? 1: -1;
			}
		}
	}

	ofstream tout(path.c_str(), ios::binary|ios::out);

	for(int i = 0; i < blen; i++)
		for(int j = 0; j < 4; j++)
			tout.write((const char*)word[i][j], sizeof(double) * p);
		
	tout<<endl;

	for(int i = 0; i < blen; i++)
	{
		for(int q = 0; q < p; q++)
		{
			for(int j = 0; j < 4; j++)
				tout<<sign1[i][j][q]<<" "<<sign2[i][j][q]<<" ";
			tout<<endl;
		}
		tout<<endl;
	}
	tout<<endl;

	for(int i = 0; i < blen; i++)
	{
		for(int j = 0; j < 4; j++)
			tout<<sign[i][j]<<" ";
		tout<<endl;
	}
	tout<<endl;
}

int main(int argc, const char * argv[])
{
	int b, p, num;
	string path;

	b = stoi(argv[1]);
	p = stoi(argv[2]);
	num = stoi(argv[3]);
	path = argv[4];
	
	for(int i = 1; i <= num; i++)
		init(b, p, path + '/' + to_string(i));
	return 0;
}