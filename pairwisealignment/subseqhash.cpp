#include <iostream>
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

random_device rd;

double f_max[100][100][101];
double f_min[100][100][101];
int g_max[100][100][101];
int g_min[100][100][101];
int h[100][100][101];

map<char, int> dict;

double word[101][100][4][100];
int sign[101][100][4];
int sign1[101][100][4][100];
int sign2[101][100][4][100];

int k, b, p;

void init(int blen)
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


	for(int r0 = 0; r0 < 100; r0++)
		for(int i = 0; i < blen; i++)
		{
			for(int j = 0; j < 4; j++)
				for(int q = 0; q < p; q++)
					word[r0][i][j][q] = distribution(generator);

			seed = unsigned(chrono::system_clock::now().time_since_epoch().count());
			shuffle(pos.begin(), pos.end(), default_random_engine(seed));

			for(int j = 0; j < 4; j++)
				sign[r0][i][j] = pos[j];

			for(int q = 0; q < p; q++)
			{
				seed = unsigned(chrono::system_clock::now().time_since_epoch().count());
				shuffle(possign.begin(), possign.end(), default_random_engine(seed));

				for(int j = 0; j < 4; j++)
				{
					sign1[r0][i][j][q] = (possign[j] % 2) ? 1: -1;
					sign2[r0][i][j][q] = (possign[j] / 2) ? 1: -1;
				}
			}
		}
}

