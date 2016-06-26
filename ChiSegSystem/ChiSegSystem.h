#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <map>
#include <set>
#include <vector>
#include <windows.h>

using namespace std;

//定义马尔科夫模型结构

typedef struct  
{
	int hidden_states_cnt;//隐含状态的数目，在本中文分词中为2，但是语音识别中，该数目每个层次均不一样
	int observestates_cnt;//观察到的序列长度，即观察状态数目，在分词中并不存在观察序列，或者说发射概率为1，不需考虑了
	double* strat_vector;//初始概率向量
	double** transfer_matrix;//转移概率矩阵
	//理应还有一个混淆矩阵，但本系统中均为一，所以不予考虑
}HMM;
//定义HMM单个节点结构
typedef struct 
{
	string path;
	float currentMaxValue;

}hmmNode;
//定义分支熵结构
typedef struct{
	float lbe;
	float rbe;
}BE;

//词图节点结构
struct Node{
	string node_word;
	float score;
	int n;//表征该节点存储的是几元词汇
	float current_maxscore;
	//vector<string> path;
	bool operator < (const Node& a) const
	{
		//按node_word由大到小排列
		return a.node_word < node_word;
	}

};

//词图单个层次结构
typedef set<Node> SingleHierachy;


typedef vector<string> Sentence;
typedef vector<Sentence> Doc;
typedef map<string,float> Dictionary;

class ChiSeg{
private:
	Dictionary dictionary[3];
	HMM hmm;
	vector<SingleHierachy> hierachy;//词图结构，按照层次组织
public:
	//生成三个词典
	bool GenerateDictionarys(string filepaths[3]);
	//查询某个词汇的互信息
	float GetTFDevValues(string word, int n);
	//生成马尔科夫模型
	void GenerateHMM(Sentence sentence);
	//Viterbi算法
	//float ViterbiStyleAlgorithm(vector<string> part_sentence,stack<string> code_result);
	string ViterbiStyleAlgorithm(Sentence sentence);
	//生成分词词图
	void GenerateGraph(Sentence sentence);
	//生成HMM转移矩阵
	void GenHMMTransMat(Sentence sentence, float* transferMar);
	//主算法流程

	bool MainProcess(const char* pun_file_path,const char* one_gram_filepath,string dictionary_filepaths[3],string source_file,string result_file);

};

class PreProcessing{
private:
	//标点符号集合
	set<string> m_puncSet;
	//原始输入的句子
	string original_sentence;
	//一元词汇词典
	set<string> one_gram_dic;
public:
	//一句话的标点符号保存链表
	vector<string> pun_list;
	//初始化，传入标点文件和一元词表文件
	bool InitPrePro(const char*  pun_file_path,const char* one_gram_filepath);
	//解析一行
	void ParseLine(string& str, Doc& doc);
	//保存一个词汇进入文章结构中
	void SaveWord(const string& word, Doc& doc);

	//将UTF-8转化为gbk
	char* U2G(const char* utf8);

};