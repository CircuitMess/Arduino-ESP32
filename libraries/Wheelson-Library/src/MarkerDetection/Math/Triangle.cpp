#include "Triangle.h"

using namespace cv;

Triangle::Triangle(){
	for(int i = 0; i < 3; i++){
		points[i] = Point2f(0.0, 0.0);
	}
}

/**
 * Triangle contructor from corners.
 * @param a Corner a.
 * @param b Corner b.
 * @param c Corner c.
 */
Triangle::Triangle(const Point2f& a, const Point2f& b, const Point2f& c){
	points[0] = a;
	points[1] = b;
	points[2] = c;
}

/**
 * Calculate the area of this triangle.
 * @return Area of this triangle.
 */
float Triangle::area() const{
	return abs(points[0].x * (points[1].y - points[2].y) + points[1].x * (points[2].y - points[0].y) + points[2].x * (points[0].y - points[1].y)) / 2.0;
}

/**
 * Check if this triangle collides with another triangle.
 * @return True if triangles are colliding, false otherwise.
 */
bool Triangle::isColliding(const Triangle& t) const{
	return containsPoint(t.points[0]) || containsPoint(t.points[1]) || containsPoint(t.points[2]) || t.containsPoint(points[0]) || t.containsPoint(points[1]) ||
		   t.containsPoint(points[2]);
}

/**
 * Check if the point is inside this triangle.
 * @param p Point to check.
 * @return True if point is inside triangle, false otherwise.
 */
bool Triangle::containsPoint(const Point2f& p) const{
	bool b1 = sign(p, points[0], points[1]) <= 0.0;
	bool b2 = sign(p, points[1], points[2]) <= 0.0;
	bool b3 = sign(p, points[2], points[0]) <= 0.0;

	return ((b1 == b2) && (b2 == b3));
}

/**
 * Sign operation used to check if point a is before or after the line composed for b and c.
 * @param a Point a.
 * @param b Point b.
 * @param c Point c.
 * @return Float value if less than 0 point is before line.
 */
float Triangle::sign(const Point2f& a, const Point2f& b, const Point2f& c) const{
	return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
}