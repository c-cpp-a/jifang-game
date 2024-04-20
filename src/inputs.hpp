#ifndef _INPUTS_HEADER_
#define _INPUTS_HEADER_
#include<conio.h>
#include<string>
#include<cctype>
#include<windows.h>
#undef getch
#include "defs.hpp"
namespace REPLAY{
	static std::string sav_str;
	static int i=0;
	void clear(){
		sav_str.clear();
		i=0;
	}
	const std::string &getstr(){
		return sav_str;
	}
	void setstr(const std::string &s){
		sav_str=s;
		i=0;
	}
}
void pause(){
	_getch();
}
#ifndef ROLEPLAY
int getch(){
	auto tmp=_getch();
	if(isalnum(tmp))	REPLAY::sav_str.push_back(tmp);
	return tmp;
}
#else
int getch(){
	Sleep(AI_THINK_MS);
	if(REPLAY::i<REPLAY::sav_str.size()){
		return REPLAY::sav_str[REPLAY::i++];
	} else{
		throw "getch Error!\n";
	}
}
#endif
#endif
