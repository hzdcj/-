#ifndef __WORD_SIMILARITY__
#define __WORD_SIMILARITY__
#include <string>
#include <map>
#include <vector>
using namespace std;
struct BasicelemNode {
	int num;
	string basicElem;
	int fatherNode;
};
struct BEStruct {
	bool judge; 
	string firstBE;
	std::vector<string> otherBE;
	std::map<string, string> relationBE;
	std::map<char, string> symbolBE;
};
class word_similarity
{
public:
	word_similarity(vector<int>& word_frequency, vector<string>&label);
	~word_similarity();   
	vector<string>& get_final_label(){ return final_label; };
private:
	void calculate_final_label(vector<string>&label);                       //传入未经过筛选的标签集合
	float calculate_sim_BE(BasicelemNode *p_ben_, const BEStruct *kp_be1_, const BEStruct *kp_be2_);
    float calculate_sim_firstBE(BasicelemNode *p_ben_, const string *kp_be1_, const string *kp_be2_);
	float calculate_sim_otherBE(BasicelemNode *p_ben_, const vector<string> *kp_vec1_, const vector<string> *kp_vec2_);
	float calculate_sim_relationBE(BasicelemNode *p_ben_, const std::map<string, string> *kp_map1_, const std::map<string, string> *kp_map2_);
	float calculate_sim_symbolBE(BasicelemNode *p_ben_, const std::map<char, string> *kp_map1_, const std::map<char, string> *kp_map2_);
	void show_BE(const BEStruct *kp_be_);
	void analysis_string(const string *kp_str_, BEStruct *kp_be_);
	void get_chinese_BE(string *p_str_);
	float basicElem_similarity(BasicelemNode *p_ben_, const string *kp_str1_, const string *kp_str2_);
	int get_word_gap(BasicelemNode *p_ben_, const string *kp_compWord1_, const string *kp_compWord2_);            
	int init_BEFile(const char *kp_filename_, BasicelemNode *p_baseicelemNode_);
	float calculate_similarity(BasicelemNode *p_ben_, const char *kp_wordFilename_, const string *kp_input1_, const string *kp_input2_);
	const char *BE_FILE_NAME = "WHOLE.DAT";
	const char *WORD_FILE_NAME = "glossary.dat";
	const int  NODE_NUM = 1618;
	const int WORD_NUM = 66181;
	const float ALFA = 1.6;
	const float DELTA = 0.2;
	const float GAMA = 0.2;
	BasicelemNode *p_basicelemNode;
	vector<int>word_frequency;                    //标签出现的频率
	vector<pair<string,float>>pair_rate;             
	int total_number=0;
	vector<string>final_label;
};
#endif