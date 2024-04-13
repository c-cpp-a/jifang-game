#ifndef _DEFS_HEADER_
#define _DEFS_HEADER_
#include<string>
#include<vector>
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
const vector<double> cardprop={0,1.7,1.3,0.9,1,0.8,1.1,1.3,1,1,0.7};//卡牌概率权重
const vector<int> carddropid={0,1,2,3,4,5,6,7,8,9,10};
const vector<double> rarityprop={90,30,10,3,2};//稀有度概率权重（2/3:2/9:2/27:1/45:2/135） 
const vector<int> rarityid={0,1,2,3,4};
constexpr double rarity_times[5]={1,1.66,2.7889,4.657463,7.77796321};//稀有度对有益效果的加成 
constexpr int AI_THINK_MS=200;//AI思考毫秒数
#endif
