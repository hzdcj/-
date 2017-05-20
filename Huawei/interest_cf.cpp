#include"interest_cf.h"
#include"mysql_manager.h"
#include"mysql_pool.h"
interest_cf* interest_cf::single = nullptr;
interest_cf* interest_cf::getInstance()
{
	if (single == nullptr)
	{
		single = new interest_cf();
		cout << "interest_cf init" << endl;
	}
	return single;
}
interest_cf::interest_cf()
{
	init_matrix();
}
void interest_cf::init_matrix()
{
	mysql_manager mysql_manager(mysql_pool::GetInstance()->GetConnection());
	mysql_manager.get_interest_data(matrix, user_id);
}
bool cmp(const pair<string,int>& a, const pair<string,int>& b)
{
	return a.second > b.second;
}
vector<string> interest_cf::getSimilarityUser(vector<int>& user_interest)            //获取相似度最大的用户集合
{
	vector<pair<string,int>>similarity(user_id.size());
	for (int i = 0; i < similarity.size(); ++i)
	{
		int num = 0;
		for (int j = 0; j < CATEGORY_SIZE; ++j)
			num += (matrix[i][j] & user_interest[j]);                      //计算和user_interest的相似度
		similarity[i] = make_pair(user_id[i],num);
	}
	sort(similarity.begin(), similarity.end(), cmp);
	vector<string>temp(5);                                              //选取5个相似的用户的候选集合      
	for (int i = 0; i < 5; ++i)
	{
		temp[i]=similarity[i].first;                               
	}
	random_shuffle(temp.begin(), temp.end());                  //打乱顺序
	return temp;
}