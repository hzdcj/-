#ifndef __USER_CF__
#define __USER_CF__
#include<vector>
#include<string>
#define MAX_COL 2000
using namespace std;
class user_cf
{
public:
	static user_cf* Create_user_cf();             //��������ʼ�������Ա�ϴ���ͼƬʱ����
	vector<int> get_pic_id(int max_num,const string &username);
	user_cf(){};
	user_cf (const user_cf&) = delete;                       
	user_cf& operator=(const user_cf&) = delete;
	void addUser(const string& username, vector<double>& train);
	void have_commom(string& username, vector<int>& pic_id);           //���۹��˾ͽ�Ԥ��ֵ����Ϊ-1
	void addData(vector<string>&user_id, vector<int>& pic_id, vector<vector<double>>& train, int itemStartIndex);              //������ݽ���
private:
	int picIndex=0;
	int itemStartIndex=0;                      //������±꿪ʼԤ��ͼƬ
	static user_cf * single;   
	static void destroy();                        //���������������;
	static void init_matrix();
	vector<string>user_id;
	vector<int>pic_id;
	vector<vector<double>>train;
	double ComputeRMSE(vector<vector<double> > predict, vector<vector<double> > test);           //����Ԥ��û��ļ���RMSE
	vector<vector<double>>predict;
	vector<vector<double>> UserBasedCF(int usersNum, int itemsNum, int itemStartIndex);                           //�������û�����
	void UserBasedCF(int usersNum, int itemsNum, int i, int itemStartIndex);                    //��ָ���û�����
	double ComputeSim(vector<double> &A, vector<double>& B, int method);
	void FindCommon(vector<double>& A, vector<double> &B, vector<double> &C, vector<double> &D);
	void SortCommom(vector<pair<int, double>>&A);                         //�����ִӴ�С����
	double norm(vector<double> &A)                                 //��������������������
	{
		double res = 0;
		for (vector<double>::size_type i = 0; i < A.size(); ++i)
			res += pow(A[i], 2);
		return sqrt(res);
	}
	double InnerProduct(vector<double> &A, vector<double>& B)                //������ӵ���ֵ
	{
		double res = 0;
		for (vector<double>::size_type i = 0; i < A.size(); ++i)
			res += A[i] * B[i];
		return res;
	}
};
#endif