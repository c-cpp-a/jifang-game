#include<iostream>
#include<vector>
#include<random>
#include<conio.h>
#include<windows.h>
#include<ctime> 
#include<map> 
#include<fstream>
#include<sstream>
using namespace std;
constexpr int cardnums=10;
const string sprite_names[7]={"","lyr","xza","cyq","wcy","nmk","AJ"};
const string card_names[11]={"","学新知识点","做题","狂人卷题","禁止内卷","腐败","一起腐败","向AJ举报","急眼","翻墙","机惨"};
const string skill_names[7][3]={//主动技能
	{},
	{"腐败王","制裁"},
	{},
	{"出题人"},
	{"水谷"},
	{"你是个废物！"},
	{}
};
const string raritynames[5]={
	"",
	"罕见",
	"稀有",
	"传奇",
	"神话"
};
constexpr int skills[7]={0,2,0,1,1,1,0};//主动技能数
const vector<double> cardprop={0,1.7,1,0.9,1,0.8,1.1,1.3,1,1,1};//卡牌概率权重
const vector<int> carddropid={0,1,2,3,4,5,6,7,8,9,10};
const vector<double> rarityprop={90,30,10,3,2};//稀有度概率权重（2/3:2/9:2/27:1/45:2/135） 
const vector<int> rarityid={0,1,2,3,4};
constexpr double rarity_times[5]={1,1.66,2.7889,4.657463,7.77796321};//稀有度对有益效果的加成 
constexpr int AI_THINK_MS=200;//AI思考毫秒数
namespace Random{
	default_random_engine e(time(0));
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
using namespace Random;
struct player{
	string name;//名字
	int spriteid;//角色id
	int life=20;//生命
	int score=100;//成绩
	int knowledge=0;//知识点
	vector<pair<int,int>> cards;//手中卡牌 first=卡牌id second=稀有度 
	bool isdead=false;//是否死亡
//	bool corruption=false;//是否腐败
//	bool involution=false;//是否内卷
//	int noend=0;//（仅AJ有效）不死之身次数
//	bool canplay[10];//能否使用此卡牌
	bool isAI=false;//是否是AI代管
	map<string,string> tags;//状态
	player(const string &_name="",bool _ai=false):name(_name),isAI(_ai){
		tags["corruption"]="false";
		tags["involution"]="false";
		if(name=="lyr"){
			spriteid=1;
		} else if(name=="xza"){
			spriteid=2;
		} else if(name=="cyq"){
			spriteid=3;
		} else if(name=="wcy"){
			spriteid=4;
		} else if(name=="nmk"){
			spriteid=5;
		} else if(name=="AJ"){
			spriteid=6;
		} else{
			spriteid=randint(0,6);
		}
		tags["spriteid"]=to_string(spriteid);
		if(spriteid==6){
			//AJ
//			noend=2;
			tags["noend"]="2";
		}
		for(int i=0;i<=cardnums;i++){
//			canplay[i]=true;
			tags[card_names[i]]="default";
		}
	}
	bool isempty(){
		return name.empty();
	}
	inline void used(int cardid){
		//这张卡牌被用了
		cards.erase(cards.begin()+cardid);
	}
	void add_lf(int delta);
	void add_sc(int delta){
		score=min(score+delta,400);
		if(score<0){
			score=0;
			add_lf(-6);
		}
	}
	void add_kng(int delta){
		knowledge=min(knowledge+delta,40);
	}
	int jcturn(){
		if(tags["jced"].empty())	return 0;
		else	return stoi(tags["jced"]);
	}
};
namespace User_input{
	player getplayer(){
		string name;
		char op;
		cout << "玩家姓名：";
		getline(cin,name);
		cout << "玩家是否为AI？(Y/N)";
		op=getch();
		if(op=='Y' || op=='y'){
			cout << "是\n";
			return player(name,true);
		} else{
			cout << "否\n";
			return player(name,false);
		}
	}
	void help(){
		//游戏信息
		cout<<"更新日志：" << endl;
		cout<<"2024.1.22 创建游戏"<<endl;
		cout<<"2024.1.23 最开始设计的游戏功能都实现了，测试完毕" << endl;
		cout<<"2024.1.27 可以增加AI了，但是这个AI就是从所有可选项中随机选一个，不知道有什么用。" << endl;
		cout<<"2024.1.29 游戏细节优化，修复bug" << endl;
		cout<<"2024.1.30 增加统计信息显示" << endl;
		cout<<"2024.1.31 增加功能：卡牌稀有度" << endl;
		cout<<"2024.4.12 增加卡牌：机惨" << endl;
		cout<<"2024.4.13 增加功能：禁用卡牌" << endl;
		cout<<"开发：lyr、xza、cyq"<<endl;
		cout<<"规则："<<endl;
		cout<<"  1.每回合结束时，将手中的牌弃到6张。"<<endl;
		cout<<"  2.角色技能如果没有备注，每回合限一次，牌没有限制。"<<endl;
		cout<<"  3.成绩<0时让成绩=0，然后生命-2，生命=0时如果没有特殊技能，角色死亡。"<<endl;
		cout<<"  4.每回合开始时，摸2张牌。"<<endl;
		cout<<"  5.游戏开始时，角色的生命为20，成绩为100，知识点为0。"<<endl;
		cout<<"卡牌（普通卡牌）："<<endl;
		cout<<"  1.学新知识点：成绩+10，知识点+1"<<endl;
		cout<<"  2.做题：成绩加上知识点数量*0.5（向上取整）"<<endl;
		cout<<"  3.狂人卷题：生命-1，成绩+知识点数量*1"<<endl;
		cout<<"  4.禁止内卷：所有角色本轮无法使用卡牌1、2、3。"<<endl;
		cout<<"  5.腐败：生命+1，成绩-7。"<<endl;
		cout<<"  6.一起腐败：你选择一名其他角色，你与其生命各+2，你成绩-8，其成绩-10。"<<endl;
		cout<<"  7.向AJ举报：你选择一名其他角色，其生命-1，如果其本轮成为过卡牌5、6的目标，改为生命-2。"<<endl;
		cout<<"  8.急眼：你选择一名其他角色，其生命-2，你成绩-5，然后如果其本轮成为过卡牌5、6的目标，其成绩-5。"<<endl;
		cout<<"  9.翻墙：除了本回合成为卡牌1、2、3的目标的人外，其他所有角色成绩-7，生命-1，然后你有50%概率成绩-3，生命-1。"<<endl;
		cout<<"  10.机惨：你选择一名其他角色，其成绩在接下来3回合中每回合-1，并且不能使用卡牌（卡牌正常抽取，可以弃牌），不叠加"<<endl;
		cout<<"有概率摸到更高稀有度，每一级有益效果*1.67（对最终结果向上取整），优先于被动技能。"<<endl;
		cout<<"角色："<<endl;
		cout<<"  1.lyr：使用卡牌5时生命+1，成绩-2。"<<endl;
		cout<<"		（1）腐败王：你可以视为使用了一张卡牌5，本轮其他所有角色无法使用卡牌1。"<<endl;
		cout<<"		（2）制裁：你可以自减5点生命，本轮其他所有角色无法使用卡牌4、5、6。"<<endl;
		cout<<"  2.xza：使用卡牌1时成绩+2，成为卡牌6的目标时成绩减少1/4。"<<endl;
		cout<<"		（1）蓝勾爷：你使用卡牌2、3时，效果结算后*1.6。"<<endl;
		cout<<"		（2）透明度：你使用卡牌5、6时有10%概率视为使用卡牌2且不视为腐败。"<<endl;
		cout<<"  3.cyq：使用卡牌5、6时有40%概率成绩-7。"<<endl;
		cout<<"		（1）急眼哥：你使用卡牌8时改为“所有其他角色生命-2，你成绩-7，然后如果有其他角色本轮成为过卡牌5、6的目标，其成绩-5。”"<<endl;
		cout<<"		（2）出题人：你可以令所有其他角色视为使用卡牌2，以此法使用的卡牌2改为“成绩加上知识点数量*0.1（向上取整）”，然后你成绩+5。"<<endl;
		cout<<"  4.wcy：每局限两次，你的生命每-1，成绩+2，成绩若>300，生命再+2。"<<endl;
		cout<<"		（1）秒题：你使用的卡牌2改为“成绩+知识点数量*0.8“。"<<endl;
		cout<<"		（2）水谷：你可以令所有角色本轮无法使用卡牌4，已有的禁止内卷效果无效。"<<endl;
		cout<<"  5.nmk：每轮摸1张卡牌5。"<<endl;
		cout<<"		（1）腐败小助手：你使用卡牌5、6时不视为腐败。"<<endl;
		cout<<"		（2）你是个废物！：你选择一名角色，令其弃置手中的所有卡牌1、2、3。"<<endl;
		cout<<"  6.AJ：在你腐败时，其它角色对你使用的卡牌7改为“你自减2点生命”。"<<endl;
		cout<<"		（1）抓腐：你使用卡牌7时。"<<endl;
		cout<<"		（2）不死之身：每局限两次，若你的生命<=2且不为0，你选择一项：1.生命+2；2.你选择一名其它角色，令其生命-5，成绩=0，然后你死亡。"<<endl;
		cout<<"禁用卡牌：" << endl;
		cout<<"  你可以将一个配置文件拖入可执行文件中，配置文件由若干行组成，每一行格式形如[name delete=c1[,c2,c3,...]]。"<<endl;
		cout<<"  name表示你要对其使用禁用的玩家名称。"<<endl;
		cout<<"  c1,c2,c3,...依次表示你要对此玩家禁用的卡牌，你指定的玩家在接下来的所有游戏中都不能使用这些卡牌。"<<endl;
		cout<<"  请注意你的配置文件编码，如果使用非ANSI编码，导入时可能会乱码。"<<endl;
		system("pause");
	}
	
}
using namespace User_input;
struct Group{
	vector<player> players;
	int playersum;
	Group(int n=0):playersum(n){
		map<string,bool> checked;
		players.push_back(player("",true));
		players.back().isdead=true;
		for(int i=1;i<=n;i++){
			player tmp;
			do{
				tmp=getplayer();
				if(checked[tmp.name]){
					cout << "此名字已被使用！\n";
				}
			} while(checked[tmp.name]);
			checked[tmp.name]=true;
			players.push_back(tmp);
		}
	}
	~Group()=default;
	int nowalive(){
		int ans=0;
		for(auto &p:players){
			ans+=(p.isdead==false);
		}
		return ans;
	}
	void gameinfo(player &nowplayer){
		system("cls");
		for(int i=1;i<=playersum;i++){
			if(players[i].isdead)	continue;
			
			cout << players[i].name;
			if(players[i].spriteid)	cout << "[" << sprite_names[players[i].spriteid] << "]";
			cout << "\t生命 " << players[i].life << "/20\t成绩 " << players[i].score << "/400\t知识点 " << players[i].knowledge << "/40";
			if(players[i].jcturn()){
				cout << "[机惨还剩 " << players[i].jcturn() << " 回合]";
			}
			cout << "\n";
		}
		cout << "玩家：" << nowplayer.name << "的回合";
		if(nowplayer.isAI){
			cout << "[AI代管]";
		}
		cout << "\n";
	}
	int choooseplayer(int playerid){
		static char chartoid[128]={};
		int op,j='A';
		for(int i=1;i<=playersum;i++){
			if(players[i].isdead)	continue;
			if(i==playerid)	continue;
			cout << "[" << char(j) << "]" << players[i].name << '\t';
			chartoid[j]=i;
			++j;
		}
		cout << '\n';
		if(players[playerid].isAI){
			Sleep(AI_THINK_MS);
			op=randint('A',j-1);
		} else{
			op=toupper(getch());
			while(!(op>='A' && op<=j-1))	op=toupper(getch());
		}
		
		return chartoid[op];
	}
	void usecard(int cardid,int playerid){
		int _usecard=players[playerid].cards[cardid].first,_rarity=players[playerid].cards[cardid].second;
		const string spritename=sprite_names[players[playerid].spriteid];
		players[playerid].cards.erase(players[playerid].cards.begin()+cardid);
		cout << players[playerid].name << "使用了" << card_names[_usecard] << "\n";
		if(_usecard>=1 && _usecard<=3){
			players[playerid].tags["involution"]="true";
		}
		if(_usecard>=5 && _usecard<=6){
			players[playerid].tags["corruption"]="true";
		}
		switch(_usecard){
		case 1:
			{
				int addsc=ceil(10*rarity_times[_rarity]),addkng=ceil(1*rarity_times[_rarity]);
				cout << players[playerid].name << "成绩+" << addsc << " 知识点+" << addkng << "\n";
				players[playerid].add_sc(addsc);
				players[playerid].add_kng(addkng);
				if(spritename=="xza"){
					cout << "xza发动了被动技能！学新知识点，成绩再+2。\n";
					players[playerid].add_sc(2);
				}	
			}
			
			break;
		case 2:
			{
				int addsc=ceil(players[playerid].knowledge*0.5);
				addsc=ceil(addsc*rarity_times[_rarity]);
				if(spritename=="xza"){
					cout << "xza发动了被动技能[蓝勾爷]！做题时各效果再x1.6。\n";
					addsc=ceil(addsc*1.6);
				} else if(spritename=="wcy"){
					cout << "wcy发动了被动技能[秒题]！使用的卡牌3改为每有1知识点，成绩+0.8。\n";
					addsc=ceil(players[playerid].knowledge*0.8);
				}
				cout << players[playerid].name << "成绩+" << addsc << "\n";
				players[playerid].add_sc(addsc);	
			}
			
			break;
		case 3:
			{
				int addsc=ceil(players[playerid].knowledge*1.0),addlf=-1;
				addsc=ceil(addsc*rarity_times[_rarity]);
				if(spritename=="xza"){
					cout << players[playerid].name << "发动了被动技能[蓝勾爷]！狂人卷题时各效果再x1.6。\n";
					addsc=ceil(addsc*1.6);
					addlf=ceil(-addlf)*-1.6;
				} 
				cout << players[playerid].name << "生命-" << -addlf << " 成绩+" << addsc << "\n";
				players[playerid].add_sc(addsc);
				players[playerid].add_lf(addlf);
			}
			break;
		case 4:
			{
				cout << "所有人本轮不能出[学新知识点][做题][狂人卷题]。\n";
				for(int i=1;i<=playersum;i++){
					players[i].tags["学新知识点"]=players[i].tags["做题"]=players[i].tags["狂人卷题"]="delete";
				}
			}
			break;
		case 5:
			{
				int addlf=1,addsc=-7;
				addlf=ceil(addlf*rarity_times[_rarity]);
				cout << players[playerid].name << "生命+" << addlf << " 成绩-" << -addsc << "\n";
				if(spritename=="lyr"){
					addlf++;
					addsc-=2;
					cout << players[playerid].name << "发动了被动技能！使用卡牌5时生命再+1，成绩再-2。\n";
				} else if(spritename=="xza"){
					if(randint(1,10)==1){
						cout << players[playerid].name << "发动了被动技能[透明度]！视为使用卡牌2且不视为腐败。\n";
						players[playerid].tags["corruption"]="false";
						int addsc=ceil(players[playerid].knowledge*0.5);
						cout << players[playerid].name << "发动了被动技能[蓝勾爷]！做题时各效果再x1.6。\n";
						cout << players[playerid].name << "成绩+" << addsc << "\n";
						addsc=ceil(addsc*1.6);
						players[playerid].add_sc(addsc);
					}
				} else if(spritename=="cyq"){
					if(randint(1,10)<=4){
						cout << players[playerid].name << "发动了被动技能！成绩再-7。\n";
						addsc-=7;
					}
				} else if(spritename=="nmk"){
					cout << players[playerid].name << "发动了被动技能[腐败小助手]！清空本轮腐败效果。\n";
					players[playerid].tags["corruption"]="false";
				}
				players[playerid].add_lf(addlf);
				players[playerid].add_sc(addsc);
			}
			break;
		case 6:
			{
				int chooseplayer=choooseplayer(playerid);
				int addsc_self=-8,addsc_it=-10,addlf_self=2,addlf_it=2;
				addlf_self=ceil(addlf_self*rarity_times[_rarity]);
				addlf_it=ceil(addlf_it*rarity_times[_rarity]);
				cout << players[playerid].name << "生命+" << addlf_self << " 成绩-" << -addsc_self << "\n";
				cout << players[chooseplayer].name << "生命+" << addlf_it << " 成绩-" << -addsc_it << "\n";
				if(spritename=="xza"){
					cout << players[playerid].name << "发动了被动技能！成为卡牌6的目标时成绩改为减去1/4（向下取整）。\n";
					addsc_self=-players[playerid].score/4;
					if(randint(1,10)==1){
						cout << players[playerid].name << "发动了被动技能[透明度]！视为使用卡牌2且不视为腐败。\n";
						players[playerid].tags["corruption"]="false";
						int addsc=ceil(players[playerid].knowledge*0.5);
						cout << players[playerid].name << "成绩+" << addsc << "\n";
						cout << "xza发动了被动技能[蓝勾爷]！做题时各效果再x1.6。\n";
						addsc=ceil(addsc*1.6);
						players[playerid].add_sc(addsc);
					}
				} else if(spritename=="cyq"){
					if(randint(1,10)<=4){
						cout << "cyq发动了被动技能！成绩再-7。\n";
						addsc_self-=7;
					}
				} else if(spritename=="nmk"){
					cout << players[playerid].name << "发动了被动技能[腐败小助手]！清空本轮腐败效果。\n";
					players[playerid].tags["corruption"]="false";
				}
				players[playerid].add_lf(addlf_self);
				players[playerid].add_sc(addsc_self);
				players[chooseplayer].add_lf(addlf_it);
				players[chooseplayer].add_sc(addsc_it);
			}
			break;
		case 7:
			{
				if(spritename=="AJ"){
					cout << "发动被动技能[抓腐]！使用卡牌7对所有人（自己除外）有效。\n";
					for(int i=1;i<=playersum;i++){
						if(i==playerid)	continue;
						if(players[i].tags["corruption"]=="true"){
							cout << players[i].name << "在腐败。生命-3。\n";
							players[i].add_lf(-3);
						} else{
							cout << players[i].name << "没有腐败。生命-1。\n";
							players[i].add_lf(-1);
						}
					}
					break;
				}
				int chooseplayer=choooseplayer(playerid);
				if(sprite_names[players[chooseplayer].spriteid]=="AJ"){
					cout << "AJ发动被动技能！向AJ举报AJ的人生命-2，AJ不受影响。\n";
					players[playerid].add_lf(-2);
					break;
				}
				int addlf=-1;
				if(players[chooseplayer].tags["corruption"]=="true")	addlf-=2;
				players[chooseplayer].add_lf(addlf);
				cout << players[chooseplayer].name << "生命-" << -addlf << "\n";
			}
			break;
		case 8:
			{
				if(spritename=="cyq"){
					cout << players[playerid].name << "发动了被动技能[急眼哥]！使用卡牌8时改为所有其它角色生命-2，自己成绩改为-7；如果角色腐败过，成绩-5。\n";
					cout << players[playerid].name << "成绩-7\n";
					players[playerid].add_sc(-7);
					for(int i=1;i<=playersum;i++){
						if(i==playerid || players[playerid].isdead)	continue;
						cout << players[i].name << "生命-2 ";
						players[i].add_lf(-2);
						if(players[i].tags["corruption"]=="true"){
							cout << "成绩-5";
							players[i].add_sc(-5);
						}
						cout << '\n';
					}
					break;
				}
				int chooseplayer=choooseplayer(playerid);
				int addlf_it=-2,addsc_self=-5,addsc_it=0;
				if(players[chooseplayer].tags["corruption"]=="true")	addsc_it-=5;
				cout << players[playerid].name << "成绩-" << -addsc_self << "\n";
				cout << players[chooseplayer].name << "生命-" << -addlf_it;
				if(addsc_it){
					cout << " 成绩-" << -addsc_it;
				}
				cout << "\n";
				players[playerid].add_sc(addsc_self);
				players[chooseplayer].add_lf(addlf_it);
				players[chooseplayer].add_sc(addsc_it);
			}
			break;
		case 9:
			{
				for(int i=1;i<=playersum;i++){
					if(i==playerid)	continue;
					if(players[i].tags["involution"]=="true"){
						cout << players[i].name << "在内卷\n";
					} else{
						int addsc=-7,addlf=-1;
						cout << players[i].name << "成绩-" << -addsc << " 生命-" << -addlf << "\n";
						players[i].add_lf(addlf);
						players[i].add_sc(addsc);
					}
				}
				if(randint(1,2)==1){
					cout << players[playerid].name << "翻墙被抓了。成绩-3 生命-1\n";
					players[playerid].add_lf(-1);
					players[playerid].add_sc(-3);
				}
			}
			break;
		case 10:
			{
				int chooseplayer=choooseplayer(playerid);
				int jcnums=ceil(3*rarity_times[_rarity]);
				players[chooseplayer].tags["jced"]=to_string(jcnums);
				cout << players[chooseplayer].name << "被机惨了，接下来" << jcnums << "个回合每回合无法行动，且每回合成绩-1。\n";
			}
			break;
		}
		if(players[playerid].isAI){
			Sleep(AI_THINK_MS);
		} else{
			getch();
		}
	}
	void update(){
		for(int i=1;i<=playersum;i++){
			players[i].tags["corruption"]=players[i].tags["involution"]=false;
			players[i].cards.push_back(randcardid());
			players[i].cards.push_back(randcardid());
			
			int jcturns=players[i].jcturn();
			if(jcturns==0){
				for(int j=0;j<=cardnums;j++){
					if(players[i].tags[card_names[i]]!="always_delete")	players[i].tags[card_names[j]]="default";
				}
			} else{
				players[i].tags["jced"]=to_string(--jcturns);
				if(jcturns==0){
					players[i].tags["jced"].clear();
					for(int j=0;j<=cardnums;j++){
						if(players[i].tags[card_names[i]]!="always_delete")	players[i].tags[card_names[j]]="default";
					}
				}
			}
		}
	}
	