vector<seed> get_bmers(string s, int k, int b, int r0)
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
		int p0 = sign[r0][0][dict[s[st]]];
		f_min[1][1][p0] = f_max[1][1][p0] = sign2[r0][0][dict[s[st]]][p0] * word[r0][0][dict[s[st]]][p0];
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
				int v = sign[r0][j-1][now];

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
						if(sign1[r0][j-1][now][z] == -1)
						{
							v1 = -f_min[i-1][j-1][k];
							v2 = -f_max[i-1][j-1][k];

							if(sign2[r0][j-1][now][z] == -1)
							{
								v1 -= word[r0][j-1][now][z];
								v2 -= word[r0][j-1][now][z];
							}
							else
							{
								v1 += word[r0][j-1][now][z];
								v2 += word[r0][j-1][now][z];
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

							if(sign2[r0][j-1][now][z] == -1)
							{
								v1 -= word[r0][j-1][now][z];
								v2 -= word[r0][j-1][now][z];
							}
							else
							{
								v1 += word[r0][j-1][now][z];
								v2 += word[r0][j-1][now][z];
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
		
		if(ret.size() > 0 && tmp.hashval == ret.back().hashval && ret.back().st >= st)
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

				if(sign1[r0][y-1][dict[s[g_min[x][y][mod] + st]]][mod] == -1)
					z = 0;
				
				mod1 = (mod + p - sign[r0][y-1][dict[s[g_min[x][y][mod] + st]]]) % p;
				x = g_min[x][y][mod];
				y--;
				mod = mod1;
			}
			else
			{
				tmp.index.push_back(g_max[x][y][mod] + st);

				if(sign1[r0][y-1][dict[s[g_max[x][y][mod] + st]]][mod] == -1)
					z = 1;

				mod1 = (mod + p - sign[r0][y-1][dict[s[g_max[x][y][mod] + st]]]) % p;
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

	return ret;
}

struct submatch{int l,r,len;};

inline bool cmp_hashval(seed a, seed b)
{
	return a.hashval < b.hashval || (a.hashval == b.hashval && a.st < b.st);
}

inline bool cmp_sm(submatch a, submatch b)
{
	return a.len > b.len;
}

seeding* processor;

int roundee;
double ans[20] = {0};

void pseudo_match(string s, string t, vector<int> &align)
{
	vector<matchpoint> matches;
	int r = 0;
	int lens = s.length();
	int lent = t.length();

	for(int r0 = 1; r0 <= roundee; r0++)
	{
		vector<seed> seeds = get_bmers(s, k, b, r0-1);
		vector<seed> seedt = get_bmers(t, k, b, r0-1);

		sort(seeds.begin(), seeds.end(), cmp_hashval);
		processor->vec2ll(seeds);

		sort(seedt.begin(), seedt.end(), cmp_hashval);
		processor->vec2ll(seedt);

		processor->getmatches(&seeds[0], &seedt[0], matches);
	}

	vector<bool> scover(lens, 0), mcover(lens, 0);
	vector<bool> scover2(lent, 0), mcover2(lent, 0);

	vector<bool> fscover(lens, 0), fmcover(lens, 0);
	vector<bool> fscover2(lent, 0), fmcover2(lent, 0);

	int truematches = 0;
	for(matchpoint m: matches)
	{
		int tp = 0;
		for(int i = 0; i < b; i++)
			if(align[m.ac1.index[i]] == m.ac2.index[i])
				tp++;

		if(2 * tp >= b)
		{
			truematches++;

			for(int i = m.ac1.index[b-1]; i <= m.ac1.index[0]; i++)
				mcover[i] = 1;			
			for(int i = 0; i < b; i++)
				scover[m.ac1.index[i]] = 1;

			for(int i = m.ac2.index[b-1]; i <= m.ac2.index[0]; i++)
				mcover2[i] = 1;
			for(int i = 0; i < b; i++)
				scover2[m.ac2.index[i]] = 1;
		}
		else
		{
			for(int i = m.ac1.index[b-1]; i <= m.ac1.index[0]; i++)
				fmcover[i] = 1;			
			for(int i = 0; i < b; i++)
				fscover[m.ac1.index[i]] = 1;

			for(int i = m.ac2.index[b-1]; i <= m.ac2.index[0]; i++)
				fmcover2[i] = 1;
			for(int i = 0; i < b; i++)
				fscover2[m.ac2.index[i]] = 1;

		}
	}

	int mc = 0, sc = 0, mc2 = 0, sc2 = 0;
	int fmc = 0, fsc = 0, fmc2 = 0, fsc2 = 0;
	double island1 = 0, island2 = 0;
	int gap1 = 0, gap2 = 0;

	for(int i = 0; i < lens; i++)
	{
		mc += mcover[i];
		sc += scover[i];
		fmc += fmcover[i];
		fsc += fscover[i];

		if(!mcover[i])
			gap1++;
		else if(gap1 > 0)
		{
			island1 += gap1 * gap1;
			gap1 = 0;
		}

		if(!scover[i])
			gap2++;
		else if(gap2 > 0)
		{
			island2 += gap2 * gap2;
			gap2 = 0;
		}
	}
	gap1 = 0;
	gap2 = 0;

	for(int i = 0; i < lent; i++)
	{
		mc2 += mcover2[i];
		sc2 += scover2[i];
		fmc2 += fmcover[i];
		fsc2 += fscover[i];

		if(!mcover2[i])
			gap1++;
		else if(gap1 > 0)
		{
			island1 += gap1 * gap1;
			gap1 = 0;
		}

		if(!scover2[i])
			gap2++;
		else if(gap2 > 0)
		{
			island2 += gap2 * gap2;
			gap2 = 0;
		}
	}

	ans[0] += matches.size();
	ans[1] += truematches;
	if(matches.size() > 0)
	{
		ans[7] += 1;
		ans[2] += double(truematches)/matches.size();
	}
	ans[3] += double(mc2) / lent + double(mc) / lens;
	ans[4] += island1;
	ans[5] += double(sc2) / lent + double(sc) / lens;
	ans[6] += island2;

	ans[8] += double(fmc2) / lent + double(fmc) / lens;
	ans[9] += double(fsc2) / lent + double(fsc) / lens;
}

int main(int argc, const char * argv[])
{
	k = stoi(argv[1]);
	b = stoi(argv[2]);
	p = stoi(argv[3]);
	roundee = stoi(argv[4]);

	string s, t;
	int n = 0;
	int tmp, len;

	init(b);


	while(cin>>s)
	{
		cin>>t;
    	vector<int> align;

		len = s.length();
		for(int i = 0; i < len; i++)
		{
			cin>>tmp;
			align.push_back(tmp);
		}

		n++;

		pseudo_match(s, t, align);
	}

	printf("%d/%d/%d/%d, %.2lf, %.2lf, %.4lf, %.4lf, %.4lf\n", k, b, p, roundee, ans[0], ans[1], ans[2] / ans[7], ans[5] / (2*n), ans[9] / (2*n));

	return 0;
}
