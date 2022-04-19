/*
 * cvShortcuts.h
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_CVSHORTCUTS_CVSHORTCUTS_H_
#define INCLUDE_CVSHORTCUTS_CVSHORTCUTS_H_

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <limits>

namespace CVShortcuts {

/**
 * Similar to cv::line, but can draw dashed lines as well.
 * @param img
 * @param pt1
 * @param pt2
 * @param colour
 * @param type
 * @param thickness
 * @param lineType
 * @param shift
 */
void drawLine(cv::Mat &img, cv::Point pt1, cv::Point pt2, const cv::Scalar &colour, int type=0, int thickness=1, int lineType=cv::LINE_AA, int shift=0);
int squaredDistance(const cv::Point &p1, const cv::Point &p2);
void show(const char *window, cv::Mat &img, double scale);
void showChannels(const char *window, cv::Mat &img, double scale);
void minMaxLoc(const std::vector<cv::Point> &points, cv::Point &min, cv::Point &max);
double meanHistogram(cv::Mat &hist, int MAX);
void onMouseShowXY(int event, int x, int y, int, void*);

}



#endif /* INCLUDE_CVSHORTCUTS_CVSHORTCUTS_H_ */
