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
	vector<string> getSimilarityUser(vector<int>& user_interest);         //��Ҫ�����ƶȵ��û�����Ȥ����
private:
	static interest_cf *single;
	void init_matrix();
	vector<vector<int>>matrix;                              //һ�ݲ����³�Ա�ľ���
	vector<string>user_id;
};
#endif