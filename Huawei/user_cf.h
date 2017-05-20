#ifndef __USER_CF__
#define __USER_CF__
#include<vector>
#include<string>
#define MAX_COL 2000
using namespace std;
class user_cf
{
public:
	static user_cf* Create_user_cf();             //服务器初始化或管理员上传新图片时调用
	vector<int> get_pic_id(int max_num,const string &username);
	user_cf(){};
	user_cf (const user_cf&) = delete;                       
	user_cf& operator=(const user_cf&) = delete;
	void addUser(const string& username, vector<double>& train);
	void have_commom(string& username, vector<int>& pic_id);           //评论过了就将预测值设置为-1
	void addData(vector<string>&user_id, vector<int>& pic_id, vector<vector<double>>& train, int itemStartIndex);              //添加数据进来
private:
	int picIndex=0;
	int itemStartIndex=0;                      //从这个下标开始预测图片
	static user_cf * single;   
	static void destroy();                        //销毁这个单例对象;
	static void init_matrix();
	vector<string>user_id;
	vector<int>pic_id;
	vector<vector<double>>train;
	double ComputeRMSE(vector<vector<double> > predict, vector<vector<double> > test);           //评价预测好坏的计算RMSE
	vector<vector<double>>predict;
	vector<vector<double>> UserBasedCF(int usersNum, int itemsNum, int itemStartIndex);                           //对所有用户评分
	void UserBasedCF(int usersNum, int itemsNum, int i, int itemStartIndex);                    //对指定用户评分
	double ComputeSim(vector<double> &A, vector<double>& B, int method);
	void FindCommon(vector<double>& A, vector<double> &B, vector<double> &C, vector<double> &D);
	void SortCommom(vector<pair<int, double>>&A);                         //对评分从大到小排序
	double norm(vector<double> &A)                                 //二范数，计算向量长度
	{
		double res = 0;
		for (vector<double>::size_type i = 0; i < A.size(); ++i)
			res += pow(A[i], 2);
		return sqrt(res);
	}
	double InnerProduct(vector<double> &A, vector<double>& B)                //计算分子的数值
	{
		double res = 0;
		for (vector<double>::size_type i = 0; i < A.size(); ++i)
			res += A[i] * B[i];
		return res;
	}
};
#endif