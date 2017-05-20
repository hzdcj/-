#ifndef __INTEREST_CF_H__
#define __INTEREST_CF_H__
#include<string>
#include<vector>
using namespace std;
class interest_cf
{
public:
	static interest_cf* getInstance();
	interest_cf();
	vector<string> getSimilarityUser(vector<int>& user_interest);         //需要求相似度的用户的兴趣数组
private:
	static interest_cf *single;
	void init_matrix();
	vector<vector<int>>matrix;                              //一份不是新成员的矩阵
	vector<string>user_id;
};
#endif