	void startcard(int i){
		int op;
		if(nowalive()>1){
			if(sprite_names[players[i].spriteid]=="nmk"){
				cout << players[i].name << "发动被动技能！每轮摸一张[腐败]。";
				players[i].cards.push_back(make_pair(5,randchoose(make_pair(rarityprop,rarityid))));
			}
			if(players[i].jcturn()>=1){
				cout << players[i].name << "被机惨了，所有卡牌被禁用，成绩-1。\n";
				players[i].add_sc(-1);
				Sleep(500);
			} else{
				do{
					get:gameinfo(players[i]);
					cout << "0 停止出牌\t";
					for(int j=0,siz=players[i].cards.size();j<siz;j++){
						pair<int,int> card=players[i].cards[j];
						cout << j+1 << ' ' << card_names[card.first];
						if(raritynames[card.second].size()){
							cout << '[' << raritynames[card.second] << ']';
						}
						cout << '\t';
					}
					cout << '\n';
					if(players[i].isAI==true){
						Sleep(AI_THINK_MS);
						op=randint('0',int(players[i].cards.size()+'0'));
					} else{
						op=getch();
						while(!(op>='0' && op<=int(players[i].cards.size()+'0')))	op=getch();	
					}
					if(op=='0'){
						break;
					} else{
						op=op-'0'-1;
						if(players[i].tags[card_names[players[i].cards[op].first]]!="default"){
							cout << "此卡牌无法使用！\n";
							if(players[i].isAI){
								Sleep(AI_THINK_MS);
							} else{
								getch();	
							}
							goto get;
						}
						usecard(op,i);
					}
				}while(op!='0' && players[i].isdead==false && nowalive()>1);
			}
		}
	}
	void startskill(int i){
		int op;
		gameinfo(players[i]);
		cout << "0 跳过技能使用";
		int skill_cnt=skills[players[i].spriteid];
		for(int j=0;j<skill_cnt;j++){
			cout << j+1 << ' ' << skill_names[players[i].spriteid][j] << '\t';
		}
		cout << '\n';
		if(players[i].isAI){
			Sleep(AI_THINK_MS);
			op=randint('0',skill_cnt+'0');
		} else{
			op=getch();
			while(!(op>='0' && op<=skill_cnt+'0'))	op=getch();
		}
		if(op!='0'){
			op=op-'0'-1;
			cout << players[i].name << "发动了[主动技]" << skill_names[players[i].spriteid][op] << "。\n";
			switch(players[i].spriteid){
			case 1:
				if(op==0){
					//腐败王
					cout << "视为使用一张[腐败]，本轮所有角色无法使用[学新知识点]。\n";
					int addlf=1,addsc=-7;
					cout << players[i].name << "生命+" << addlf << " 成绩-" << -addsc << "\n";
					addlf++;
					addsc-=2;
					cout << players[i].name << "发动了被动技能！使用卡牌5时生命再+1，成绩再-2。\n";
					players[i].add_lf(addlf);
					players[i].add_sc(addsc);
					players[i].tags["corruption"]="true";
					for(int j=1;j<=playersum;j++){
						players[j].tags["学新知识点"]="delete";
					}
				} else if(op==1){
					//制裁
					cout << "自减5点生命，其他角色本轮卡牌[禁止内卷][腐败][一起腐败]失效。\n";
					cout << players[i].name << "生命-5。\n";
					players[i].add_lf(-5);
					for(int j=1;j<=playersum;j++){
						if(j==i)	continue;
						players[j].tags["禁止内卷"]=players[j].tags["腐败"]=players[j].tags["一起腐败"]="delete";
					}
				}
				break;
			case 2:
				break;
			case 3:
				if(op==0){
					//出题人
					cout << "（视作打出[做题]）所有人的成绩加上自己的知识点数量乘以0.1，自己成绩+5。\n";
					cout << players[i].name << "成绩+5\n";
					players[i].add_sc(5);
					for(int j=1;j<=playersum;j++){
						int addsc=ceil(players[j].knowledge*0.1);
						if(sprite_names[players[j].spriteid]=="xza"){
							cout << players[j].name << "发动了被动技能[蓝勾爷]！效果结算后*1.6。\n";
							addsc=ceil(addsc*1.6);
						}
						cout << players[j].name << "成绩+" << addsc << "\n";
						players[j].add_sc(addsc);
						players[j].tags["involution"]=true;
					}
				}
				break;
			case 4:
				if(op==0){
					//水谷
					cout << "本轮无法出[禁止内卷]。已有的[禁止内卷]无效（卡牌[学新知识点][做题][狂人卷题]有效），无视技能。\n";
					for(int j=1;j<=playersum;j++){
						players[j].tags["学新知识点"]=players[j].tags["做题"]=players[j].tags["狂人卷题"]="default";
						players[j].tags["禁止内卷"]="delete";
					}
				}
				break;
			case 5:
				if(op==0){
					//你是个废物！
					int playerid=choooseplayer(i);
					cout << players[i].name << "弃置了" << players[playerid].name << "的所有卡牌[学新知识点][做题][狂人卷题]。\n";
					vector<pair<int,int>> relcard;
					for(auto &x:players[i].cards){
						if(x.first>=4){
							relcard.push_back(x);
						}
					}
					players[i].cards=relcard;
				}
				break;
			}
			if(players[i].isAI){
				Sleep(AI_THINK_MS);
			} else{
				getch();	
			}
		}
	}
	void popcard(int i){
		int op;
		while(players[i].cards.size()>6){
			system("cls");
			cout << "您还需要弃掉 " << players[i].cards.size()-6 << " 张牌。";
			for(int j=0,siz=players[i].cards.size();j<siz;j++){
				pair<int,int> card=players[i].cards[j];
				cout << j+1 << ' ' << card_names[card.first];
				if(card.second>1){
					cout << '[' << raritynames[card.second] << ']';
				}
				cout << '\t';
			}
			cout << '\n';
			if(players[i].isAI){
				Sleep(AI_THINK_MS);
				op=randint('1',int(players[i].cards.size()+'0'));
			} else{
				op=getch();
				while(!(op>='1' && op<=int(players[i].cards.size()+'0')))	op=getch();
			}
			players[i].used(op-'1');
		}
	}
	player gamemain(){
		while(nowalive()>1){
			update();
			for(int i=1;i<=playersum;i++){
				system("cls");
				gameinfo(players[i]);
				if(players[i].isdead){
					cout << players[i].name << "死了。\n";
					Sleep(500);
					continue;
				}
				startcard(i);
				startskill(i);
				popcard(i);
			}
		}
		for(int i=1;i<=playersum;i++){
			if(!players[i].isdead){
				return players[i];
			}
		}
		return player();
	}
};
auto pgroup=Group();
map<string,pair<int,int>> players_calc;//统计玩家的 key=名字 first=赢得局数 second=总局数 
vector<pair<string,vector<string>>> deletecards;//被禁用的卡牌
void player::add_lf(int delta){
	life=min(life+delta,20);
	if(life>0 && life<=2 && spriteid==6 && tags["noend"].size()){
		string s=tags["noend"];
		if(s=="1"){
			tags["noend"]=string();
		} else{
			tags["noend"]=to_string(stoi(s)-1);
		}
		cout << "AJ发动了触发技能[不死之身]！请选择：\n";
		cout << "[1]生命+2\t[2]死去，但是选择A，他/她生命-5，成绩归0。\n";
		int op;
		if(isAI){
			Sleep(AI_THINK_MS);
			op=randint('1','2');
		} else{
			op=getch();
			while(!(op=='1' || op=='2'))	op=getch();
		}
		if(op=='1'){
			add_lf(2);
		} else{
			isdead=true;
			cout << "请选择：";
			int op=pgroup.choooseplayer(0);
			pgroup.players[op].add_lf(-5);
			pgroup.players[op].score=0;
		}
	}
	if(life<0 || isdead){
		cout << name << "死了。\n";
		isdead=true;
	}
}
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
			cerr << "不支持导入此配置文件！\n";
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
		getch();
		goto start;
	} else if(x==3){
		system("cls");
		cout << "姓名\t赢的对局\t总对局\t胜率\n";
		for(map<string,pair<int,int>>::iterator it=players_calc.begin();it!=players_calc.end();++it){
			cout << it->first << '\t' << it->second.first << '\t' << it->second.second << '\t' << 100.0*(it->second.first)/(it->second.second) << "%\n";
		}
		getch();
		goto start;
	}
	else{
		goto start;
	}
	return 0;
}
