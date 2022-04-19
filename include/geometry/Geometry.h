/*
 * Geometry.h
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_GEOMETRY_GEOMETRY_H_
#define INCLUDE_GEOMETRY_GEOMETRY_H_

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

namespace Geometry {

class Point2D
{
private:
	double du, dv;

public:
	Point2D();
	Point2D(double u, double v);
	void u(double u);
	void v(double v);
	double u() const;
	double v() const;
	int x() const;
	int y() const;
	Point2D operator+(const Point2D &rhs) const;
	Point2D operator-(const Point2D &rhs) const;
	Point2D operator*(const Point2D &rhs) const;
	Point2D operator*(double rhs) const;
	Point2D operator/(double rhs) const;
	double dot(const Point2D &rhs);
	double crossW(const Point2D &rhs);
	double squaredLength() const;
	const Point2D normalised();
	cv::Point getCVCoordinates(int height, Point2D offset, Point2D scale) const;
	void draw(cv::Mat& img, cv::Scalar colour = cv::Scalar(250, 0, 0), Point2D offset = Point2D(0,0), Point2D scale = Point2D(1.0,1.0));
	void drawLine(cv::Mat& img, const Point2D &p, cv::Scalar colour = cv::Scalar(200, 200, 200), Point2D offset = Point2D(0,0), Point2D scale = Point2D(1.0,1.0));
	void drawArrow(cv::Mat &img, Point2D to, double arrowScale = 1.0, cv::Scalar colour = cv::Scalar(255, 0, 255), int thickness = 1, Point2D offset = Point2D(0,0), Point2D scale = Point2D(1.0,1.0));
	void drawCircle(cv::Mat &img, double radious, cv::Scalar colour = cv::Scalar(200, 10, 200), Point2D offset = Point2D(0,0), Point2D scale = Point2D(1.0,1.0));
	friend std::istream& operator >> (std::istream &i, Point2D& p);
};

std::ostream& operator << (std::ostream &o, const Point2D& p);
std::istream& operator >> (std::istream &i, Point2D& p);

bool isPolygonCWW(std::vector<Point2D> &polygon);

}

#endif /* INCLUDE_GEOMETRY_GEOMETRY_H_ */
