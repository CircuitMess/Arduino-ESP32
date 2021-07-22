#include "BallTracker.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc.hpp>
#include <queue>

using namespace cv;

std::vector<Ball> BallTracker::detect(uint8_t* data, uint16_t width, uint16_t height, uint8_t hue, BallTracker::PixFormat format, Color* output){
	Mat frame;

	if(format == RGB565){
		Mat frame565(height, width, CV_8UC2, data);
		cvtColor(frame, frame565, CV_BGR5652RGB);
	}else if(format == RGB888){
		frame = Mat(height, width, CV_8UC3, data);
	}

	resize(frame, frame, Size(), 0.5, 0.5);

	Mat draw;




	Mat blur;
	cv::boxFilter(frame, blur, frame.depth(), cv::Size(4, 4), cv::Point(-1,-1), true, cv::BORDER_CONSTANT );

	Mat hsv;
	cvtColor(blur, hsv, COLOR_BGR2HSV);
	blur.release();


	Mat mask;
	if(hue < 15){
		Mat mask1, mask2;
		inRange(hsv, Scalar(0, 70, 50), Scalar(hue + 15, 255, 255), mask1);
		inRange(hsv, Scalar(180 - 15 + hue, 70, 50), Scalar(180, 255, 255), mask2);
		mask = mask1 | mask2;
		mask1.release();
		mask2.release();
	}else if(hue > 165){
		Mat mask1, mask2;
		inRange(hsv, Scalar(hue - 15, 70, 50), Scalar(180, 255, 255), mask1);
		inRange(hsv, Scalar(0, 70, 50), Scalar(hue + 15 - 180, 255, 255), mask2);
		mask = mask1 | mask2;
		mask1.release();
		mask2.release();
	}else{
		Scalar colorLower(hue-15, 100, 50);
		Scalar colorUpper(hue+15, 255, 255);
		inRange(hsv, colorLower, colorUpper, mask);
	}


	if(output){
		cvtColor(mask, draw, CV_GRAY2BGR565);
	}

	struct Contour {
		float area;
		int index;
	};

	struct ContourComparator {
		bool operator()(const Contour& c1, const Contour& c2){
			return c1.area < c2.area;
		}
	};

	std::priority_queue<Contour, std::vector<Contour>, ContourComparator> contourSizes;

	Mat coutourImage;
	mask.copyTo(coutourImage);
	CvMat cimage = cvMat(coutourImage);

	CvSeq* ccontours = 0;
	MemStorage storage(cvCreateMemStorage());
	cvFindContours(&cimage, storage, &ccontours, sizeof(CvContour), RETR_LIST, CHAIN_APPROX_SIMPLE);

	CvSeq* contour = ccontours;
	for(int i = 0; contour != nullptr; i++, contour = contour->h_next){
		CvPoint* points = static_cast<CvPoint*>(malloc(contour->total * sizeof(CvPoint)));
		cvCvtSeqToArray(contour, points, CV_WHOLE_SEQ);
		std::vector<Point2i> c(points, points + contour->total);
		contourSizes.push({ (float) contourArea(c), i });
		free(points);
	}

	/** ### */

	std::vector<Ball> balls;

	for(int i = 0; i < min((size_t) 5, contourSizes.size()) && !contourSizes.empty(); i++){
		const auto& cs = contourSizes.top();
		if(cs.area < 15) continue;

		CvSeq* ccontour = ccontours;
		for(int i = 0; i < cs.index; i++, ccontour = ccontour->h_next);

		CvPoint* points = static_cast<CvPoint*>(malloc(ccontour->total * sizeof(CvPoint)));
		cvCvtSeqToArray(ccontour, points, CV_WHOLE_SEQ);
		std::vector<Point2i> c(points, points + ccontour->total);
		free(points);

		/*if(output){
			std::vector<std::vector<Point2i>> cDraw;
			cDraw.push_back(c);
			drawContours(draw, cDraw, -1, Scalar(0, 255, 0));
		}*/

		Point2f center;
		float radius;
		minEnclosingCircle(c, center, radius);

		Moments M = moments(c);
		center = { (float) (M.m10 / M.m00), (float) (M.m01 / M.m00) };

		if(output){
			circle(draw, center, radius, Scalar(150, 150, 0));
		}

		float diff = 0;
		int count = 0;
		for(const auto& p : c){
			float distance = sqrt(pow(p.x - center.x, 2) + pow(p.y - center.y, 2));
			diff += abs(distance - radius);
			count++;
		}
		diff /= count;

		char buf[200];
		sprintf(buf, "%.2f", diff);
		//putText(draw, buf, { (int) center.x, (int) center.y }, FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(255, 0, 0));

		if(diff >= 1){
			contourSizes.pop();
			continue;
		}

		if(output){
			circle(draw, center, radius, Scalar(255, 0, 255), 1);
		}

		Mat colorMask(hsv.rows, hsv.cols, CV_8UC1, Scalar(0));
		std::vector<std::vector<Point2i>> colorMaskContours;
		colorMaskContours.push_back(c);
		drawContours(colorMask, colorMaskContours, -1, Scalar(255), FILLED);
		Scalar m = mean(hsv, colorMask);

		balls.push_back({ (uint8_t) m[0], diff, Point2i { (int) center.x * 2, (int) center.y * 2 }, (uint) radius * 2 });

		contourSizes.pop();
	}

	if(output != nullptr){
		resize(draw, draw, Size(), 2, 2, INTER_NEAREST);
		memcpy(output, draw.data, 160 * 120 * 2);
	}

	return balls;
}
