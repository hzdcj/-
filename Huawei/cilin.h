#ifndef __CILIN_H__
#define __CILIN_H__
#include <map>
#include <vector>
#include <string>
#include <set>
#include<unordered_map>



using namespace std;

const float CILIN_A = 0.65;
const float CILIN_B = 0.8;
const float CILIN_C = 0.9;
const float CILIN_D = 0.96;
const float CILIN_E = 0.5;
const float CILIN_F = 0.1;
const float PI = 3.1416;
const int   CILIN_DEGREE = 180;

typedef map<string,vector<string> > word_map_t;
typedef map<string,set<int> > relation_code_map_t;
class cilin
{
public:
	cilin();
	~cilin();
	vector<string>& get_final_label(){ return final_label; };
	unordered_map<string, vector<string>>& get_relative_label(){ return relative_label; };
	long get_ms_time();
	void calculate_sim(unordered_map<string, int>&label);
	
private:
	float similarity(const string& w1, const string& w2);
	bool read_cilin(const char* path);
	unordered_map<string, vector<string>>relative_label;
	vector<string>final_label;
	void sort_label(unordered_map<string, int>&label);
	float sim_by_code(const string& c1,const string& c2);
	int   get_n(const string&);
	int   get_k(const string&,const string&, const string& common_str);
	float sim_formula(float coeff,int n,int k);
	void  split_code_layer(const string& code,vector<int>& layers,vector<string>& fathers);
	void  get_code_father_child(const string& code);
	int   get_layer_by_no(const string& code,int no);
	string get_common_str(const string& c1,const string& c2);
	static void split(const string& s, const string& delim,vector<string >& ret);
	static string& trim(std::string &s);
	word_map_t m_code_word_map; //���룺����
	word_map_t m_word_code_map;//�������
	
	relation_code_map_t m_code_father_child_map;//������,�ӱ��룬������n

};
#endif

