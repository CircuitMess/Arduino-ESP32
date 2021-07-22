#ifndef TAGDETECTION_MARKER_H
#define TAGDETECTION_MARKER_H

#include "MarkerInfo.h"

namespace Aruco {

/**
 * Class is used to represent aruco markers.
 * All markers are assumed to be 5x5 markers capable of 1024 options, after considering rotation and error detection.
 */
class Marker {
public:
	/**
	 * Contain all the cells in the marker.
	 * Cells are encoded as [x][y] from left to right on X and from up to down on Y.
	 */
	int cells[7][7];

	/**
	 * Number of rows used to store data in the marker.
	 */
	int rows;

	/**
	 * Number of cols used to store data in the marker.
	 */
	int cols;

	/**
	 * Number of 90 degrees turns applied to the marker.
	 */
	int rotation;

	/**
	 * Id of this markers.
	 * Can be calculated after filling up the cells info.
	 */
	int id;

	/**
	 * Flag to store if the marker was already validated.
	 */
	int validated;

	/**
	 * Store info about the marker real world morphology and location.
	 */
	MarkerInfo info;

	/**
	 * Projected corner points in camera coordinates.
	 * This vector should have only 4 points.
	 * But can be used to store extra known points for the marker.
	 */
	std::vector<cv::Point2f> projected;

	/**
	 * Aruco marker constructor.
	 */
	Marker();

	/**
	 * Attach info to this marker.
	 *
	 * @param info Marker information.
	 */
	void attachInfo(MarkerInfo _info);

	/**
	 * Get the id of the marker. The ID its a value between 0 and 1024.
	 *
	 * @return ID of this aruco marker.
	 */
	int calculateID();

	/**
	 * Calculate all parameters and check if its a valid aruco marker.
	 * Should be called only after projected points and cell info is added.
	 * @return true if the marker is valid, false otherwise.
	 */
	bool validate();

	/**
	 * Used to rotate marker 90 degrees.
	 * Can be used to rotate the marker data and make sure that it is read properly.
	 */
	void rotate();

	/**
	 * Calculates the sum of the hamming distance (number of diferent bits) for this marker relative to the ids matrix used to validate the aruco markers.
	 */
	int hammingDistance();

	/**
	 * Print marker cells to the stdout.
	 */
	void print();
};

}

#endif //TAGDETECTION_MARKER_H
