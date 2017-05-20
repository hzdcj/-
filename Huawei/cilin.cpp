#include "cilin.h"
#include "stdio.h"
#include <iostream>  
#include <fstream>  
#include <stdlib.h>  
#include <map>
#include <string>
#include <math.h>
#include<algorithm>
#include<ctime>
const char* path = "cilin.txt";
cilin::cilin()                                              
{
    read_cilin(path);
}
cilin::~cilin()
{
}
void cilin::calculate_sim(unordered_map<string, int>&label)
{
	relative_label.clear();                                      //���relative_label
	for (auto it1 = label.begin(); it1 != label.end();)
	{
		for (auto it2 = it1; it2 != label.end();)
		{
			if (it1 == it2)
			{
				++it2;
				continue;
			}
			float score = similarity(it1->first, it2->first);
			if (score == -1.0f)                                              //����ʵ���û�У�ֱ��������ѭ��
			{
				++it1;
				break;
			}
			if (score > 0.8)                               //������ƶȳ���0.8,��ǩ�ϲ�
			{
				int num1= it1->second;
				int num2 = it2 ->second;
				if (num1 >= num2)
				{
					relative_label[it1->first].push_back(it2->first);
					it1->second += it2->second;
					label.erase(it2++);
				}
				else
				{
					relative_label[it2->first].push_back(it1->first);
					it2->second += it1->second;
					label.erase(it1++);
					it2 = it1;
				}
				continue;
			}
			++it2;
		}
		++it1;
	}
	sort_label(label);
}
bool cmpe(const pair<string, float>&a, const pair<string, float>&b)
{
	return a.second > b.second;
}
void up_to_down(vector<pair<string,int>>&a, int i,int &K)
{
	int pos;
	int t1 = 2 * i;                 //���ӽڵ�
	int t2 = t1 + 1;               //�Һ��ӽڵ�
	if (t1 > K)
		return;
	else
	{
		if (t2 > K)
			pos = t1;
		else
			pos = a[t1].second > a[t2].second ? t2 : t1;         //�Ƚ����Һ��ӽڵ��С��ȡ��С���Ǹ�
		if (a[i].second > a[pos].second)
		{
			pair<string,int> tmp = a[i];
			a[i] = a[pos];
			a[pos] = tmp;
			up_to_down(a, pos, K);
		}
	}
}
void create_heap(vector<pair<string, int>>&a, int &K)               //������ʱ�����濪ʼ
{
	int pos = K / 2;
	for (int i = pos; i >= 1; --i)
	{
		up_to_down(a, i, K);
	}
}
void cilin::sort_label(unordered_map<string, int>&label)
{
	//vector<pair<string, float>>temp(label.size());
	//int total_num = 0;
	//for (auto it = label.begin(); it != label.end();++it)              //�ܼ��ܵı�ǩ��
	//{
	//	total_num += it->second;
	//}
	//int i = 0;
	//for (auto it = label.begin(); it != label.end(); ++it)
	//{
	//	float rate = (float)it->second / total_num;                  //�����ǩռ��
	//	temp[i++] = make_pair(it->first, rate);
	//}
	//vector<string>temp_label;
	//sort(temp.begin(), temp.end(), cmpe);
	//int size = min(10, (int)temp.size());                             //���10����ǩ
	//for (int i = 0; i < size; ++i)
	//{
	//	if (i>5 && temp[i].second < 0.5)                    
	//		break;
	//	temp_label.push_back(temp[i].first);
	//}
	//swap(final_label, temp_label);                                  //�õ����յı�ǩ,�����ظ�ʹ��
	int total_num = 0;
	for (auto it = label.begin(); it != label.end();++it)              //�ܼ��ܵı�ǩ��
	{
		total_num += it->second;
	}
	bool flag = false;                                              //�������С���ѵı�־,ͳ��ǰ5�ı�ǩ
	vector<pair<string,int>>frequency;
	frequency.push_back(make_pair("", -1));
	int min_heap_size = 10;                                            
	for (auto it = label.begin(); it != label.end(); ++it)
	{
		if (frequency.size() < min_heap_size+1)
		{
			frequency.push_back(*it);
		}
		else
		{
			if (!flag)
			{
				create_heap(frequency, min_heap_size);
				flag = true;
			}
			else
			{
				if (it->second > frequency[1].second)
				{
					frequency[1] = *it;
					up_to_down(frequency, 1, min_heap_size);
				}
			}
		}
	}
	vector<string>temp_label;
	for (int i = 1; i < frequency.size(); ++i)                           
	{ 
		if (frequency[i].second * 10 > total_num)                      //a*10>total_num�Ĳ����ȥ
			temp_label.push_back(frequency[i].first);
	}
	swap(final_label, temp_label);                                  //�õ����յı�ǩ,�����ظ�ʹ��

}
//����ʵ����ƶ�
float cilin::similarity(const string& w1,const string& w2){
	word_map_t::iterator it1 = m_word_code_map.find(w1);
	word_map_t::iterator it2 = m_word_code_map.find(w2);
	if(it1 == m_word_code_map.end())
		return -1 ;
	if (it2 == m_word_code_map.end())
		return -2;

	//�����ʿ��ܶ�Ӧ�������
	vector<string>& code1_vec = it1->second;
	vector<string>& code2_vec = it2->second;

	float sim_max = 0;
	float sim_cur = 0;
	for (int i = 0; i < code1_vec.size(); ++i)
	{
		for (int j = 0; j < code2_vec.size(); ++j){
			sim_cur = sim_by_code(code1_vec[i],code2_vec[j]);
			if(sim_max < sim_cur)
				sim_max = sim_cur;
		}
	}
	return sim_max;
}

