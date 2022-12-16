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
#include "seeding.h"

using namespace std;
random_device rd;

double f_max[100][100][101];
double f_min[100][100][101];
int g_max[100][100][101];
int g_min[100][100][101];
int h[100][100][101];

map<char, int> dict;

double word[100][4][100];
int sign[100][4];
int sign1[100][4][100];
int sign2[100][4][100];

int k, b, p;
string tablenum;

void init(string path)
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

	ifstream tin(path.c_str(), ios::binary|ios::in);

	for(int i = 0; i < b; i++)
		for(int j = 0; j < 4; j++)
			tin.read((char *) word[i][j], sizeof(double) * p);



	for(int i = 0; i < b; i++)
		for(int q = 0; q < p; q++)
			for(int j = 0; j < 4; j++)
				tin>>sign1[i][j][q]>>sign2[i][j][q];

	for(int i = 0; i < b; i++)
		for(int j = 0; j < 4; j++)
			tin>>sign[i][j];
}

ofstream out;
vector<seed> get_bmers(string s, int k, int b)
{
	int del = k - b;
	vector<seed> ret;
	int len = s.length();

	memset(h, 0, sizeof(h));


	f_max[0][0][0] = f_min[0][0][0] = 0;
	for(int i = 1; i <= k; i++)
	{
		f_max[i][0][0] = 0;
		f_min[i][0][0] = 0;
	}
	double v1, v2;

	for(int st = 0; st + k <= len; st++)
	{	
		int p0 = sign[0][dict[s[st]]];
		f_min[1][1][p0] = f_max[1][1][p0] = sign2[0][dict[s[st]]][p0] * word[0][dict[s[st]]][p0];
		g_min[1][1][p0] = g_max[1][1][p0] = 0;
		h[1][1][p0] = st + 1;

		for(int i = 0; i <= k; i++)
			h[i][0][0] = st+1;

		for(int i = 2; i <= k; i++)
		{
			int minj = max(1, i - del);
			int maxj = min(i, b);

			for(int j = minj; j <= maxj; j++)
			{			
				int now = dict[s[st + i - 1]];
				int v = sign[j-1][now];

				for(int k = 0; k < p; k++)
				{
					int z = (k + v) % p;

					if(h[i][j][z] < st + 1)
					{
						f_min[i][j][z] = 1e15;
						f_max[i][j][z] = -1e15;
					}

					if(h[i-1][j][z] == st + 1) //i not selected
					{	
						if(f_min[i-1][j][z] < f_min[i][j][z])
						{
							f_min[i][j][z] = f_min[i-1][j][z];
							g_min[i][j][z] = g_min[i-1][j][z];
						}
						
						if(f_max[i-1][j][z] > f_max[i][j][z])
						{
							f_max[i][j][z] = f_max[i-1][j][z];
							g_max[i][j][z] = g_max[i-1][j][z];
						}
						
						h[i][j][z] = st + 1;
					}

					if(h[i-1][j-1][k] == st + 1) // select i
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
							{
								f_max[i][j][z] = v1;
								g_max[i][j][z] = i - 1;
							}
							if(v2 < f_min[i][j][z])
							{
								f_min[i][j][z] = v2;
								g_min[i][j][z] = i - 1;
							}
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
							{
								f_min[i][j][z] = v1;
								g_min[i][j][z] = i - 1;
							}
							if(v2 > f_max[i][j][z])
							{
								f_max[i][j][z] = v2;
								g_max[i][j][z] = i - 1;
							}
						}

						h[i][j][z] = st + 1;
					}

					// if(g_max[i][j][z] == i || g_min[i][j][z] == i)
					// 	cout<<i<<" "<<j<<" "<<z<<" "<<h[i][j][z]<<" "<<f_min[i][j][z]<<" "<<f_max[i][j][z]<<endl;
				}
			}
		}

		seed tmp;
		int mod;

		for(int i = 0; i < p; i++)
			if(h[k][b][i] == st + 1)
			{
				if(fabs(f_min[k][b][i]) > f_max[k][b][i])
					tmp.hashval = f_min[k][b][i];
				else
					tmp.hashval = f_max[k][b][i];

				mod = i;
				break;
			}
		
		if(ret.size() > 0 && tmp.hashval == ret.back().hashval)
		{
			ret.back().ken = st + k - 1;
			continue;
		}

		int x = k;
		int y = b;

		bool z = 1;
		if(tmp.hashval > 0)
			z = 0;
		int mod1;

		while(y > 0)
		{
			if(z == 1)
			{
				tmp.index.push_back(g_min[x][y][mod] + st);

				if(sign1[y-1][dict[s[g_min[x][y][mod] + st]]][mod] == -1)
					z = 0;
				
				mod1 = (mod + p - sign[y-1][dict[s[g_min[x][y][mod] + st]]]) % p;
				x = g_min[x][y][mod];
				y--;
				mod = mod1;
			}
			else
			{
				tmp.index.push_back(g_max[x][y][mod] + st);

				if(sign1[y-1][dict[s[g_max[x][y][mod] + st]]][mod] == -1)
					z = 1;

				mod1 = (mod + p - sign[y-1][dict[s[g_max[x][y][mod] + st]]]) % p;
				x = g_max[x][y][mod];
				y--;
				mod = mod1;
			}
		}
		tmp.st = tmp.index.back();
		tmp.en = tmp.index.front();

		tmp.kst = st;
		tmp.ken = st + k - 1;

		ret.push_back(tmp);
	}

	for(seed tmp: ret)
	{
		out<<setprecision(30)<<tmp.hashval<<" ";
		for(int i = b-1; i >= 0; i--)
			out<<tmp.index[i]<<" ";
		out<<endl;
	}

	return ret;
}
string refseq = "";

int main(int argc, const char * argv[])
{
	k = stoi(argv[1]);
	b = stoi(argv[2]);
	p = stoi(argv[3]);
	string refgenome = argv[4];
	string tablepath = argv[5];
	tablenum = argv[6];
	string outputpath = argv[7];

	init(tablepath + "/" + tablenum);

	ifstream refin(refgenome.c_str());
	string info, seq;
	while(getline(refin, info))
	{
		getline(refin, refseq);

		string path = outputpath  + "/" + info + "_" + tablenum;
		out.open(path.c_str());
		get_bmers(refseq, k, b);

		out.close();
		out.clear();
	}
	return 0;
}