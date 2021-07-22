#include "Aruco.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

using namespace Aruco;
using namespace cv;

Mat Aruco::processImage(const Mat& image){
	Mat aruco;
	resize(image, aruco, Size(7, 7), 0, 0, INTER_LINEAR);

	Mat binary;
	threshold(aruco, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

	return aruco;
}

/**
 * Read aruco data from binary image with exactly the same size of the aruco marker.
 * @param binary Binary image containing aruco info.
 * @return ArucoMarker with the information collected.
 */
Marker Aruco::readData(const Mat& binary){
	Marker marker = Marker();

	for(unsigned int i = 0; i < binary.cols * binary.rows; i++){
		marker.cells[i / binary.cols][i % binary.cols] = (binary.data[i] == 255);
	}

	return marker;
}