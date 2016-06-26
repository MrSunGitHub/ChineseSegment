// ChiSegSystem.cpp : 定义控制台应用程序的入口点。
//
#include "ChiSegSystem.h"

/*实现预处理类*/

bool PreProcessing::InitPrePro(const char*  pun_file_path,const char* one_gram_filepath)
{
	/*加载标点符号文件*/

	if (NULL == pun_file_path)
	{
		cout << "Parameter is NULL ";
		return false;
	}
	ifstream in;
	in.open(pun_file_path, ios::in);
	if (! in.good())
	{
		cout << "Open file failed ";
		return false;
	}
	// 读取文件
	m_puncSet.clear();
	string word;
	float value;
	while (getline(in, word))
	{
		m_puncSet.insert(word);

	}
	in.close();

	/*加载一元词表文件*/

	if (NULL == one_gram_filepath)
	{
		cout << "Parameter is NULL ";
		return false;
	}
	in.open(one_gram_filepath, ios::in);
	if (! in.good())
	{
		cout << "Open file failed ";
		return false;
	}
	while (!in.eof())
	{
		//循环读取文件并加载入词典
		in>>word>>value>>value;
		one_gram_dic.insert(set<string>::value_type(word));
	}
	in.close();
	return true;
}

//解析一行
void PreProcessing::ParseLine(string& str, Doc& doc)
{
	str = U2G(str.c_str());
	//清除前一行保存的标点
	pun_list.clear();
	doc.clear();
	int sen_cnt = 0;
	size_t s = 0;
	size_t t = string::npos;
	string word;
	while ((t = str.find_first_of(' ', s)) != string::npos)
	{
		if (t > s)
		{
			//截出一个词来
			word = str.substr(s, t-s);
			//该词存入文章中
			SaveWord(word, doc);
		}
		//更新扫描指针位置
		s = t+1;
	}
	word = str.substr(s);//substr未指定第二个参数，将截取到字符串的末尾，以为最后一个词后面是没有空格的
	if (! word.empty())
		SaveWord(word, doc);
}

//************************************     
// 函数名称: SaveWord     
// 函数说明：     
// 作  者：Mr.SUN     
// 作成日期：2015/07/23     
// 返 回 值: void     
// 参    数: const string & word     
// 参    数: Doc & doc     
//************************************
void PreProcessing::SaveWord(const string& word, Doc& doc)
{
	set<string>::iterator iter=m_puncSet.find(word);
	if (iter == m_puncSet.end())
	{
		// 非标点：保存
		if (0 == doc.size())
		{
			doc.push_back(Sentence());
		}
		set<string>::iterator iter = one_gram_dic.find(word);
		if (iter == one_gram_dic.end())
			cout << "Unrecognized word: [" << word << "]" << endl;
		else
			//将该词的整数编码存储
			doc[doc.size()-1].push_back(*iter);
	}
	else
	{
		// 标点：跳过（首先清除上一个空句）
		size_t sen_cnt = doc.size();
		if ((sen_cnt > 0) && (doc[sen_cnt-1].size() == 0))//doc[sen_cnt-1].size()表示是个空句
		{
			doc.erase(doc.begin()+sen_cnt-1);
		}
		pun_list.push_back(*iter);//为了保存一个句子中的标点符号
		//标点的话生成一个新的句子
		doc.push_back(Sentence());
	}
}

//************************************     
// 函数名称: U2G     
// 函数说明：     
// 作  者：Mr.SUN     
// 作成日期：2015/07/23     
// 返 回 值: char*     
// 参    数: const char * utf8     
//************************************
char* PreProcessing::U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}



//************************************     
// 函数名称: GenerateDictionarys     
// 函数说明：  生成查询N元分支熵的字典结构   
// 作  者：Mr.SUN     
// 作成日期：2015/07/22     
// 返 回 值: bool  即字典生成 成功与否   
// 参    数: string filepaths[3]   三个字典文件路径  
//************************************
bool ChiSeg::GenerateDictionarys(string filepaths[3])
{
	string word,temp_word;
	float be=0.0f;
	unsigned long long word_cnt=0;//计算读取了多少词汇的计数器
	float dicMin = 0.0f;//保存词典当中的最小值，方便平滑
	cout<<"Generating Dictionaries";
	//循环读取三个文件并加载入词典中
	for (int i=0;i< 3;i++)
	{
		dicMin = 5;
		word_cnt = 0;
		ifstream in;
		in.open(filepaths[i].c_str(), ios::in);
		if (! in.good())
		{
			cout << "Open file failed ";
			return false;
		}
		// 读取文件
		while(!in.eof())
		{
			word="";//先清空字符串保存结果
			//读取字符串信息
			for (int j=0;j<=i;j++)
			{
				in>>temp_word;
				word+=temp_word;
			}
			in  >> be;
/*
此处若采用了标准化方法进行分词，不需要对一元的TF单独处理了
			if (i==0)
			{
				//一元的词汇单独处理
				//be = be/100 ;
				be = 0.05;
			}*/
			dicMin = dicMin < be ? dicMin: be;
			//将数加载进入相应的字典中
			dictionary[i].insert(map<string ,float>::value_type(word,be*(i+1)));
			//显示处理进度的标示

			if (word_cnt++%20000 == 0)
			{
				cout<<".";
			}
		}
		//将最小值存储，方便后面进行平滑
		dictionary[i].insert(map<string, float>::value_type("min", dicMin*(i + 1)));
		in.close();
	}
	return true;
}


