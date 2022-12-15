#include <iostream>
#include <vector>
#include <set>
#include <chrono>
#include <climits>
#include <random>
#include <fstream>
#include <map>
#include <cstring>
#include <cstdio>
#include <algorithm>


using namespace std;

random_device rd;

double f_max[100][100][101];
double f_min[100][100][101];
bool h[100][100][101];

map<char, int> dict;


double word[100][4][100];
int sign[100][4];
int sign1[100][4][100];
int sign2[100][4][100];

int p;

void init(int blen, int r)
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
}

double DP(int blen, string s)
{
	int len = s.length();
	int del = len - blen;

	memset(h, 0, sizeof(h));

	double v = 0;

	f_max[0][0][0] = f_min[0][0][0] = v;
	h[0][0][0] = 1;
	for(int i = 1; i <= len; i++)
	{
		f_max[i][0][0] = v;
		f_min[i][0][0] = v;
		h[i][0][0] = 1;
	}

	f_min[1][1][sign[0][dict[s[0]]]] = f_max[1][1][sign[0][dict[s[0]]]] = v * sign1[0][dict[s[0]]][sign[0][dict[s[0]]]] + sign2[0][dict[s[0]]][sign[0][dict[s[0]]]] * word[0][dict[s[0]]][sign[0][dict[s[0]]]];
	h[1][1][sign[0][dict[s[0]]]] = 1;

	double v1, v2, v3, v4;
	for(int i = 2; i <= len; i++)
	{
		int minj = max(1, i - del);
		int maxj = min(i, blen);

		for(int j = minj; j <= maxj; j++)
		{
			int now = dict[s[i - 1]];
			int v = sign[j-1][now];

			for(int k = 0; k < p; k++)
			{
				int z = (k + v) % p;
				if(h[i][j][z] == 0)
				{
					f_min[i][j][z] = 1e15;
					f_max[i][j][z] = -1e15;
				}

				if(h[i-1][j][z] == 1)
				{
					f_min[i][j][z] = min(f_min[i][j][z], f_min[i-1][j][z]);
					f_max[i][j][z] = max(f_max[i][j][z], f_max[i-1][j][z]);
					h[i][j][z] = 1;
				}

				if(h[i-1][j-1][k])
				{
					if(sign1[j-1][now][z] == -1)
					{
						v1 = -f_min[i-1][j-1][k];
						v2 = -f_max[i-1][j-1][k];

						if(sign2[j-1][now][z] == -1)
						{
							v1 -= word[j-1][now][z];
							v2 -= word[j-1][now][z];
						}
						else
						{
							v1 += word[j-1][now][z];
							v2 += word[j-1][now][z];
						}

						if(v1 > f_max[i][j][z])
							f_max[i][j][z] = v1;
						if(v2 < f_min[i][j][z])
							f_min[i][j][z] = v2;
					}

					else
					{
						v1 = f_min[i-1][j-1][k];
						v2 = f_max[i-1][j-1][k];

						if(sign2[j-1][now][z] == -1)
						{
							v1 -= word[j-1][now][z];
							v2 -= word[j-1][now][z];
						}
						else
						{
							v1 += word[j-1][now][z];
							v2 += word[j-1][now][z];
						}

						if(v1 < f_min[i][j][z])
							f_min[i][j][z] = v1;
						if(v2 > f_max[i][j][z])
							f_max[i][j][z] = v2;
					}

					h[i][j][z] = 1;
				}
			}
		}
	}

	double ans = 0;
	for(int i = 0; i < p; i++)
		if(h[len][blen][i])
		{
			ans = max(ans, fabs(f_min[len][blen][i]));
			ans = max(ans, f_max[len][blen][i]);

			if(ans > 0)
				return ans;
		}

	return ans;
}


int tp = 0;
int skip = 0;

void match(string s, string t, int b)
{
	if(b > t.length())
	{
		skip++;
		return;
	}
	
	double v1 = DP(b, s);
	double v2 = DP(b, t);

	//cout<<v1<<" "<<v2<<endl;
	if(v1 == v2)
		tp ++;
}


int main(int argc, const char * argv[])
{
	int b = stoi(argv[1]);
	p = stoi(argv[2]);

	string s, t;
	int n = 0;
	init(b, 0);

	ifstream inFile;



	string fname = argv[3];	
	inFile.open(fname.c_str());

	while(getline(inFile, s))
	{
		getline(inFile, t);

		match(s, t, b);
		n++;
	}
	printf("%.2lf, %d, %d, %d\n", tp * 100.0 / (n-skip), tp, n - skip, skip);


	return 0;
}