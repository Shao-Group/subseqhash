#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <chrono>
#include <climits>
#include <map>
#include <unordered_map>

using namespace std;

struct seed{double hashval; int st,en,kst,ken; vector<int> index; seed * next = NULL; seed * succ = NULL;};
struct matchpoint{double hashval; seed ac1, ac2;};

class seeding{
public:

	void vec2ll(vector<seed>& v);
	void getmatches(seed *t1, seed *t2, vector<matchpoint>& matches);
};