/**
*函数名:GenHMMTransMat
*功能:输入的句子，根据词典生成转移概率矩阵
*参数说明:  Sentence sentence
*参数说明:  float * transferMar
*返回值:void
*说明:
*日期:2016-4-24-15:38
*作者:MR.SUN
*/void ChiSeg::GenHMMTransMat(Sentence sentence, float* transferMar)
{

}


//************************************     
// 函数名称: GenerateHMM     
// 函数说明：     
// 作  者：Mr.SUN     
// 作成日期：2015/07/22     
// 返 回 值: bool     
// 参    数: Sentence sentence     
//************************************
void ChiSeg::GenerateHMM(Sentence sentence)
{
	//初始化模型的各个参数

	//观察序列是插空的空格数目
	hmm.observestates_cnt=sentence.size()-1;
	//隐含序列每列两个
	hmm.hidden_states_cnt=hmm.observestates_cnt*2;
	//对转移矩阵进行赋值

	//


}

void ChiSeg::GenerateGraph(Sentence sentence)
{
	Node newNode;//
	hierachy.clear();
	SingleHierachy single;
	//应用上面形成的原始切分句子形成分词词图
	int sentenceLength=sentence.size();
	//添加特定量的层次
	for (int j=0;j<sentenceLength;j++)
	{
		hierachy.push_back(single);
	}
	//N元扫描控制变量
	for (int i=0;i<sentenceLength;i++)
	{
		//形成所有可能的切分状态，存入相应的层次中
		for (int j=i;j<sentenceLength&&(j-i)<3;j++)
		{
			//(j-i)=1时表示是二元词，id=sentence[j-1]*maxID+sentence[j]
			//(j-i)=2时表示是三元词，id=sentence[j-2]*maxID+sentence[j-1]8maxid+sentence[j]
			if(j-i == 0)
			{
				newNode.node_word = sentence[j];
				newNode.n=1;
			}
			if (j-i == 1)
			{
				newNode.node_word = sentence[j-1]+sentence[j];
				newNode.n=2;
			}
			if (j-i == 2)
			{
				newNode.node_word = sentence[j-2]+sentence[j-1]+sentence[j];
				newNode.n=3;
			}	
			//字典获取该节点的权值
			newNode.score = GetTFDevValues(newNode.node_word, j - i + 1);
			//节点最大值默认为该节点的权值
			newNode.current_maxscore=newNode.score;
			//节点插入该层
			hierachy[j].insert(newNode);
		}
	}

}


//************************************     
// 函数名称: ViterbiStyleAlgorithm     
// 函数说明：  维特比算法寻找最优路径   
// 作  者：Mr.SUN     
// 作成日期：2015/07/23     
// 返 回 值: std::string     
// 参    数: Sentence sentence     
//************************************
string ChiSeg::ViterbiStyleAlgorithm(Sentence sentence)
{
	Node temp;
	int senLength=sentence.size();
	vector<Node> optimalCandidates(senLength);
	for (int i=0;i<senLength;i++)
	{
		//初始化当前层次的最优值,因此处采用了减去平均值的方式归一化，根据统计最小值应为-2
		optimalCandidates[i].current_maxscore=-200;
		//更新一层中的节点值
		SingleHierachy single=hierachy[i];
		SingleHierachy::iterator iter;
		for (iter=single.begin();iter!=single.end();iter++)
		{
			int keyLength=iter->n;
			int flag=i-keyLength;
			if (flag<0)//回溯至词图开头
			{
				temp=*iter;
				//temp.path.push_back(iter->key.key);		
				temp.current_maxscore=iter->score;
				hierachy[i].erase(*iter);
				hierachy[i].insert(temp);
			}
			else//未回溯至词图开头
			{
				temp=*iter;
				Node backOptimum = optimalCandidates[flag];
				float backOptimumScore=backOptimum.current_maxscore;
				temp.current_maxscore+=backOptimumScore;				
				hierachy[i].erase(*iter);
				hierachy[i].insert(temp);
			}
			if (temp.current_maxscore>=optimalCandidates[i].current_maxscore)
			{
				optimalCandidates[i]=temp;
				/*optimalCandidates[i].current_maxscore=temp.current_maxscore;
				optimalCandidates[i].n=temp.n;*/
			}
		}
	}

	//输出最终的切分结果
	//栈结构暂时保存结果
	stack<string> results_stack;
	string result="";
	int sen_cnt=senLength-1;
	while (sen_cnt >=0)
	{
		results_stack.push(optimalCandidates[sen_cnt].node_word);
		sen_cnt-=optimalCandidates[sen_cnt].n;
		if (results_stack.size()>50)
		{
			cout<<"即将进入死循环------"<<endl;
			break;
		}
	}
	while (!results_stack.empty())
	{
		result+=results_stack.top()+" ";
		results_stack.pop();
	}
	//为了去除末尾的空格
	return result.substr(0,result.length()-1);

}
	
