#include"user_cf.h"
#include"mysql_manager.h"
#include<iostream>
#include<algorithm>
#include"mysql_pool.h"
user_cf * user_cf::single = nullptr;
user_cf* user_cf::Create_user_cf()
{
	if (single == nullptr)
	{
		single = new user_cf();
		init_matrix();
		cout << "user_cf init" << endl;
	}
	return single;
}
void user_cf::destroy()
{
	if (single != nullptr)
	{
		delete single;
		single = nullptr;
	}
}
void user_cf::init_matrix()
{
	mysql_manager mysql_manager(mysql_pool::GetInstance()->GetConnection());
	mysql_manager.modify_matrix();
}
void user_cf::have_commom(string & username, vector<int>& pic_id)                                   //评论过了就将预测值设置为0
{
	int user_index = find(this->user_id.begin(), this->user_id.end(), username) - this->user_id.begin();
	for (int i = 0; i < pic_id.size(); ++i)
	{
		int pic_index = find(this->pic_id.begin(), this->pic_id.end(), pic_id[i]) - this->pic_id.begin();
		predict[user_index][pic_index] = 0;
	}                                                     
}
void user_cf::addUser(const string& username,vector<double>&train)                     //添加一条user,添加的train值必须和类中的pic_id一一对应
{
	this->train.push_back(train);
	predict.resize(predict.size() + 1);
	user_id.push_back(username);
	UserBasedCF(user_id.size(), this->train[0].size(), user_id.size() - 1, itemStartIndex);
}
void user_cf::addData(vector<string>&user_id,vector<int>& pic_id, vector<vector<double>>& train,int itemStartIndex)           //添加新的图片信息,需要形成新的矩阵,并进行预测,itemStartIndex为从第几个图开始预测,之前的只作参考
{  
	this->user_id = user_id;
	this->pic_id = pic_id;
	this->train = train;
	this->itemStartIndex = itemStartIndex;
	this->predict = UserBasedCF(user_id.size(), train[0].size(), itemStartIndex);
}
double user_cf::ComputeRMSE(vector<vector<double> > predict, vector<vector<double> > test)
{
	int Counter = 0;
    double sum = 0;
    for (vector<vector<double> >::size_type i = 0; i < test.size(); ++i)
    {
        for (vector<double>::size_type j = 0; j < test[0].size(); ++j)
        {
            if (predict[i][j] && test[i][j])
            {
				++Counter;
                sum += pow((test[i][j] - predict[i][j]), 2);
            }
        }
    }
    return sqrt(sum / Counter);
}
double user_cf::ComputeSim(vector<double> &A, vector<double>& B, int method)
{
	switch (method)
	{
	case 0://欧氏距离
	{
			   vector<double> C;
			   for (vector<double>::size_type i = 0; i < A.size(); ++i)
			   {
				   C.push_back((A[i] - B[i]));
			   }
			   return 1 / (1 + norm(C));
			   break;
	}
	case 1://皮尔逊相关系数
	{
			   double A_mean = 0;
			   double B_mean = 0;
			   for (vector<double>::size_type i = 0; i < A.size(); ++i)
			   {
				   A_mean += A[i];
				   B_mean += B[i];
			   }
			   A_mean /= A.size();
			   B_mean /= B.size();
			   vector<double> C(A);
			   vector<double> D(B);
			   for (vector<double>::size_type i = 0; i < A.size(); ++i)
			   {
				   C[i] = A[i] - A_mean;
				   D[i] = B[i] - B_mean;
			   }
			   return InnerProduct(C, D) / (norm(C) * norm(D));
			   break;
	}
	case 2:
	{
			  return InnerProduct(A, B) / (norm(A) * norm(B));
			  break;
	}
	default:
	{
			   cout << " Choose method:" << endl;
			   cout << "0:欧氏距离\n1:皮尔逊相关系数\n2:余弦相似度\n";
			   return -1;
	}
	}
}
void user_cf::FindCommon(vector<double> &A, vector<double>& B, vector<double> &C, vector<double> &D)             //找到相似的物品。可以根据分数进行修改
{
	for (vector<double>::size_type i = 0; i < A.size(); ++i)
	{
		if (A[i] && B[i])
		{
			C.push_back(A[i]);
			D.push_back(B[i]);
		}
	}
}
vector<vector<double>> user_cf::UserBasedCF(int usersNum, int itemsNum, int itemStartIndex)
{
	vector<vector<double>>predict(usersNum, vector<double>(itemsNum, 0));
	for (int i = 0; i < usersNum; ++i) //对每个用户每件物品都进行预测
	{
		//找出user i未评分的item j，预测user i 对item j的评分
		for (int itemStartIndex = 0; itemStartIndex < itemsNum; ++itemStartIndex)
		{
			if (train[i][itemStartIndex])
				continue;
			//如果item j没有被user i评过分,找出对 item j评过分的用户
			else
			{
				vector<double> sim;
				vector<double> historyScores;
				for (int k = 0; k < usersNum; ++k)
				{
					//如果user k对item j 评过分，计算user k与user i的相似度
					if (train[k][itemStartIndex])//找出对item j 评过分的user k
					{
						// 为了计算user k与user i的相似度，必须找出二者共同评过分的items
						// 把二者对共同评过分的items的评分分别存储在两个vector中
						vector<double> commonA, commonB;
						FindCommon(train[i], train[k], commonA, commonB);
						//如果二者存在共同评过分的items,计算相似度
						if (!commonA.empty())
						{
							sim.push_back(ComputeSim(commonA, commonB, 2));              // 相似度
							// 把user k对item j 的历史评分记录下来
							historyScores.push_back(train[k][itemStartIndex]);
						}
					}

				}
				// 计算出所有与user i存在共同评过分的items的users与user i之间的相似度，
				// 保存在sim中,这些users对目标items j(即user i没有评过分)的历史评分记
				// 录在historyScores中。利用这两个vector，计算出相似度加权平均分作为预
				// 测user i对item j的评分
				double SimSum = 0;
				if (!sim.empty())
				{
					for (vector<double>::size_type m = 0; m < sim.size(); ++m)
					{
						SimSum += sim[m];
					}
					predict[i][itemStartIndex] = InnerProduct(sim, historyScores) / (SimSum);
				}
			}
		}
	}
	return predict;
}
void user_cf::UserBasedCF(int usersNum, int itemsNum, int i, int itemStartIndex)
{
	//指定user i未评分的item j，预测user i 对item j的评分
	for (int j = 0; j < itemsNum; ++j)
	{
		if (train[i][j])
				continue;
		//如果item j没有被user i评过分,找出对 item j评过分的用户
		else
		{
			vector<double> sim;
			vector<double> historyScores;
			for (int k = 0; k < usersNum; ++k)
			{
				//如果user k对item j 评过分，计算user k与user i的相似度
				if (train[k][j])//找出对item j 评过分的user k
				{
					// 为了计算user k与user i的相似度，必须找出二者共同评过分的items
					// 把二者对共同评过分的items的评分分别存储在两个vector中
					vector<double> commonA, commonB;
					FindCommon(train[i], train[k], commonA, commonB);
					//如果二者存在共同评过分的items,计算相似度
					if (!commonA.empty())
					{
						sim.push_back(ComputeSim(commonA, commonB, 2));              // 相似度
						// 把user k对item j 的历史评分记录下来
						historyScores.push_back(train[k][j]);
					}
				}

			}
			// 计算出所有与user i存在共同评过分的items的users与user i之间的相似度，
			// 保存在sim中,这些users对目标items j(即user i没有评过分)的历史评分记
			// 录在historyScores中。利用这两个vector，计算出相似度加权平均分作为预
			// 测user i对item j的评分
			double SimSum = 0;
			if (!sim.empty())
			{
				for (vector<double>::size_type m = 0; m < sim.size(); ++m)
				{
					SimSum += sim[m];
				}
				predict[i][j] = InnerProduct(sim, historyScores) / (SimSum);
			}
		}
	}
}
bool cmp(const pair<int, double> &a, const pair<int, double> &b)
{
	return a.second>b.second;
}
void user_cf::SortCommom(vector<pair<int, double>>&A)
{
	sort(A.begin(), A.end(), cmp);
}
vector<int> user_cf::get_pic_id(int max_num, const string &username)
{
	auto it = find(user_id.begin(), user_id.end(), username);
	int index;
	if (it != user_id.end())
	{
		index = it - user_id.begin();               //获取username所在的行数
	}
	else                                                                             //如果不存在则加入，并计算预测值，只会进行一次
	{
		mysql_manager mysql_manager(mysql_pool::GetInstance()->GetConnection());
		vector<pair<int,double>> &point= mysql_manager.getTrainVector(username);
		vector<double>temp(train[0].size());
		for (int i = 0; i < point.size(); ++i)
			temp[point[i].first] = point[i].second;
		addUser(username, temp);
		index = predict.size() - 1;
	}
	vector<int>temp;
	vector<pair<int, double>>this_temp;
	for (int j = 0; j < predict[0].size(); ++j)
	{
		if (predict[index][j]!=0)
			this_temp.push_back(make_pair(pic_id[j],predict[index][j]));              //把未评过的加入数组中
	}
	SortCommom(this_temp);                                    //排序完的评分和pic_id
	int size = min((int)this_temp.size(), max_num);
	for (int i = 0; i < size; ++i)
		temp.push_back(this_temp[i].first);                    //找出前面的pic_id
	return temp;
}