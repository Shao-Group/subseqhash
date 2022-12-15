/*
  Read in a list of strings from file and compute the statistics on edit 
  distance between adjacent bmers.

  The first line of the input file is a number N, followed by N bmers, one on 
  each line.

  By: Ke, Xiang@PSU
  Last edited: 12/14/2022
*/

#include <string.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

long unsigned encode(string str, const int k)
{
    long unsigned enc = 0;
    int i, x;
    for(i=0; i<k; i+=1)
    {
		switch(str[i])
		{
			case 'A': x=0; break;
			case 'C': x=1; break;
			case 'G': x=2; break;
			case 'T': x=3; break;
		}
		enc = (enc << 2)|x;
    }
    return enc;
}

int editDist(const long unsigned s1, const int k1, const long unsigned s2, const int k2, const int max_d)
{
    if(k1 > k2) return editDist(s2, k2, s1, k1, max_d);
    int diag_index = k2 - k1;
    if(max_d >= 0 && diag_index >= max_d) return diag_index;
    
    int row[k2+1];
    int i, j, diag, cur,tmp;
    for(i=0; i<k2+1; i+=1){
	row[i] = i;
    }

    long unsigned s1_copy, s2_copy;
    for(i=1, s1_copy=s1; i<k1+1; i+=1, s1_copy>>=2){
	diag_index += 1;
	diag = row[0];
	row[0] = i;
	
	for(j=1, s2_copy=s2; j<k2+1; j+=1, s2_copy>>=2){
	    //substitution
	    cur = diag + ((s1_copy & 3) == (s2_copy & 3) ? 0 : 1);
	    //deletion
	    tmp = row[j] + 1;
	    cur = cur > tmp ? tmp : cur;
	    //insertion
	    tmp = row[j-1] +1;
	    cur = cur > tmp ? tmp : cur;
	    
	    diag = row[j];
	    row[j] = cur;
	}

	if(max_d >= 0 && row[diag_index] >= max_d){
	    break;
	}
    }

    return row[diag_index];
}

void readFromFile(const char* filename, int k, size_t length, vector<long unsigned> & stringlist)
{
	ifstream fin;
	fin.open(filename);

	size_t i;
	string s;
	double w;
	int phi;

	for(i=0; i < length; i+=1)
	{
		fin>>s>>w>>phi;
		stringlist.push_back(encode(s, k));
	}

	fin.close();
}

int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		printf("usage: statistics k number_of_strings filename\n");
		return 1;
	}		

	int k = atoi(argv[1]);
	size_t list_len = atoi(argv[2]);

	vector<long unsigned> stringlist;
	readFromFile(argv[3], k, list_len, stringlist);

	int j, cur_closest_dist, dist, l, i;

	//smallest and avg dist between stringlist[i] and all strings
	//in the window stringlist[i-d,...,i+d] (exclude i obviously)
	int d_choices_num = 10;
	int d_choices[] = {1, 2, 3, 4, 5, 10, 20, 50, 100, 500};
	int min_dist_in_window[d_choices_num][k+1];
	double avg_dist_in_window[d_choices_num];
	memset(min_dist_in_window, 0, sizeof (int) * d_choices_num * (k+1));
	memset(avg_dist_in_window, 0, sizeof (double) * d_choices_num);
	int d, total_strings_in_window;
	double total_dist_in_window;

	for(l=0; l<d_choices_num; l+=1)
	{
		d = d_choices[l];
		for(i=0; i<list_len; i+=1)
		{
			cur_closest_dist = k+1;
			total_dist_in_window = 0.0;
			total_strings_in_window = 0;
			for(j=1; j<=d; j+=1)
			{
				if(i-j >= 0)
				{
					dist = editDist(stringlist[i], k, stringlist[i-j], k, -1);
					total_dist_in_window += dist;
					total_strings_in_window += 1;
					if(dist < cur_closest_dist)
					{
						cur_closest_dist = dist;
					}
				}	
				if(i+j < list_len)
				{
					dist = editDist(stringlist[i], k, stringlist[i+j], k, -1);
					total_dist_in_window += dist;
					total_strings_in_window += 1;
					if(dist < cur_closest_dist)
					{
						cur_closest_dist = dist;
					}
				}
			}
			min_dist_in_window[l][cur_closest_dist] += 1;
			avg_dist_in_window[l] += total_dist_in_window / total_strings_in_window;
		}
	}


	int sum = 0;

	printf("\nwithin window [i-d, i+d]");
	for(i=0; i<d_choices_num; i+=1)
	{
		sum = 0;
		for(j=1; j<=k; j+=1)
		{
			sum += min_dist_in_window[i][j] * j;
		}
		printf("\nd=%d, avg min distance: %f, avg distance: %f\nedit count\n",
		d_choices[i], ((double)sum)/list_len, avg_dist_in_window[i]/list_len);
		
		for(j=1; j<=k; j+=1)
		{
			printf("%2d %d\n", j, min_dist_in_window[i][j]);
		}
	}

	return 0;
}