float ChiSeg::GetTFDevValues(string word, int n)
{
	if (n==1)
	{
		return 0.047;
	}
	else
	{
		map<string, float>::iterator iter = dictionary[n - 1].find(word);
		if (iter == dictionary[n - 1].end())
			return dictionary[n - 1]["min"] - 0.1;//平滑方法是用最小值减去0.1
		else
			return dictionary[n - 1][word];
	}
	
}





//************************************     
// 函数名称: ViterbiStyleAlgorithm     
// 函数说明：     
// 作  者：Mr.SUN     
// 作成日期：2015/07/23     
// 返 回 值: float     
// 参    数: vector<string> part_sentence     传入的已经断好的句子
// 参    数: stack<string> code_result     暂存递归结果的栈
//************************************
/*
float ChiSeg::ViterbiStyleAlgorithm(vector<string> part_sentence,vector<string> code_result[3])
{
	int sen_len=part_sentence.size();
	//若是0个词，返回1
	if (sen_len == 0)
	{
		return 1;
	}
	//若是单个词，返回其本身的分支熵
	else if (sen_len == 1)
	{
		return GetBEValues(part_sentence[0],1);
	}
	//若是双词，只需比较两种的大小即可
	else if (sen_len == 2)
	{
		//拼接二元词汇
		string find_word=part_sentence[0]+" "+part_sentence[1];
		float value1=GetBEValues(find_word,2);
		//移除part_sentence第一个元素，作为递归的参数
		part_sentence.erase(part_sentence.begin());
		float value2=GetBEValues(part_sentence[0],1)*ViterbiStyleAlgorithm(part_sentence);
	}
	//词汇多于三元
	else
	{
		float value[3];
		string find_word="";
		for (int i=0;i<3;i++)
		{
			find_word+=part_sentence[0];
			part_sentence.erase(part_sentence.begin());
			value[i] = GetBEValues(find_word,i+1)*ViterbiStyleAlgorithm(part_sentence,code_result);
		}
		//返回最大值
		float max_value=value[0];
		if (value[1] >max_value)
		{
			max_value = value[1];
			return max_value;
		}
		else if (value[2] >max_value)
		{
			max_value =value[2];
			return max_value;
		}
		else
		{

			return max_value;
		}
		
		
	}
}*/


bool ChiSeg::MainProcess(const char* pun_file_path,const char* one_gram_filepath,string dictionary_filepaths[3],string source_file,string result_file)
{
	//初始化过程
	PreProcessing *pre=new PreProcessing();
	// 参数验证
	if (NULL == pun_file_path || NULL == one_gram_filepath)
	{
		cout<< "Parameter is NULL ";
		return false;
	}
	//初始化预处理类
	pre->InitPrePro(pun_file_path,one_gram_filepath);

	//加载词典

	GenerateDictionarys(dictionary_filepaths);
	ofstream out;
	out.open(result_file.c_str(), ios::out);
	if (! out.good())
	{
		cout << "Open result file failed ";
		return false;
	}
	ifstream in;
	in.open(source_file.c_str(), ios::in);
	if (! in.good())
	{
		cout << "Open file failed ";
		return false;
	}
	int doc_cnt = 0;
	string buff;
	Doc doc;
	string final_result;
	while (getline(in, buff))
	{
		int pun_cnt=0;
		pre->ParseLine(buff, doc);				// 解析当前行
		//循环该文章的每一句进行切分（首先形成词图，然后维特比）
		for (unsigned int i=0; i<doc.size(); i++)
		{
			if (doc[i].size()!= 0)
			{
				final_result="";
				//构造切分词图
				GenerateGraph(doc[i]);
				//维特比算法寻找最优路径
				final_result=ViterbiStyleAlgorithm(doc[i]);
				/*do 
				{
					final_result+=pre->pun_list[pun_cnt];
					pun_cnt++;
				} while (pre->pun_list[pun_cnt]=="|||");*/
				out<<final_result;
			}						
	    }
		//out<<endl<<buff;
		out<<"\n";
		if ((doc_cnt++)%1000==0)
		{
			cout<<".";
		}		
	}
	in.close();
	out.close();
}

