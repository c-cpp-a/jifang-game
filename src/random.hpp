#ifndef _RANDOM_HEADER_ 
#define _RANDOM_HEADER_
#include<random>
#include "defs.hpp"
using namespace std;
namespace Random{
	int seed=time(0);
	default_random_engine e(seed);
	int randint(int l,int r){
		if(l>r)	swap(l,r);
		return uniform_int_distribution<int>(l,r)(e);
	}
	template<typename T>
	const T &randchoose(const pair<vector<double>,vector<T>> &x){
		double randsum=0;
		for(auto &s:x.first){
			randsum+=s;
		}
		double rands=uniform_real_distribution<double>(0,randsum)(e);
		double summ=0;
		for(int i=0,siz=x.first.size();i<siz;i++){
			summ+=x.first[i];
			if(rands<=summ)	return x.second[i];
		}
		return x.second.back();
	}
	pair<int,int> randcardid(){
		return make_pair(randchoose(make_pair(cardprop,carddropid)),randchoose(make_pair(rarityprop,rarityid)));
	}
}
#endif 
