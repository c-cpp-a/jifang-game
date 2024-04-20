//github.com/c-cpp-a/jifang-game
#include<iostream>
#include<map>
#include<vector>
#include<fstream>
#include<sstream>
#include "./src/players.hpp"
#include "./src/fileio.hpp"
using namespace std;
map<string,pair<int,int>> players_calc;//统计玩家的 key=名字 first=赢得局数 second=总局数 
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
	if(argc>1){
		//TODO 支持配置文件
		//配置文件格式：
		//对于每一行 [name delete=xxx]，xxx是你想要禁用的卡牌，用,隔开
		bool flag=setdelete(argv[1]);
		if(!flag){
			cout << "不支持导入此配置文件！\n";
			throw "";
		}
		cout << "设置中……\n";
		for(auto &perdel:deletecards){
			cout << "对于玩家 " << perdel.first << "，禁用卡牌 ";
			for(auto &card:perdel.second){
				cout << card << "，";
			}
			cout << "完毕。\n";
		}
		deletecard();
		cout << "设置完成\n";
		system("pause");
		system("cls");
	}
	cout.flags(ios::fixed);
	cout.precision(2);
	system("title 机房杀");
	start:
	system("cls");
	cout<<"欢迎来到机房杀！\n";
	cout<<"[1]游戏说明\t[2]开始游戏\t[3]统计信息\n";
	int x;
	string s;
	x=getch()-'0';
	if(x==1){
		system("cls");
		help();
		goto start;
	}
	else if(x==2){
		REPLAY::clear();
		Random::seed=time(0);
		Random::e.seed(seed);
		int n;
		system("cls");
		cout<<"游戏人数：";
		cin>>n;
		string tmpp;
		getline(cin,tmpp);
		pgroup=Group(n);
		for(int i=1;i<=pgroup.playersum;i++){
			players_calc[pgroup.players[i].name].second++;
		}
		player winner=pgroup.gamemain();
		system("cls");
		cout<<"获胜者是：";
		if(winner.isempty())cout<<"空气……\n";
		else{
			cout << winner.name << "!\n";
			players_calc[winner.name].first++;
		}
		pause();
		system("cls");
		cout << "你想要保存本次对局吗？(Y/N)";
		int op=_getch();
		if(op=='Y' || op=='y'){
			GAMEFILE file;
			file.content=REPLAY::getstr();
			for(int i=1;i<=pgroup.playersum;i++){
				file.info.push_back(make_pair(pgroup.players[i].name,pgroup.players[i].isAI));
			}
			file.seed=Random::seed;
			ofstream fout(string("Jifang_game_seed="+to_string(Random::seed)+".replay").c_str());
			fout << file;
			fout.close();
		}
		goto start;
	} else if(x==3){
		system("cls");
		cout << "姓名\t赢的对局\t总对局\t胜率\n";
		for(map<string,pair<int,int>>::iterator it=players_calc.begin();it!=players_calc.end();++it){
			cout << it->first << '\t' << it->second.first << '\t' << it->second.second << '\t' << 100.0*(it->second.first)/(it->second.second) << "%\n";
		}
		pause();
		goto start;
	}
	else{
		goto start;
	}
	return 0;
}