//�����ļ��������ʼ�
bool cilin::read_cilin(const char* path){
	if(!path)
		return false;

	char buffer[8192];  
	ifstream in(path,ifstream::in);  
	if (! in.is_open()){ 
		cout << "Error opening file"; 
		exit (1); 
	}  

	vector<string> split_vec;
	split_vec.reserve(32);
	string delim = " ";
	int line_count = 0;

	while (!in.eof() ){  
		in.getline (buffer,sizeof(buffer));  
		string src(buffer);

		split_vec.clear();
		split(src,delim,split_vec);
		if(split_vec.size() < 2)			
			break;

		line_count++;
		/*word_map_t::iterator it = m_code_word_map.find(split_vec[0]);
		if(it == m_code_word_map.end())*/
			m_code_word_map[split_vec[0]].assign(split_vec.begin() + 1 ,split_vec.end());
		//else

		vector<string>::iterator it = split_vec.begin() + 1;
		for (; it != split_vec.end(); ++it)
		{
			string& w = *it;		
			m_word_code_map[w].push_back(split_vec[0]);
		}

		get_code_father_child(split_vec[0]);
	}  
	printf("end reading...\n");
	return true;
};

void cilin::split_code_layer(const string& code,vector<int>& layers,vector<string>& fathers){
	if(code.size() < 8){
		//error code;
		return;
	}
	layers.clear();
	fathers.clear();

	layers.push_back(code[0]);
	fathers.push_back(code.substr(0,1));

	layers.push_back(code[1]);
	fathers.push_back(code.substr(0,2));
	
	int l = atoi(code.substr(2,2).c_str());
	layers.push_back(l);
	fathers.push_back(code.substr(0,4));

	layers.push_back(code[4]);
	fathers.push_back(code.substr(0,5));

	l = atoi(code.substr(5,2).c_str());
	layers.push_back(l);	
}

//�õ�����ĵ�n������,������k
int cilin::get_layer_by_no(const string& code,int n){
	if(code.size() < 8){
		//error code;
		return -1;
	}
	
	switch(n){
	case 0:
	case 1:
		return code[n];
		break;
	case 2:
		return atoi(code.substr(2,2).c_str());
		break;
	case 3:
		return code[4];
		break;
	case 4:
		return atoi(code.substr(5,2).c_str());
		break;
	}
	return -1;
}

//�游�ӹ�ϵ����߷�����nЧ��
void cilin::get_code_father_child(const string& code){
	vector<int> layer_vec;
	vector<string>father_key_vec;

	split_code_layer(code,layer_vec,father_key_vec);
	if(layer_vec.size() < 5 || father_key_vec.size() < 4)
		return;

	for (int i = 0; i < 4; ++i){
		string& key = father_key_vec[i];
		//m_code_word_map[key].insert(layer_vec[i+1]);

		relation_code_map_t::iterator it_fc = m_code_father_child_map.find(key);
		if(it_fc == m_code_father_child_map.end()){
			set<int> child_set;
			child_set.insert(layer_vec[i+1]);
			m_code_father_child_map.insert(make_pair(key,child_set));
		}
		else
			m_code_father_child_map[key].insert(layer_vec[i+1]);
	}
}

