#include "Quadrilateral.h"
#include <iostream>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

Quadrilateral::Quadrilateral(){
	for(int i = 0; i < 4; i++){
		points.push_back(Point2f(0.0, 0.0));
	}
}

/**
 * Quad contructor from a points.
 */
Quadrilateral::Quadrilateral(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, const cv::Point2f& d){
	points.push_back(a);
	points.push_back(b);
	points.push_back(c);
	points.push_back(d);
}

/**
 * Calculate Area of this quad.
 *
 * @return Area of this quad.
 */
float Quadrilateral::area() const{
	return contourArea(points);
}

/**
 * Check if point is inside this quad.
 *
 * @param p Point to check.
 * @return true if point is inside this quad
 */
bool Quadrilateral::containsPoint(const Point2f& p) const{
	return pointPolygonTest(points, p, false) >= 0.0;
}

void Quadrilateral::draw(cv::Mat& image, const cv::Scalar& color, int weigth) const{
	for(int j = 0; j < 3; j++){
		line(image, points[j], points[j + 1], color, weigth, 8);
	}

	line(image, points[3], points[0], color, weigth, 8);
}

void Quadrilateral::print() const{
	cout << "[" << this->points[0] << ", " << this->points[1] << ", " << this->points[2] << ", " << this->points[3] << "]" << endl;
}

Quadrilateral Quadrilateral::biggerQuadrilateral(const vector<Quadrilateral>& quads) const{
	Quadrilateral max = quads[0];
	float max_area = quads[0].area();

	//Search for bigger quad
	for(unsigned int i = 1; i < quads.size(); i++){
		float area = quads[i].area();
		if(area > max_area){
			max = quads[i];
			max_area = area;
		}
	}
	return max;
}

void Quadrilateral::drawVector(cv::Mat& image, const std::vector<Quadrilateral>& quads, const cv::Scalar& color) const{
	for(unsigned int i = 0; i < quads.size(); i++){
		quads[i].draw(image, color);
	}
}