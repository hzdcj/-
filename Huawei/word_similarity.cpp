#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include"word_similarity.h"
const float BETA[4] = { 0.5, 0.2, 0.17, 0.13 };
word_similarity::word_similarity(vector<int>& word_frequency, vector<string>&label)
{
	p_basicelemNode = new BasicelemNode[NODE_NUM];
	init_BEFile(BE_FILE_NAME, p_basicelemNode);
	this->word_frequency = word_frequency;
	pair_rate.resize(word_frequency.size());
	calculate_final_label(label);
}
word_similarity::~word_similarity()
{
	delete[]p_basicelemNode;
	p_basicelemNode = nullptr;
}
bool cmp(const pair<string, float>&a, const pair<string, float>&b)
{
	return a.second > b.second;
}
void word_similarity::calculate_final_label(vector<string>&label)
{
	vector<int>frequency_increament;
	frequency_increament.resize(label.size());
	for (int i = 0; i < frequency_increament.size(); ++i)
	{
		for (int j = 0; j < frequency_increament.size(); ++j)
		{
			if (i != j)
			{
				float similarity1 = calculate_similarity(p_basicelemNode, WORD_FILE_NAME, &label[i], &label[j]);
				cout << label[i] << " " << label[j] << " " << similarity1 << endl;;
				if (similarity1>0.7)
				{
					frequency_increament[i] += word_frequency[i];
				}
			}
		}
	}
	for (int i = 0; i < word_frequency.size(); ++i)
	{
		word_frequency[i] += frequency_increament[i];            //得到最终的标签个数
		total_number += word_frequency[i];                       //最终合并的标签的总数
		cout << word_frequency[i] << endl;
	}
	//pair_rate.resize(word_frequency.size());
	for (int i = 0; i < word_frequency.size(); ++i)
	{
		float rate = (float)word_frequency[i] / total_number;      //标签的比例
		pair_rate.push_back(make_pair(label[i], rate));
	}
	sort(pair_rate.begin(), pair_rate.end(), cmp);
	int size = min(10, (int)word_frequency.size());
	for (int i = 0; i < size; ++i)
	{
		if (i>5&&pair_rate[i].second < 0.5)
			break;
		final_label.push_back(pair_rate[i].first);
	}
}
float word_similarity::calculate_similarity(BasicelemNode *p_ben_, const char *kp_wordFilename_, const string *kp_input1_, const string *kp_input2_) {
	if ((*kp_input1_) == (*kp_input2_))
		return 1.0;

	ifstream infile(kp_wordFilename_);
	if (!infile)
		return -1.0;

	vector<string> valueStr1, valueStr2;
	string tempStr;
	size_t tempPos;

	for (int i = 0; i < WORD_NUM; ++i) {
		infile >> tempStr;

		if (tempStr == (*kp_input1_)) { 
			infile >> tempStr;
			getline(infile, tempStr, '\n');
			tempPos = tempStr.find_first_not_of(" \t");
			tempStr = tempStr.substr(tempPos, tempStr.size() - tempPos);
			valueStr1.push_back(tempStr);
		}
		else if (tempStr == (*kp_input2_)) {
			infile >> tempStr;
			getline(infile, tempStr, '\n');
			tempPos = tempStr.find_first_not_of(" \t");
			tempStr = tempStr.substr(tempPos, tempStr.size() - tempPos);
			valueStr2.push_back(tempStr);
		}
		else {
			infile >> tempStr;
			getline(infile, tempStr, '\n');
		}
	}

	infile.close();
	infile.clear();

	if (valueStr1.size() == 0 || valueStr2.size() == 0) //有单词不在表中，返回-2.0
		return -2.0;

	float max = 0., temp = 0.;

	for (size_t i = 0; i < valueStr1.size(); ++i) {
		BEStruct *p_be1 = new BEStruct;
		analysis_string(&valueStr1[i], p_be1);
		for (size_t j = 0; j < valueStr2.size(); ++j) {
			BEStruct *p_be2 = new BEStruct;
			analysis_string(&valueStr2[j], p_be2);
			//show_BE(p_be1);
			//show_BE(p_be2);
			temp = calculate_sim_BE(p_ben_, p_be1, p_be2);
			if (temp > max)
				max = temp;
			delete p_be2;
		}
		delete p_be1;
	}
	return max;
}