string& cilin::trim(string &s){  
	if (s.empty()){  
		return s;  
	}  

	s.erase(0,s.find_first_not_of(" "));  
	s.erase(s.find_last_not_of(" ") + 1);  
	return s;  
}  

void cilin::split(const string& s, const string& delim,vector<string >& ret)  
{  
	size_t last = 0;  
	size_t index=s.find_first_of(delim,last);  
	while (index!=std::string::npos)  
	{  
		ret.push_back(s.substr(last,index-last));  
		last=index+1;  
		index=s.find_first_of(delim,last);  
	}  
	if (index-last>0)  
	{  
		ret.push_back(s.substr(last,index-last));  
	}  
}  



//�������ƶȵĹ�ʽ����ͬ�Ĳ�ϵ����ͬ
float cilin::sim_formula(float coeff,int n,int k){
	return coeff * cos(n * PI / CILIN_DEGREE) * ((n - k + 1) / float(n));
}

//���ݱ���������ƶ�
float cilin::sim_by_code(const string& c1,const string& c2){
	if(c1.empty() || c2.empty())
		return .0f;

	char c1_last_ch = c1[c1.length() - 1];
	char c2_last_ch = c2[c2.length() - 1];

	// �����һ��������'@'��β����ô��ʾ���ҷ�գ����������ֻ��һ���ʣ�ֱ�ӷ���f
	if(c1_last_ch == '@' || c2_last_ch == '@')
		return CILIN_F;

	string common_str = get_common_str(c1,c2);
	if(common_str.empty())
		return .0f;

	int len_common = common_str.length();
	// ���ǰ���߸��ַ���ͬ����ڰ˸��ַ�Ҳ��ͬ��ҪôͬΪ'='��ҪôͬΪ'#''
	if(len_common >= 7){
		if(c1_last_ch != c2_last_ch)
			return .0f;

		if(c1_last_ch == '=')
			return 1.0f;

		if(c1_last_ch == '#')
			return CILIN_E;
	}

	int k = get_k(c1,c2,common_str);
	int n = get_n(common_str);

	//printf("  %s,%s,n = %d,k = %d  ",c1.c_str(),c2.c_str(),n,k);
	switch(len_common){
	case 0://�����������ͬһ������
		return CILIN_F;
	case 1:
		return sim_formula(CILIN_A, n, k);
	case 2:	
		return sim_formula(CILIN_B, n, k);
	case 4:	
		return sim_formula(CILIN_C, n, k);
	case 5:	
		return sim_formula(CILIN_D, n, k);
	}

	return .0f;
}

//�������ڷ�֧��ķ�֧��
//�������֧�ĸ��ڵ��ܹ��ж��ٸ��ӽڵ�
//���������common_str���������ǹ�ͬ������һ��
//���磬���ǵ�common_strΪǰ���㣬�����ǹ�ͬ���ڵ����㣬������ͳ��ǰ����Ϊcommon_str�ĵ������������ͺ���
int  cilin::get_n(const string& common_str){

	if(common_str.empty())
		return 0;

	relation_code_map_t::iterator it = m_code_father_child_map.find(common_str);
	if(it != m_code_father_child_map.end()){
		return it->second.size();
	}
	return 0;
}

//�������������Ӧ��֧�ľ��룬���ھ���Ϊ1
int cilin::get_k(const string&c1,const string& c2, const string& common_str){
	if(c1.empty() || c2.empty() || common_str.empty())
		return -1;
	int len_common = common_str.size();
	int cur_layer_no = 0;
	
	if(len_common == 1 || len_common == 2)
		cur_layer_no = len_common;
	else if(len_common == 4)
		cur_layer_no = 3;
	else if(len_common == 5)
		cur_layer_no = 4;
	else if(len_common == 7)
		cur_layer_no = 5;
	else return -1;

	int c1_layer_no = get_layer_by_no(c1,cur_layer_no);
	int c2_layer_no = get_layer_by_no(c2,cur_layer_no);
	return abs(c1_layer_no - c2_layer_no);
}

string cilin::get_common_str(const string& c1,const string& c2){
	string common_str;
	for (int i = 0; i < c1.length(); ++i)
	{
		if(c1[i] == c2[i])
			common_str.push_back(c1[i]);
		else{
			if(i == 3 || i == 6)
				//common_str.pop_back();
				common_str.erase(common_str.size() - 1,1);

			break;
		}
	}
	return common_str;
}

long cilin::get_ms_time()
{

	return clock();
}
