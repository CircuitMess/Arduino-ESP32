#ifndef TAGDETECTION_ARUCO_H
#define TAGDETECTION_ARUCO_H

#include <opencv2/core/types.hpp>
#include "Marker.h"

namespace Aruco {
	/**
	 * Get aruco marker bits data.
	 * @param image Square image with the aruco marker.
	 * @return Binary image with the aruco code  the image has resolution (cols + 2, rows + 2)
	 */
	cv::Mat processImage(const cv::Mat& image);

	/**
	 * Read aruco data from binary image with exactly the same size of the aruco marker.
	 * @param binary Binary image containing aruco info.
	 * @return ArucoMarker with the information collected.
	 */
	Marker readData(const cv::Mat& binary);

};

#endif //TAGDETECTION_ARUCO_H
