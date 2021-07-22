#ifndef TAGDETECTION_MARKERINFO_H
#define TAGDETECTION_MARKERINFO_H

#include <vector>
#include <opencv2/core/types.hpp>

namespace Aruco {

/**
 * Class is used to store info about a marker.
 * The info present in this class is related with real world information.
 * Units are store in meters for positions and dimensions and radians for angles.
 */
class MarkerInfo {
public:
	/**
	 * Marker ID.
	 */
	int id;

	/**
	 * Size of the marker in real position scale.
	 */
	double size;

	/**
	 * Marker position in real world coordinates.
	 */
	cv::Point3d position;

	/**
	 * Euler rotation of this marker.
	 */
	cv::Point3d rotation;

	/**
	 * World corner points calculated from position and rotation of the marker.
	 * This vector should have only 4 points.
	 * But can be used to store extra known points for the marker.
	 * World points stored in the following order: TopLeft, TopRight, BottomRight, BottomLeft.
	 */
	std::vector <cv::Point3f> world;

	/**
	 * Default constructor with id -1.
	 */
	MarkerInfo();

	/**
	 * Aruco marker constructor.
	 * @param id Marker id.
	 * @param size Marker size in meters.
	 * @param position Marker world position.
	 */
	MarkerInfo(int _id, double _size, cv::Point3f _position);

	/**
	 * Aruco marker constructor.
	 * @param id Marker id.
	 * @param size Marker size in meters.
	 * @param position Marker world position.
	 * @param rotation Marker world euler rotation.
	 */
	MarkerInfo(int _id, double _size, cv::Point3f _position, cv::Point3f _rotation);

	/**
	 * Calculate the marker world points, considering the marker center position and rotation.
	 * First the marker is rotated and is translated after so the rotation is always relative to the marker center.
	 */
	void calculateWorldPoints();

	/**
	 * Print info about this marker to the stdout.
	 */
	void print();
};

}

#endif //TAGDETECTION_MARKERINFO_H
