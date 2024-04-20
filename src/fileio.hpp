#ifndef _FILEIO_HEADER_
#define _FILEIO_HEADER
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
using namespace std;
struct GAMEFILE{
	string content;
	vector<pair<string,bool>> info;
	int seed;
};
ostream &operator<<(ostream &os,const GAMEFILE &x){
	string outn,savs;
	outn=x.content;
	outn+="\n";
	outn+=to_string(x.info.size());
	outn+="\n";
	for(auto &sth:x.info){
		outn+=sth.first+to_string(sth.second)+"\n";
	}
	outn+=to_string(x.seed);
//	for(int i=0,siz=outn.size();i<siz;i++){
//		savs.push_back(outn[i]^outn[siz-i-1]);
//	}
	os << outn;
	return os;
}
istream &operator>>(istream &is,GAMEFILE &x){
//	is >> x.content;
	getline(is,x.content);
	int n;
	is >> n;
	is.get();
	for(int i=1;i<=n;i++){
		pair<string,bool> tmp;
//		is >> tmp.first;
		getline(is,tmp.first);
		tmp.second=(tmp.first.back()=='1');
		tmp.first.pop_back();
		x.info.push_back(tmp);
	}
	is >> x.seed;
	return is;
}
#endif
