#include<iostream>
#include<fstream>
#define ROLEPLAY
#include "./src/players.hpp"
#include "./src/fileio.hpp"
using namespace std;
vector<pair<string,vector<string>>> deletecards;//被禁用的卡牌
void deletecard(){
	for(auto &perdel:deletecards){
		for(auto &pl:pgroup.players){
			if(pl.name==perdel.first){
				for(auto &card:perdel.second){
					pl.tags[card]="always_delete";
				}
			}
		}
	}
}
bool setdelete(const char * filename){
	static char buff1[1001]={},buff2[100001]={};
	string tmpline;
	string tmp,tmp2;
	vector<string> tmpp;
	istringstream sin;
	ifstream fin(filename);
	while(getline(fin,tmpline) && tmpline.size()){
		if(tmpline.back()!=']')	return false;
		int flag=sscanf(tmpline.c_str(),"[%s delete=%s]",buff1,buff2);
		if(flag!=2){
			return false;
		}
		tmp=string(buff1);
		tmp2=string(buff2);
		sin.str(tmp2);
		tmp2.clear();
		while(getline(sin,tmp2,',')){
			tmpp.push_back(tmp2);
		}
		deletecards.push_back(make_pair(tmp,tmpp));
		deletecards.back().second.back().pop_back();
	}
	fin.close();
	return true;
}
int main(int argc,char ** argv){
	if(argc<2){
		cerr << "回放失败：找不到文件！\n";
		system("pause");
		return 0;
	}
	if(argc>2){
		//TODO 支持配置文件
		//配置文件格式：
		//对于每一行 [name delete=xxx]，xxx是你想要禁用的卡牌，用,隔开
		bool flag=setdelete(argv[2]);
		if(!flag){
			cerr << "不支持导入此配置文件！\n";
			system("pause");
//			throw "";
			return 0;
		}
		cerr << "设置中……\n";
		for(auto &perdel:deletecards){
			cerr << "对于玩家 " << perdel.first << "，禁用卡牌 ";
			for(auto &card:perdel.second){
				cerr << card << "，";
			}
			cerr << "完毕。\n";
		}
		deletecard();
		cerr << "设置完成\n";
		system("pause");
		system("cls");
	}
	GAMEFILE file;
	ifstream fin(argv[1]);
	fin >> file;
	fin.close();
	string title=string("title 机房杀[")+argv[1]+"]";
	system(title.c_str());
//	cerr << "导入信息：seed=" << file.seed << ", content=" << file.content << ".\n";
	REPLAY::clear();
	REPLAY::setstr(file.content);
	Random::seed=file.seed;
	Random::e.seed(seed);
	pgroup.playersum=file.info.size();
//	cerr << "playersum=" << pgroup.playersum << ".\n";
	for(auto &sth:file.info){
		pgroup.players.push_back(player(sth.first,sth.second));
	}
	player winner=pgroup.gamemain();
	system("cls");
	cerr<<"获胜者是：";
	if(winner.isempty())cerr<<"空气……\n";
	else{
		cerr << winner.name << "!\n";
	}
	system("pause");
	return 0;
}
