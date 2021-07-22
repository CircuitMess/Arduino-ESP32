#ifndef TAGDETECTION_IMGPROC_H
#define TAGDETECTION_IMGPROC_H

#include <opencv2/core/types.hpp>

void adaptive(const cv::Mat& input, cv::Mat& output, int blockSize);
float angleCornerPointsCos(const cv::Point& b, const cv::Point& c, const cv::Point& a);
cv::Mat deformQuad(const cv::Mat& image, const cv::Point2i& size, const std::vector<cv::Point2f>& quad);
cv::Mat rotationMatrix(const cv::Point3d& euler);

#endif //TAGDETECTION_IMGPROC_H