int word_similarity::init_BEFile(const char *kp_filename_, BasicelemNode *p_baseicelemNode_) {
	ifstream infile(kp_filename_, ifstream::in);
	if (!infile)
		return -1;

	for (int i = 0; i < NODE_NUM; ++i) {
		infile >> p_baseicelemNode_[i].num >> p_baseicelemNode_[i].basicElem >> p_baseicelemNode_[i].fatherNode;
		get_chinese_BE(&p_baseicelemNode_[i].basicElem);
	}

	infile.close();
	infile.clear();

	return 0;
}


int word_similarity::get_word_gap(BasicelemNode *p_ben_, const string *kp_compWord1_, const string *kp_compWord2_) {
	int num1 = -1, num2 = -1, fatherNum1 = -1, fatherNum2 = -1;
	int len = 0;

	for (int i = 0; i < NODE_NUM; ++i) {
		if (p_ben_[i].basicElem == (*kp_compWord1_)) {
			num1 = p_ben_[i].num;
			fatherNum1 = p_ben_[i].fatherNode;
		}
		if (p_ben_[i].basicElem == (*kp_compWord2_)) {
			num2 = p_ben_[i].num;
			fatherNum2 = p_ben_[i].fatherNode;
		}
	}

	if (num1 == -1 || num2 == -1 || fatherNum1 == -1 || fatherNum2 == -1)// 两个单词是不是在表中
		return -1;

	//caculate the disttance of two basic element
	//take word1's father path into a vector
	vector<int> fatherPath;

	while (num1 != fatherNum1) {
		fatherPath.push_back(num1);
		num1 = fatherNum1;
		fatherNum1 = p_ben_[fatherNum1].fatherNode;
	}

	fatherPath.push_back(num1);
	//at the time of find fatherhood of word2, catch the distance of 2 words
	vector<int>::iterator fatherPathPos;

	while (num2 != fatherNum2) { //TODO:this condition of judge need to be change
		if ((fatherPathPos = find(fatherPath.begin(), fatherPath.end(), num2)) != fatherPath.end())
			return fatherPathPos - fatherPath.begin() + len;
		num2 = fatherNum2;
		fatherNum2 = p_ben_[fatherNum2].fatherNode;
		++len;
	}

	if (num2 == fatherNum2)//this judge is to deal with the fatherhood of word2 is root of the tree(where num==fatherNode)
	if ((fatherPathPos = find(fatherPath.begin(), fatherPath.end(), num2)) != fatherPath.end())
		return fatherPathPos - fatherPath.begin() + len;
	return 20;//if there isn't a path between two words, default distance is 20(it's far for this)
}


float word_similarity::basicElem_similarity(BasicelemNode *p_ben_, const string *kp_str1_, const string *kp_str2_) {
	if ((*kp_str1_) == "" && (*kp_str2_) == "")
		return 1.;

	if ((*kp_str1_) == "" || (*kp_str2_) == "") //the similarity of a "" and BE is delta
		return DELTA;

	if ((*kp_str1_) == (*kp_str2_)) //the similarity of two same BEs is 1
		return 1.0;

	int d = get_word_gap(p_ben_, kp_str1_, kp_str2_);

	if (d >= 0)
		return ALFA / (ALFA + d);
	else
		return -1.0; //one of the string is not BE
}


