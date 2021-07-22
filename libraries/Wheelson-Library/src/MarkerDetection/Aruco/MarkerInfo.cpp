#include "MarkerInfo.h"
#include <iostream>
#include <opencv2/core/mat.hpp>
#include "../imgproc.h"

using namespace Aruco;
using namespace cv;
using namespace std;

MarkerInfo::MarkerInfo(){
	id = -1;
	size = 1.0;
	position = Point3d(0.0, 0.0, 0.0);
	rotation = Point3d(0.0, 0.0, 0.0);
	world = vector<Point3f>();
	calculateWorldPoints();
}


MarkerInfo::MarkerInfo(int _id, double _size, Point3f _position){
	id = _id;
	size = _size;
	position = _position;
	rotation = Point3f(0.0, 0.0, 0.0);
	world = vector<Point3f>();
	calculateWorldPoints();
}


MarkerInfo::MarkerInfo(int _id, double _size, Point3f _position, Point3f _rotation){
	id = _id;
	size = _size;
	position = _position;
	rotation = _rotation;
	world = vector<Point3f>();
	calculateWorldPoints();
}


void MarkerInfo::calculateWorldPoints(){
	double half = size / 2.0;

	world.clear();

	world.push_back(Point3f(-half, -half, 0));
	world.push_back(Point3f(-half, +half, 0));
	world.push_back(Point3f(half, +half, 0));
	world.push_back(Point3f(half, -half, 0));

	Mat rot = rotationMatrix(rotation);

	for(unsigned int i = 0; i < world.size(); i++){
		Mat temp = (Mat_<double>(3, 1) << world[i].x, world[i].y, world[i].z);
		Mat transf = rot * temp;

		world[i].x = transf.at<double>(0, 0) + position.x;
		world[i].y = transf.at<double>(1, 0) + position.y;
		world[i].z = transf.at<double>(2, 0) - position.z;
	}
}


void MarkerInfo::print(){
	cout << "{" << endl;
	cout << "    ID: " << id << endl;
	cout << "    Size: " << size << endl;
	cout << "    Position: " << position.x << ", " << position.y << ", " << position.z << endl;
	cout << "    Rotation: " << rotation.x << ", " << rotation.y << ", " << rotation.z << endl;

	for(unsigned int i = 0; i < world.size(); i++){
		cout << "    World: " << world[i].x << ", " << world[i].y << ", " << world[i].z << endl;
	}

	cout << "}" << endl;
}
