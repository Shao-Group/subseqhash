#include <iostream>
#include <random>
#include <map>
#include <chrono>
#include <fstream>
#include <algorithm>

using namespace std;
random_device rd;

char nt[4] = {'A', 'C', 'G', 'T'};
map<char,int> dict;

string simulation(int len)
{
	string s = "";
	for(int i = 0; i < len; i++)
	{
		int x = rd() % 4;
		s = s + nt[x];
	}

	return s;
}

vector<vector<pair<string, string>>> data (30);
int n;

int editdist(string s1, string s2)
{
	int f[101][101] = {0};

	int n1 = s1.length();
	int n2 = s2.length();

	for(int i = 0; i <= n1; i++)
		f[i][0] = i;
	for(int i = 0; i <= n2; i++)
		f[0][i] = i;

	for(int i = 1; i <= n1; i++)
		for(int j = 1; j <= n2; j++)
		{
			f[i][j] = f[i-1][j-1] + ((s1[i-1] == s2[j-1]) ? 0: 1);
			f[i][j] = min(f[i][j], f[i-1][j] + 1);
			f[i][j] = min(f[i][j], f[i][j-1] + 1);
		}

	return f[n1][n2];
}

int en = 0;
int maxedits;

void r_mutation(string s, int r)
{
	int i, len = s.length();

	vector<int> pos;
	for(int i = 0; i < len; i++)
		pos.push_back(i);

	unsigned seed = unsigned(chrono::system_clock::now().time_since_epoch().count());
	shuffle(pos.begin(), pos.end(), default_random_engine(seed));

	int change[100] = {0};

	for(int i = 0; i < r; i++)
	{
		int type = rd() % 3;
		string t = "";

		// cout<<i<<" "<<pos[i]<<" "<<type<<endl;

		if(type == 0)
			change[pos[i]] = 1;
		else if(type == 1)
			change[pos[i]] = 2;
		else 
			change[pos[i]] = 3;


		for(int j = 0; j < len; j++)
			if(change[j] == 1)
			{
				char c = nt[(dict[s[j]] + (rd() % 3) + 1) % 4];
				t = t + c;
			}
			else if(change[j] == 2)
			{
				char c = nt[rd() % 4];
				t = t + c;
				t = t + s[j];
			}
			else if(change[j] == 0)
				t = t + s[j];

			int e = editdist(s,t);

			if(e && e <= maxedits && data[e-1].size() < n)
			{
				data[e-1].push_back(make_pair(s,t));
				if(data[e-1].size() == n)
					en++;
			}
	}
}

int main(int argc, const char * argv[])
{
	int seqlen = stoi(argv[1]);

	n = atoi(argv[2]);
	maxedits = atoi(argv[3]);
	string directory = argv[4];

    dict['A'] = 0;
    dict['C'] = 1;
    dict['G'] = 2;
    dict['T'] = 3;
    
	while(en < maxedits)	
	{
		string s = simulation(seqlen);
		r_mutation(s, seqlen);
 	}

	for(int i = 0; i < maxedits; i++)
	{		
		string fname = directory +  "/" + to_string(i+1);
		FILE * fp = fopen (fname.c_str(), "w");

		for(auto a: data[i])
			fprintf(fp, "%s\n%s\n", a.first.c_str(), a.second.c_str());
		fclose(fp);
	}
	return 0;
}