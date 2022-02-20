#pragma once
#include <string>
#include "json/json.h"
using namespace std;
class Editor {
private:
	vector<double> angle_data;
	vector<bool> twirl;

	Json::Value root;

	void SetAngle(double &angle);

public:
	void Open(string adofai_file_location); //파일 열기
	void ChangeFile(); //변형
	void Save(string adofai_file_location); //저장
};

