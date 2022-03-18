#include "Editor.h"
#include <iostream>
#include <fstream>
using namespace std;

void Editor::SetAngle(double & angle) {
	while (angle < 0) {
		angle += 360;
	}
	while (angle >= 360) {
		angle -= 360;
	}
}

void Editor::Open(string adofai_file_location) {
	Json::CharReaderBuilder reader;
	ifstream is(adofai_file_location, ifstream::binary);
	string errorMessage;
	auto bret = Json::parseFromStream(reader, is, &this->root, &errorMessage);

	if (bret == false) {
		cout << "Error to parse JSON file !!!" << " " << errorMessage << endl;
		return;
	}


	if (this->root["angleData"].size()== 0) {
		string path = this->root["pathData"].asString();

		map<char, int> direction;
		direction.insert(pair<char, int>('R', 1));
		direction.insert(pair<char, int>('p', 2));
		direction.insert(pair<char, int>('J', 3));
		direction.insert(pair<char, int>('E', 4));
		direction.insert(pair<char, int>('T', 5));
		direction.insert(pair<char, int>('o', 6));
		direction.insert(pair<char, int>('U', 7));
		direction.insert(pair<char, int>('q', 8));
		direction.insert(pair<char, int>('G', 9));
		direction.insert(pair<char, int>('Q', 10));
		direction.insert(pair<char, int>('H', 11));
		direction.insert(pair<char, int>('W', 12));
		direction.insert(pair<char, int>('L', 13));
		direction.insert(pair<char, int>('x', 14));
		direction.insert(pair<char, int>('N', 15));
		direction.insert(pair<char, int>('Z', 16));
		direction.insert(pair<char, int>('F', 17));
		direction.insert(pair<char, int>('V', 18));
		direction.insert(pair<char, int>('D', 19));
		direction.insert(pair<char, int>('Y', 20));
		direction.insert(pair<char, int>('B', 21));
		direction.insert(pair<char, int>('C', 22));
		direction.insert(pair<char, int>('M', 23));
		direction.insert(pair<char, int>('A', 24));
		direction.insert(pair<char, int>('!', -1));

		for (int i = 0; i < path.size(); i++) {
			this->root["angleData"][i] = (direction[path[i]] - 1) * 15;
			if (direction[path[i]] == -1) {
				this->root["angleData"][i] = 999;
			}
		}
	}

	this->root.removeMember("pathData");

	Json::Value angleData = this->root["angleData"];

	this->angle_data.clear();
	for (int i = 0; i < angleData.size(); i++) {
		this->angle_data.push_back(angleData[i].asDouble());
	}

	this->twirl.clear();
	this->twirl.resize(this->angle_data.size());
	for (int i = 0; i < twirl.size(); i++) {
		this->twirl[i] = false;
	}

	Json::Value twirlData = this->root["actions"];
	for (int i = 0; i < twirlData.size(); i++) {
		if (twirlData[i]["eventType"] == "Twirl") {
			this->twirl[twirlData[i]["floor"].asInt()] = true;
		}
	}
}

void Editor::ChangeFile() {
	vector<double> new_angle;
	new_angle.push_back(angle_data.front());
	bool tt = false;
	vector<int> tile_number;
	tile_number.push_back(0);
	for (int i = 1; i < angle_data.size(); i++) {
		tile_number.push_back(new_angle.size());
		if (this->twirl[i]) {
			tt = !tt;
		}
		if (angle_data[i] != 999) {
			double angle = angle_data[i - 1];
			if (angle != 999) {
				if (tt) {
					new_angle.push_back(angle - 180 + 15);
				}
				else {
					new_angle.push_back(angle + 180 - 15);
				}
				new_angle.push_back(999);
			}
			else {
				double angle = angle_data[i - 2];
				if (tt) {
					new_angle.push_back(angle + 15);
				}
				else {
					new_angle.push_back(angle - 15);
				}
				new_angle.push_back(999);
			}
		}
		new_angle.push_back(angle_data[i]);
	}
	this->root["angleData"].clear();
	for (int i = 0; i < new_angle.size(); i++) {
		this->root["angleData"][i] = new_angle[i];
	}
	tile_number.push_back(tile_number.back() + 1);
	for (int i = 0; i < this->root["actions"].size(); i++) {
		int floor = this->root["actions"][i]["floor"].asInt();
		this->root["actions"][i]["floor"] = tile_number[floor];
		
		if (this->root["actions"][i]["eventType"].asString() == "MoveTrack") {
			string start = this->root["actions"][i]["startTile"][1].asString();
			string end = this->root["actions"][i]["endTile"][1].asString();
			if (start == "ThisTile") {
				this->root["actions"][i]["startTile"][0] = tile_number[this->root["actions"][i]["startTile"][0].asInt() + floor] - tile_number[floor];
			}
			else if (start == "Start") {
				this->root["actions"][i]["startTile"][0] = tile_number[this->root["actions"][i]["startTile"][0].asInt()];
			}
			else if (start == "End") {
				this->root["actions"][i]["startTile"][0] = tile_number[tile_number.size() - 1 - this->root["actions"][i]["startTile"][0].asInt()] - tile_number.back();
			}

			if (end == "ThisTile") {
				this->root["actions"][i]["endTile"][0] = tile_number[this->root["actions"][i]["endTile"][0].asInt() + floor] - tile_number[floor];
			}
			else if (end == "Start") {
				this->root["actions"][i]["endTile"][0] = tile_number[this->root["actions"][i]["endTile"][0].asInt()];
			}
			else if (end == "End") {
				this->root["actions"][i]["endTile"][0] = tile_number[tile_number.size() - 1 - this->root["actions"][i]["endTile"][0].asInt()] - tile_number.back();
			}

			auto move = this->root["actions"][i];
			move["startTile"][0] = move["startTile"][0].asInt() + 1;
			move["endTile"][0] = move["endTile"][0].asInt() + 1;
			this->root["actions"].insert(i + 1, move);

			move["startTile"][0] = move["startTile"][0].asInt() + 1;
			move["endTile"][0] = move["endTile"][0].asInt() + 1;
			this->root["actions"].insert(i + 2, move);
			i += 2;
		}
	}
}

