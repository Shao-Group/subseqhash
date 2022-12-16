#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <random>
#include <set>
#include <chrono>
#include <climits>
#include <map>
#include <cstring>
#include <unordered_set>
#include "seeding.h"

random_device rd;

int k, b, p;
int tablenum;

inline bool cmp_hashval(seed a, seed b)
{
	return a.hashval < b.hashval || (a.hashval == b.hashval && a.st < b.st);
}


seeding* processor;

int matchdata[1000][2] = {0};
double ans[20] = {0};
vector<vector<seed>> seeds(20);
string refseq[20];
int chrnum = 0;

vector<vector<bool>> scover, scover1;
vector<vector<bool>> fscover, fscover1;

void pseudo_match(string t, int readnum, int refnum, vector<seed> &seedt, vector<int> &align)
{
	vector<matchpoint> matches;
	int lent = t.length();
		
	processor->getmatches(&(seeds[refnum][0]), &(seedt[0]), matches);


	int matchessize = 0;
	int truematches = 0;

	for(matchpoint m: matches)
	{
		int tp = 0;
		for(int i = 0; i < b; i++)
			if(m.ac1.index[i] == align[m.ac2.index[i]])
				tp++;

		if(2 * tp >= b)
		{
			truematches++;
			for(int i = m.ac2.index[0]; i <= m.ac2.index[b-1]; i++)
				scover1[readnum][i] = 1;
			for(int i = 0; i < b ; i++)
				scover[readnum][m.ac2.index[i]] = 1;
		}
		else
		{
			for(int i = m.ac2.index[0]; i <= m.ac2.index[b-1]; i++)
				fscover1[readnum][i] = 1;
			for(int i = 0; i < b ; i++)
				fscover[readnum][m.ac2.index[i]] = 1;
		}
	}

	matchessize += matches.size();
	matches.clear(); //match to other chromosome
	for(int j = 0; j < chrnum; j++)
		if(j != refnum)
			processor->getmatches(&(seeds[j][0]), &(seedt[0]), matches);

	for(matchpoint m: matches)
	{
		for(int i = m.ac2.index[0]; i <= m.ac2.index[b-1]; i++)
			fscover1[readnum][i] = 1;
		for(int i = 0; i < b ; i++)
			fscover[readnum][m.ac2.index[i]] = 1;
	}


	matchessize += matches.size();


	matchdata[readnum][0] += matchessize;
	matchdata[readnum][1] += truematches;
}

void stats(int readnum, int lent)
{
	int fsc = 0, sc = 0;
	int fsc1 = 0, sc1 = 0;
	double island = 0;
	int gap = 0;

	for(int i = 0; i < lent; i++)
	{
		sc += scover[readnum][i];
		fsc += fscover[readnum][i];

		sc1 += scover1[readnum][i];
		fsc1 += fscover1[readnum][i];

		if(!scover[readnum][i])
			gap++;
		else if(gap > 0)
		{
			island += gap * gap;
			gap = 0;
		}
	}

	if(gap > 0)
		island += gap * gap;

	ans[0] += matchdata[readnum][0];
	ans[1] += matchdata[readnum][1];
	if(matchdata[readnum][0] > 0)
	{
		ans[7] += 1;
		ans[2] += double(matchdata[readnum][1])/matchdata[readnum][0];
	}
	ans[3] += double(sc) / lent;
	ans[4] += double(fsc) / lent;
	ans[5] += double(sc1) / lent;
	ans[6] += double(fsc1) / lent;
}

void getseeds(string path, vector<seed> &seeds)
{
	ifstream fin(path.c_str());

	double hashval;
	int x;

	while(fin>>hashval)
	{
		seed tmp;
		tmp.hashval = hashval;

		for(int i = 0; i < b; i++)
		{
			fin>>x;
			tmp.index.push_back(x);
		}

		seeds.push_back(tmp);
	}
}
vector<vector<int>> align;
vector<string> trueref;
vector<string> reads;

int main(int argc, const char * argv[])
{
	k = stoi(argv[1]);
	b = stoi(argv[2]);
	p = stoi(argv[3]);
	tablenum = stoi(argv[4]);

	string refpath = argv[5];

	string refseedpath = argv[6];
	string readseedpath = argv[7];

	string in;
	ifstream refin(refpath.c_str());
	while(getline(refin, in))
	{
		refseq[chrnum++] = in;
		getline(refin, in);
	}

	string s, t;
	int n = 0;
	int tmp, len;


	while(cin>>s)
	{
		vector<int> aligninput;

		len = s.length();
		for(int j = 0; j < len; j++)
		{
			cin>>tmp;
			aligninput.push_back(tmp);
		}
		cin>>t;

		reads.push_back(s);
		align.push_back(aligninput);
		trueref.push_back(t);

		scover.push_back(vector<bool>(len, 0));
		scover1.push_back(vector<bool>(len, 0));
		fscover.push_back(vector<bool>(len, 0));
		fscover1.push_back(vector<bool>(len, 0));

		n++;
	}

	for(int i = 1; i <= tablenum; i++)
	{
		for(int j = 0; j < chrnum; j++)
			{
				string refpath = refseedpath + "/" + refseq[j] + "_" + to_string(i);
				// cout<<refpath<<endl;
				seeds[j].clear();
				getseeds(refpath, seeds[j]);

				sort(seeds[j].begin(), seeds[j].end(), cmp_hashval);
				processor->vec2ll(seeds[j]);
			}

		ifstream fin;
		string qpath = readseedpath  + "/" + to_string(i);
		fin.open(qpath.c_str());

		for(int j = 0; j < n; j++)
		{
			vector<seed> seedt;
			double hashval;
			int x;


			while(fin>>hashval)
			{
				if(hashval == 0.0)
					break;

				seed tmp;
				tmp.hashval = hashval;

				for(int i = 0; i < b; i++)
				{
					fin>>x;
					tmp.index.push_back(x);
				}

				seedt.push_back(tmp);
			}


			sort(seedt.begin(), seedt.end(), cmp_hashval);
			processor->vec2ll(seedt);

			for(int l = 0; l < chrnum; l++)
				if(refseq[l] == trueref[j])
				{
					pseudo_match(reads[j], j, l, seedt, align[j]);
					break;
				}

			if(i == tablenum)
				stats(j, reads[j].length());
		}
	}

	printf("%d/%d/%d/%d, %.2lf, %.2lf, %.4lf, %.4lf, %.4lf\n", k, b, p, tablenum, ans[0], ans[1], ans[2] / ans[7], ans[3] / n, ans[4] / n);
	return 0;
}