#include "imgproc.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>

using namespace cv;
using namespace std;

void adaptive(const Mat& input, Mat& output, int blockSize){
	input.copyTo(output);

	Size size = input.size();
	Mat mean;
	boxFilter(input, mean, input.type(), Size(blockSize, blockSize), Point(-1, -1), true, BORDER_REPLICATE | BORDER_ISOLATED);

	uchar tab[768];
	uchar imaxval = saturate_cast<uchar>(255);
	for(int i = 0; i < 768; i++)
		tab[i] = (uchar) (i - 255 > 0 ? imaxval : 0);


	for(int i = 0; i < size.height; i++){
		const uchar* sdata = input.ptr(i);
		const uchar* mdata = mean.ptr(i);
		uchar* ddata = output.ptr(i);

		for(int j = 0; j < size.width; j++)
			ddata[j] = tab[sdata[j] - mdata[j] + 255];
	}
}



float angleCornerPointsCos(const Point& b, const Point& c, const Point& a){
	float dx1 = b.x - a.x;
	float dy1 = b.y - a.y;
	float dx2 = c.x - a.x;
	float dy2 = c.y - a.y;

	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-12);
}

Mat deformQuad(const Mat& image, const Point2i& size, const vector<Point2f>& quad){
	Mat out = Mat::zeros(size.x, size.y, CV_8UC3);

	vector<Point2f> points;
	points.emplace_back(0, 0);
	points.emplace_back(0, out.rows);
	points.emplace_back(out.cols, out.rows);
	points.emplace_back(out.cols, 0);

	Mat transformation = getPerspectiveTransform(quad, points);
	warpPerspective(image, out, transformation, out.size(), INTER_NEAREST);

	return out;
}

Mat rotationMatrix(const Point3d& euler){
	//Rotation on x axis
	Mat rx = (Mat_<double>(3, 3) <<
								 1, 0, 0,
			0, cos(euler.x), -sin(euler.x),
			0, sin(euler.x), cos(euler.x));

	//Rotation on y axis
	Mat ry = (Mat_<double>(3, 3) <<
								 cos(euler.y), 0, sin(euler.y),
			0, 1, 0,
			-sin(euler.y), 0, cos(euler.y));

	//Rotation on z axis
	Mat rz = (Mat_<double>(3, 3) <<
								 cos(euler.z), -sin(euler.z), 0,
			sin(euler.z), cos(euler.z), 0,
			0, 0, 1);

	return rz * ry * rx;
}