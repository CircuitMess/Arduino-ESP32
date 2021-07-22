#include "Markers.h"
#include "MarkerDetection/Aruco/Aruco.h"
#include "MarkerDetection/Math/Quadrilateral.h"
#include "MarkerDetection/imgproc.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>

using namespace cv;
using namespace std;
using namespace Aruco;

static const float limitCosine = 0.7;
static const double maxError = 0.025;
static const int minArea = 100;

std::vector<Marker> Markers::detect(uint8_t* data, uint16_t width, uint16_t height, Markers::PixFormat format, Color* output){
	Mat gray;

	if(format == GRAY){
		gray = Mat(height, width, CV_8UC1, data);
	}else if(format == RGB565){
		Mat frame(height, width, CV_8UC2, data);
		cvtColor(frame, gray, CV_BGR5652GRAY);
	}else if(format == RGB888){
		Mat frame(height, width, CV_8UC2, data);
		cvtColor(frame, gray, CV_RGB2GRAY);
	}

	Mat threshBig;
	adaptive(gray, threshBig, gray.cols / 8);

	Mat thresh;
	resize(threshBig, thresh, Point(), 0.5, 0.5);

	Mat coutourImage;
	thresh.copyTo(coutourImage);
	CvMat cimage = cvMat(coutourImage);

	CvSeq* ccontours = 0;
	MemStorage storage(cvCreateMemStorage());
	cvFindContours(&cimage, storage, &ccontours, sizeof(CvContour), RETR_LIST, CHAIN_APPROX_SIMPLE);

	Mat draw;

	if(output != nullptr){
		cvtColor(thresh, draw, CV_GRAY2BGR565);
	}

	if(!ccontours){
		return vector<Marker>();
	}

	vector<Quadrilateral> quads = vector<Quadrilateral>();
	quads.reserve(10);
	for(; ccontours != nullptr; ccontours = ccontours->h_next){
		if(ccontours->total < 4) continue;

		CvPoint* points = static_cast<CvPoint*>(ps_malloc(ccontours->total * sizeof(CvPoint)));
		cvCvtSeqToArray(ccontours, points, CV_WHOLE_SEQ);

		Mat pts(ccontours->total, 1, CV_32SC2, points);

		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * maxError, true);

		free(points);

		// Square contours have 4 vertices, ...
		if(approx.size() != 4) continue;

		// after approximation relatively large area (to filter out noisy contours), ...
		if(fabs(contourArea(Mat(approx))) <= minArea) continue;

		// and be convex.
		if(!isContourConvex(Mat(approx))) continue;

		if(output != nullptr){
			for(int i = 1; i < approx.size(); i++){
				line(draw, approx[i-1], approx[i], CV_RGB(0, 0, 255), 1);
			}
			line(draw, approx.back(), approx.front(), CV_RGB(0, 0, 255), 1);
		}

		float maxCosine = 0;

		//Find the maximum cosine of the angle between joint edges
		for(int j = 2; j < 5; j++){
			float cosine = fabs(angleCornerPointsCos(approx[j % 4], approx[j - 2], approx[j - 1]));
			maxCosine = MAX(maxCosine, cosine);
		}

		//Check if all angle corner close to 90 (more than the max cosine)
		if(maxCosine >= limitCosine) continue;

		//Quad is ok
		Quadrilateral quad = Quadrilateral();
		for(int j = 0; !approx.empty() && j < 4; j++){
			quad.points[j] = approx.back();
			approx.pop_back();
		}
		quads.push_back(quad);

		//quad.draw(draw, CV_RGB(255, 0, 0), 2);
	}

	vector<Marker> markers;
	for(unsigned int i = 0; i < quads.size(); i++){
		Mat board = deformQuad(thresh, Point2i(49, 49), quads[i].points);
		Mat binary = Aruco::processImage(board);

		int size = 7;

		// skip all white or all black quads
		int a = 0, b = 0;
		for(int x = 0; x < size; x++){
			for(int y = 0; y < size; y++){
				a += binary.at<uint8_t>(x, y) == 0;
				b += binary.at<uint8_t>(x, y) == 255;
			}
		}
		if(a == size*size || b == size*size) continue;

		// skip quads without black border
		bool o = false;
		for(int i = 0, bad = 0; i < size; i++){
			if(binary.at<uint8_t>(i, 0) != 0 || binary.at<uint8_t>(i, size-1) != 0 || binary.at<uint8_t>(0, i) != 0 || binary.at<uint8_t>(size-1, i) != 0){
				bad++;
				if(bad > 3){
					o = true;
					break;
				}
			}
		}
		if(o) continue;

		// candidate
		// quads[i].draw(draw, CV_RGB(0, 0, 255), 2);

		//Process aruco image and get data
		Aruco::Marker marker = Aruco::readData(binary);
		marker.projected = quads[i].points;

		//Check if marker is valid
		if(marker.validate()){
			markers.push_back(marker);

			if(output != nullptr){
				quads[i].draw(draw, CV_RGB(0, 255, 0), 2);
			}
		}
	}

	if(output != nullptr){
		Mat draw2;
		resize(draw, draw2, Point(), 2, 2, INTER_NEAREST);

		memcpy(output, draw2.data, 160 * 120 * 2);
	}

	return markers;
}