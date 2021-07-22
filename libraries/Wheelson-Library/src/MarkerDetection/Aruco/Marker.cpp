#include "Marker.h"
#include <iostream>

#include "MarkerInfo.h"

using namespace Aruco;
using namespace cv;
using namespace std;

Marker::Marker(){
	rows = 5;
	cols = 5;
	id = -1;
	rotation = 0;
	validated = false;
}


void Marker::attachInfo(MarkerInfo _info){
	info = _info;
}

int Marker::calculateID(){
	id = 0;

	for(int i = 1; i < 6; ++i){
		id <<= 1;
		id |= cells[i][2];
		id <<= 1;
		id |= cells[i][4];
	}

	return id;
}

bool Marker::validate(){
	validated = false;

	//Check if there are points
	if(projected.size() == 0){
		return false;
	}

	//Check black border allow up to three white squares for edge light bleed cases
	unsigned int bad = 0;
	for(unsigned int i = 0; i < 7; i++){
		if(cells[i][0] != 0 || cells[i][6] != 0 || cells[0][i] != 0 || cells[6][i] != 0){
			bad++;
			if(bad > 3){
				return false;
			}
		}
	}

	//Check hamming distance of internal data
	for(int j = 0; j < 4; j++){
		if(hammingDistance() == 0){
			calculateID();
			validated = true;
			return true;
		}

		rotate();
	}

	return false;
}

void Marker::rotate(){
	int temp[7][7];
	int n = 7;

	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			temp[i][j] = cells[n - j - 1][i];
		}
	}

	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			cells[i][j] = temp[i][j];
		}
	}

	rotation++;

	std::rotate(projected.begin(), projected.begin() + 1, projected.end());
}

int Marker::hammingDistance(){
	int ids[4][5] = {
			{ 1, 0, 0, 0, 0 },
			{ 1, 0, 1, 1, 1 },
			{ 0, 1, 0, 0, 1 },
			{ 0, 1, 1, 1, 0 }
	};

	int dist = 0;
	int sum, minSum;

	for(int i = 1; i < 6; ++i){
		minSum = 99999;

		for(int j = 1; j < 5; ++j){
			sum = 0;

			for(int k = 1; k < 6; ++k){
				sum += cells[i][k] == ids[j - 1][k - 1] ? 0 : 1;
			}

			if(sum < minSum){
				minSum = sum;
			}
		}

		dist += minSum;
	}

	return dist;
}

void Marker::print(){
	cout << "{" << endl;
	cout << "    Valid: " << validated << endl;
	cout << "    Hamming: " << hammingDistance() << endl;
	cout << "    ID: " << id << endl;
	cout << "    Cells: [";
	for(int i = 0; i < 7; i++){
		for(int j = 0; j < 7; j++){
			cout << cells[i][j] << ", ";
		}

		if(i == 6){
			cout << "]" << endl;
		}else{
			cout << endl << "            ";
		}
	}
	cout << "    Rotation: " << rotation << endl;

	for(unsigned int i = 0; i < projected.size(); i++){
		cout << "    Projected: " << projected[i].x << ", " << projected[i].y << endl;
	}

	info.print();

	cout << "}" << endl;
}
