#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <chrono>
#include <climits>
#include <map>
#include <unordered_map>
#include "seeding.h"

void seeding::vec2ll(vector<seed>& v)
{
	int sz = v.size();
	seed* head = &v[0];
	seed* last = &v[0];

	for(int i = 1; i < sz; i++)
	{
		// cout<<i<<" "<<head<<" "<<last<<endl;
		if(last->hashval == v[i].hashval)
		{	
			last->next = &v[i];
			last = &v[i];
		}
		else
		{
			head->succ = &v[i];
			head = last = &v[i];
		}
	}
}

void seeding::getmatches(seed *t1, seed *t2, vector<matchpoint>& matches)
{
	seed *s1, *s2;
	while (t1 != NULL && t2 != NULL)
	{

		if(t1->hashval < t2->hashval) 
			t1 = t1->succ;
		else if(t2->hashval < t1->hashval) 
			t2 = t2->succ;
		else 
		{
			s1 = t1;
			
			while(s1 != NULL)
			{
				s2 = t2;

		// 		cout<<s1<<" "<<s2<<endl;
				while(s2 != NULL)
				{
					matchpoint tmp;
					tmp.hashval = s1->hashval;
					tmp.ac1 = *s1;
					tmp.ac2 = *s2;

					matches.push_back(tmp);

					s2 = s2->next;
				}

				s1 = s1->next;
			}

		  	t1 = t1->succ;
		  	t2 = t2->succ;
		}
	}
}