void word_similarity::analysis_string(const string *kp_str_, BEStruct *kp_be_) {
	string temp, soy, s = (*kp_str_); //yeah, var "soy" is just a JiangYou
	string *str = &s;

	if ((*str)[0] != '{')
		kp_be_->judge = true;
	else {
		kp_be_->judge = false;
		(*str) = str->substr(1, str->size() - 2);
	}

	size_t pos = str->find_first_of(',');
	size_t oldPos = 0;
	size_t equalPos = 0;
	int isFirstBE = 0;

	if ((*str)[0] > 64 && (*str)[0] < 123) { //1st kp_be_ is firstBE
		if (pos == string::npos) { //only firstBE
			kp_be_->firstBE = (*str);
			get_chinese_BE(&kp_be_->firstBE);//get the chinese kp_be_
			//cout << "firstBE:" << temp << endl;
			return;
		}
		while (oldPos != string::npos) {
			temp = str->substr(oldPos, pos - oldPos);
			if (pos == string::npos)
				oldPos = pos;
			else
				oldPos = pos + 1;
			pos = str->find_first_of(',', oldPos);
			if (isFirstBE == 0) { //come to the firstBE
				get_chinese_BE(&temp);
				//cout << "firstBE" << temp << ends;
				kp_be_->firstBE = temp;
				++isFirstBE;
				continue;
			}
			if (temp[0] == '(') { //specific word
				//cout << "specific word:" << temp << ends;
				kp_be_->otherBE.push_back(temp);
				continue;
			}
			if ((equalPos = temp.find('=')) != string::npos) { //relationBE
				soy = temp.substr(equalPos + 1);
				if (soy[0] != '(')//after the '=' is not the specific word
					get_chinese_BE(&soy);
				kp_be_->relationBE.insert(std::pair<string, string>(temp.substr(0, equalPos), soy));
				//cout << "relationBE:" << temp.substr(0, equalPos) << "=" << soy << ends;
				continue;
			}
			if (temp[0] < 65 || temp[0] > 122) { //symbolBE
				soy = temp.substr(1);
				if (soy[0] != '(')//after the symbol is not the specific word
					get_chinese_BE(&soy);
				kp_be_->symbolBE.insert(std::pair<char, string>(temp[0], soy));
				//cout << "symbolBE" << temp[0] << "=" << soy << ends;
				continue;
			}
			get_chinese_BE(&temp);
			kp_be_->otherBE.push_back(temp);
			//cout << "otherBE:" << temp << ends;
		}
	}
	else { //there is not firstBE
		while (oldPos != string::npos) {
			temp = str->substr(oldPos, pos - oldPos);
			if (pos == string::npos)
				oldPos = pos;
			else
				oldPos = pos + 1;
			pos = str->find_first_of(',', oldPos);
			if (temp[0] == '(') {
				kp_be_->otherBE.push_back(temp);
				//cout << "specific word:" << temp << ends;
				continue;
			}
			if ((equalPos = temp.find('=')) != string::npos) { //relationBE
				soy = temp.substr(equalPos + 1);
				if (soy[0] != '(')//after the '=' is not the specific word
					get_chinese_BE(&soy);
				kp_be_->relationBE.insert(std::pair<string, string>(temp.substr(0, equalPos), soy));
				//cout << "relationBE:" << temp.substr(0, equalPos) << "=" << soy << ends;
				continue;
			}
			if (temp[0] < 65 || temp[0] > 122) { //symbolBE
				soy = temp.substr(1);
				if (soy[0] != '(')//after the symbol is not the specific word
					get_chinese_BE(&soy);
				kp_be_->symbolBE.insert(std::pair<char, string>(temp[0], soy));
				//cout << "symbolBE:" << temp[0] << "=" << soy << ends;
				continue;
			}
			get_chinese_BE(&temp);
			kp_be_->otherBE.push_back(temp);
			//cout << "otherBE" << temp << ends;
		}
	}
	//show_BE(kp_be_);
}


void word_similarity::get_chinese_BE(string *p_str_) {
	size_t pos = p_str_->find_first_of("|");
	(*p_str_) = p_str_->substr(pos + 1, p_str_->size() - pos);
}


