#include <iostream>
#include <random>
#include <map>
#include <fstream>

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

vector<int>align;

string r_mutation(string s, double r) //Mutate with rate r
{
	int i, len = s.length();
	string t;
	for(i = 0; i < len; i++)
	{
		double z = rd() % 10000;
		if(z >= 10000 * r)
		{
			align.push_back(t.length());
			t = t + s[i];
		}
		else
		{
			int x = rd() % 3;

			if(x == 0)
			{			
				align.push_back(-1);
				char c = nt[(dict[s[i]] + (rd() % 3) + 1) % 4];
				t = t + c;
			}
			else if(x == 1)
			{				
				align.push_back(t.length());
				t = t + s[i];
				char c = nt[rd() % 4];
				t = t + c;
			}
			else
				align.push_back(-1);
		}
	}

	return t;
}

int main(int argc, const char * argv[])
{
	int seqlen = stoi(argv[1]);
	double errorrate = stod(argv[2]);

	int n = atoi(argv[3]);

    dict['A'] = 0;
    dict['C'] = 1;
    dict['G'] = 2;
    dict['T'] = 3;
    
	for(int i = 0; i < n; i++)
	{
		align.clear();
		string s = simulation(seqlen);
		string t = r_mutation(s, errorrate);
		
		printf("%s\n%s\n", s.c_str(), t.c_str());
		for(int j = 0; j < seqlen; j++)
			printf("%d ", align[j]);
		printf("\n");
	}
	return 0;
}