void Editor::ChangeFileOther() {
	vector<double> new_angle;
	new_angle.push_back(angle_data.front());
	bool tt = false;
	vector<int> tile_number;
	tile_number.push_back(0);
	for (int i = 1; i < angle_data.size(); i++) {
		tile_number.push_back(new_angle.size());
		if (this->twirl[i]) {
			tt = !tt;
		}
		if (angle_data[i] != 999) {
			double angle = angle_data[i - 1];
			if (angle != 999) {
				for (int i = 1; i <= 10; i++) {
					if (tt) {
						new_angle.push_back(angle - 180 + i * 3);
					}
					else {
						new_angle.push_back(angle + 180 - i * 3);
					}
					new_angle.push_back(999);
				}
			}
			else {
				double angle = angle_data[i - 2];
				for (int i = 1; i <= 10; i++) {
					if (tt) {
						new_angle.push_back(angle + i * 3);
					}
					else {
						new_angle.push_back(angle - i * 3);
					}
					new_angle.push_back(999);
				}
			}
		}
		new_angle.push_back(angle_data[i]);
	}
	this->root["angleData"].clear();
	for (int i = 0; i < new_angle.size(); i++) {
		this->root["angleData"][i] = new_angle[i];
	}
	tile_number.push_back(tile_number.back() + 1);
	for (int i = 0; i < this->root["actions"].size(); i++) {
		int floor = this->root["actions"][i]["floor"].asInt();
		this->root["actions"][i]["floor"] = tile_number[floor];

		if (this->root["actions"][i]["eventType"].asString() == "MoveTrack") {
			string start = this->root["actions"][i]["startTile"][1].asString();
			string end = this->root["actions"][i]["endTile"][1].asString();
			if (start == "ThisTile") {
				this->root["actions"][i]["startTile"][0] = tile_number[this->root["actions"][i]["startTile"][0].asInt() + floor] - tile_number[floor];
			}
			else if (start == "Start") {
				this->root["actions"][i]["startTile"][0] = tile_number[this->root["actions"][i]["startTile"][0].asInt()];
			}
			else if (start == "End") {
				this->root["actions"][i]["startTile"][0] = tile_number[tile_number.size() - 10 - this->root["actions"][i]["startTile"][0].asInt()] - tile_number.back();
			}

			if (end == "ThisTile") {
				this->root["actions"][i]["endTile"][0] = tile_number[this->root["actions"][i]["endTile"][0].asInt() + floor] - tile_number[floor];
			}
			else if (end == "Start") {
				this->root["actions"][i]["endTile"][0] = tile_number[this->root["actions"][i]["endTile"][0].asInt()];
			}
			else if (end == "End") {
				this->root["actions"][i]["endTile"][0] = tile_number[tile_number.size() - 10 - this->root["actions"][i]["endTile"][0].asInt()] - tile_number.back();
			}

			auto move = this->root["actions"][i];
			for (int j = 0; j < 10; j++) {
				move["startTile"][0] = move["startTile"][0].asInt() + 1;
				move["endTile"][0] = move["endTile"][0].asInt() + 1;
				this->root["actions"].insert(i + 1 + j, move);
			}
			i += 10;
		}
	}
}

void Editor::Save(string adofai_file_location) {
	FILE *f;
	fopen_s(&f, adofai_file_location.c_str(), "wb");
	fprintf_s(f, "%s", this->root.toStyledString().c_str());
	fclose(f);
}