void word_similarity::show_BE(const BEStruct *kp_be_) {
	//cout<<kp_be_->judge<<" ";
	if (kp_be_->judge == true)
		cout << "实词：" << " ";
	else
		cout << "虚词：" << " ";
	cout << "firstBE:" << kp_be_->firstBE << "\t";
	cout << "otherBE:";

	std::vector<string>::const_iterator pos0;

	for (pos0 = (kp_be_->otherBE).begin(); pos0 != (kp_be_->otherBE).end(); ++pos0)
		cout << (*pos0) << ends;
	cout << "\t";

	cout << "relationBE:";
	std::map<string, string>::const_iterator pos1;
	for (pos1 = (kp_be_->relationBE).begin(); pos1 != (kp_be_->relationBE).end(); ++pos1)
		cout << pos1->first << "=" << pos1->second << ends;
	cout << "\t";
	cout << "symbolBE:";
	std::map<char, string>::const_iterator pos2;
	for (pos2 = (kp_be_->symbolBE).begin(); pos2 != (kp_be_->symbolBE).end(); ++pos2)
		cout << pos2->first << "=" << pos2->second << ends;
	cout << endl;
}


float word_similarity::calculate_sim_firstBE(BasicelemNode *p_ben_, const string *kp_be1_, const string *kp_be2_) {
	return basicElem_similarity(p_ben_, kp_be1_, kp_be2_);
}


float word_similarity::calculate_sim_otherBE(BasicelemNode *p_ben_, const vector<string> *kp_vec1_, const vector<string> *kp_vec2_) {
	// I take some changes of the algorithm of sim_otherBE, for more convenience to programming
	if (kp_vec1_->size() == 0 && kp_vec2_->size() == 0)
		return 1.;

	float sum = 0.; float maxTemp = 0.; float temp = 0.;

	for (size_t i = 0; i < (kp_vec1_->size()); ++i) {
		maxTemp = -1.0;
		temp = 0.;

		for (size_t j = 0; j < (kp_vec2_->size()); ++j) {
			temp = 0.0;
			if ((*kp_vec1_)[i][0] != '(' && (*kp_vec2_)[j][0] != '(')//two BEs
				temp = basicElem_similarity(p_ben_, &((*kp_vec1_)[i]), &((*kp_vec2_)[j]));
			else if ((*kp_vec1_)[i][0] == '(' && (*kp_vec2_)[j][0] == '(') { //two specific words
				if ((*kp_vec1_)[i] == (*kp_vec2_)[j])//two same specific word
					temp = 1.0;
				else
					maxTemp = 0.0;
			}
			else //one specific word, and one is BE
				temp = GAMA;
			if (temp > maxTemp)
				maxTemp = temp;
		}
		if (maxTemp == -1.0)//there is no element in kp_vec2_
			maxTemp = DELTA;
		sum = sum + maxTemp;
	}
	if ((kp_vec1_->size()) < (kp_vec2_->size()))
		sum = sum + ((kp_vec2_->size()) - (kp_vec1_->size())) * DELTA;
	int s = (kp_vec1_->size()) > (kp_vec2_->size()) ? (kp_vec1_->size()) : (kp_vec2_->size());
	return sum / s;
}


