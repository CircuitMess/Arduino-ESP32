#ifndef TAGDETECTION_TRIANGLE_H
#define TAGDETECTION_TRIANGLE_H

#include <opencv2/core/types.hpp>

/**
 * Class is used to represent and manipulate triangles.
 */
class Triangle {
public:
	/**
	 * Triangle points.
	 */
	cv::Point2f points[3];

	/**
	 * Default triangle contructor.
	 */
	Triangle();

	/**
	 * Triangle contructor from corners.
	 * @param a Corner a.
	 * @param b Corner b.
	 * @param c Corner c.
	 */
	Triangle(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c);

	/**
	 * Calculate the area of this triangle.
	 * @return Area of this triangle.
	 */
	float area() const;

	/**
	 * Check if this triangle collides with another triangle.
	 * @return True if triangles are colliding, false otherwise.
	 */
	bool isColliding(const Triangle& t) const;

	/**
	 * Check if the point is inside this triangle.
	 * @param p Point to check.
	 * @return True if point is inside triangle, false otherwise.
	 */
	bool containsPoint(const cv::Point2f& p) const;

	/**
	 * Sign operation used to check if point a is before or after the line composed for b and c.
	 * @param a Point a.
	 * @param b Point b.
	 * @param c Point c.
	 * @return Float value if less than 0 point is before line.
	 */
	float sign(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c) const;
};


#endif //TAGDETECTION_TRIANGLE_H
