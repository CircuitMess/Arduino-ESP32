#ifndef TAGDETECTION_QUADRILATERAL_H
#define TAGDETECTION_QUADRILATERAL_H

#include <opencv2/core/types.hpp>
#include <vector>

/**
 * Used to represent and operate over quads.
 */
class Quadrilateral {
public:
	/**
	 * Vector of points that compose the quad.
	 */
	std::vector<cv::Point2f> points;

	/**
	 * Quad constructor initializes 4 points.
	 */
	Quadrilateral();

	/**
	 * Quad contructor from a points.
	 */
	Quadrilateral(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, const cv::Point2f& d);

	/**
	 * Calculate Area of this quad.
	 *
	 * @return Area of this quad.
	 */
	float area() const;

	/**
	 * Check if point is inside this quad.
	 *
	 * @param p Point to check.
	 * @return true if point is inside this quad
	 */
	bool containsPoint(const cv::Point2f& p) const;

	/**
	 * Draw quad lines to image.
	 *
	 * @param image Image where to draw line.
	 * @param color Color of the lines to be drawn.
	 * @param weight Weight of the lines.
	 */
	void draw(cv::Mat& image, const cv::Scalar& color, int weigth = 1) const;

	/**
	 * Print quad info to cout.
	 */
	void print() const;

	/**
	 * Get bigger square on a std::vector (caller have to check std::vector size).
	 *
	 * @param quad Vector of quads.
	 * @return quad Bigger quad in the std::vector.
	 */
	Quadrilateral biggerQuadrilateral(const std::vector<Quadrilateral>& quads) const;

	/**
	 * Draw all squares from std::vector to image.
	 *
	 * @param image Image where to draw.
	 * @param quads Vector of quads to draw.
	 * @param color Color used to draw the quads.
	 */
	void drawVector(cv::Mat& image, const std::vector<Quadrilateral>& quads, const cv::Scalar& color) const;
};


#endif //TAGDETECTION_QUADRILATERAL_H