float word_similarity::calculate_sim_relationBE(BasicelemNode *p_ben_, const std::map<string, string> *kp_map1_, const std::map<string, string> *kp_map2_) {
	if (kp_map1_->size() == 0 && kp_map2_->size() == 0)
		return 1.;

	float sum = 0.; float maxTemp = 0.; float temp = 0.;
	std::map<string, string>::const_iterator it1, it2;

	for (it1 = kp_map1_->begin(); it1 != kp_map1_->end(); ++it1) {
		maxTemp = 0.; temp = 0.;
		it2 = kp_map2_->find(it1->first);

		if (it2 != kp_map2_->end()) { //there is same relationBE
			if ((it1->second)[0] != '(' && (it2->second)[0] != '(') { //two BEs
				temp = basicElem_similarity(p_ben_, &(it1->second), &(it2->second));
			}
			else if ((it1->second)[0] == '(' && (it2->second)[0] == '(') { //two specific words
				if ((it1->second) == (it2->second))
					temp = 1.;
				else
					maxTemp = 0.;
			}
			else //one BEs and one specific word
				temp = GAMA;
		}
		else //there is no same relationBE in map2
			maxTemp = DELTA;
		if (temp > maxTemp)
			maxTemp = temp;
		sum = sum + maxTemp;
	}
	if ((kp_map1_->size()) < (kp_map2_->size()))
		sum = sum + ((kp_map2_->size()) - (kp_map1_->size())) * DELTA;
	int s = (kp_map1_->size()) > (kp_map2_->size()) ? (kp_map1_->size()) : (kp_map2_->size());
	return sum / s;
}


float word_similarity::calculate_sim_symbolBE(BasicelemNode *p_ben_, const std::map<char, string> *kp_map1_, const std::map<char, string> *kp_map2_) {
	if (kp_map1_->size() == 0 && kp_map2_->size() == 0)
		return 1.;

	float sum = 0.; float maxTemp = 0.; float temp = 0.;
	std::map<char, string>::const_iterator it1, it2;

	for (it1 = kp_map1_->begin(); it1 != kp_map1_->end(); ++it1) {
		maxTemp = 0.; temp = 0.;
		it2 = kp_map2_->find(it1->first);
		if (it2 != kp_map2_->end()) { //there is same relationBE
			if ((it1->second)[0] != '(' && (it2->second)[0] != '(') { //two BEs
				temp = basicElem_similarity(p_ben_, &(it1->second), &(it2->second));
			}
			else if ((it1->second)[0] == '(' && (it2->second)[0] == '(') { //two specific words
				if ((it1->second) == (it2->second))
					temp = 1.;
				else
					maxTemp = 0.;
			}
			else //one BEs and one specific word
				temp = GAMA;
		}
		else //there is no same relationBE in map2
			maxTemp = DELTA;
		if (temp > maxTemp)
			maxTemp = temp;
		sum = sum + maxTemp;
	}
	if ((kp_map1_->size()) < (kp_map2_->size()))
		sum = sum + ((kp_map2_->size()) - (kp_map1_->size())) * DELTA;
	int s = (kp_map1_->size()) > (kp_map2_->size()) ? (kp_map1_->size()) : (kp_map2_->size());
	return sum / s;
}


float word_similarity::calculate_sim_BE(BasicelemNode *p_ben_, const BEStruct *kp_be1_, const BEStruct *kp_be2_) {
	if (kp_be1_->judge != kp_be2_->judge)
		return 0.0;
	float sim1 = 0., sim2 = 0., sim3 = 0., sim4 = 0., sim = 0.;
	sim1 = calculate_sim_firstBE(p_ben_, &(kp_be1_->firstBE), &(kp_be2_->firstBE));
	//cout << "sim1=" << sim1 << endl;
	sim2 = calculate_sim_otherBE(p_ben_, &(kp_be1_->otherBE), &(kp_be2_->otherBE));
	//cout << "sim2=" << sim2 << endl;
	sim3 = calculate_sim_relationBE(p_ben_, &(kp_be1_->relationBE), &(kp_be2_->relationBE));
	//cout << "sim3=" << sim3 << endl;
	sim4 = calculate_sim_symbolBE(p_ben_, &(kp_be1_->symbolBE), &(kp_be2_->symbolBE));
	//cout << "sim4=" << sim4 << endl;
	sim = BETA[0] * sim1 + BETA[1] * sim1 * sim2 + BETA[2] * sim1 * sim2 * sim3 + BETA[3] * sim1 * sim2 * sim3 * sim4;
	//cout << "sim=" << sim << endl;
	return sim;
}