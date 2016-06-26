#include "ChiSegSystem.h"
void Average()
{
	float sum = 0;
	float temp = 0;
	int word_count = 0;
	ifstream in;
	in.open("E:\\coding\\ChineseSegment\\�����ļ�\\MI-marked-seg-accuracy.txt", ios::in);
	cout << "\n Reading File " << endl;
	if (!in.good())
	{
		cout << "Open file failed ";
		return;
	}
	// ��ȡ�ļ�
	while (!in.eof())
	{
		in >> temp;
		sum += temp;
		word_count++;
	}
	in.close();
	ofstream out;
	out.open("E:\\coding\\ChineseSegment\\�����ļ�\\TestResult.txt", ios::out | ios::app);
	out << "0.01" << "\t" << fixed << sum / word_count << endl;
	out.close();
}

int main(int argc, char* argv[])
{
	
/*
	if (5 != argc)
	{
		cout << "Usage: BreakEntropy.exe <Seg File> <Punc File> <n-gram> <result File> " << endl;
		return 1;
	}
D:\\
	int n =atoi(argv[3]);
	if ((2 != n) && (3 != n)&&(1 != n))
	{
		cout << "Parameter error: n-gram = 2 or 3" << endl;
		return 1;
	}

	new CCalculator(argv[1], argv[2], n, argv[4]);

	return 0;*/
	ChiSeg *chiseg=new ChiSeg();
	const char* pun_file_path="E:\\coding\\ChineseSegment\\�����ļ�\\punc.txt";
	const char* one_gram_filepath="E:\\coding\\ChineseSegment\\�����ļ�\\1gram.txt";
	//string dictionary_filepaths[3] = { "E:\\coding\\ChineseSegment\\�����ļ�\\----\\TFDev�ļ�\\MaxMin��׼��\\Result-1Gram.txt", "E:\\coding\\ChineseSegment\\�����ļ�\\----\\TFDev�ļ�\\MaxMin��׼��\\Result-2Gram.txt", "E:\\coding\\ChineseSegment\\�����ļ�\\----\\TFDev�ļ�\\MaxMin��׼��\\Result-3Gram.txt" };
	string dictionary_filepaths[3]={"E:\\coding\\ChineseSegment\\�����ļ�\\----\\TFDev�ļ�\\��ȥƽ��ֵ��׼��\\Result-1Gram.txt","E:\\coding\\ChineseSegment\\�����ļ�\\----\\TFDev�ļ�\\��ȥƽ��ֵ��׼��\\Result-2Gram.txt","E:\\coding\\ChineseSegment\\�����ļ�\\----\\TFDev�ļ�\\��ȥƽ��ֵ��׼��\\Result-3Gram.txt"};
	//string dictionary_filepaths[3] = { "E:\\coding\\ChineseSegment\\ChiSeg-GitHub\\ChiSeg-GitHub\\Normlize\\Normlize\\Result-Way2-1Gram.txt", "E:\\coding\\ChineseSegment\\ChiSeg-GitHub\\ChiSeg-GitHub\\Normlize\\Normlize\\Result-Way2-2Gram.txt", "E:\\coding\\ChineseSegment\\ChiSeg-GitHub\\ChiSeg-GitHub\\Normlize\\Normlize\\Result-Way2-3Gram.txt" };
	//string dictionary_filepaths[3] = { "E:\\coding\\ChineseSegment\\ChiSeg-GitHub\\ChiSeg-GitHub\\Normlize\\Normlize\\Result-Way3-1Gram.txt", "E:\\coding\\ChineseSegment\\ChiSeg-GitHub\\ChiSeg-GitHub\\Normlize\\Normlize\\Result-Way3-2Gram.txt", "E:\\coding\\ChineseSegment\\ChiSeg-GitHub\\ChiSeg-GitHub\\Normlize\\Normlize\\Result-Way3-3Gram.txt" };
	//string dictionary_filepaths[3]={"E:\\coding\\ChineseSegment\\�����ļ�\\----\\MI��׼���ļ�\\Result-1Gram.txt","E:\\coding\\ChineseSegment\\�����ļ�\\agrMI_2gram.txt","E:\\coding\\ChineseSegment\\�����ļ�\\agrMI_3gram.txt"};


	//string source_file="E:\\coding\\ChineseSegment\\�����ļ�\\agr_seg.txt";
	string source_file = "E:\\coding\\ChineseSegment\\���������Ϣ\\5000���\\ũҵ\\���\\ICTCLAS�з��ı�.txt";
	string result_file="E:\\coding\\ChineseSegment\\�����ļ�\\MI-marked-seg-result��ũҵ��������зֽ����.txt";
	chiseg->MainProcess(pun_file_path,one_gram_filepath,dictionary_filepaths,source_file,result_file);
	system("E:\\coding\\ChineseSegment\\GetICALASCodes\\Release\\GetICALASCodes.exe");
	system("E:\\coding\\ChineseSegment\\TestAccuracy\\Release\\TestAccuracy.exe");
	Average();
}