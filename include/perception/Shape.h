/*
 * Shape.h
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_PERCEPTION_SHAPE_H_
#define INCLUDE_PERCEPTION_SHAPE_H_

#include "geometry/Geometry.h"
#include "cvshortcuts/cvShortcuts.h"
#include "perception/VisualHistoryKeeper.h"

namespace Perception {

enum ImageShape { CIRCLE, RECTANGLE, INTPOLYGON };

/**
 * Base virtual class for every shape.
 */
class Shape
{
protected:
	int margin;		// Margin of interest around the polygon.

	virtual std::istream& readData(std::istream &i) = 0;

public:
	void setMargin(int margin);
	int getMargin();
	virtual ImageShape getShapeType() const = 0;
	virtual std::ostream& write(std::ostream &o) const = 0;
	virtual void draw(cv::Mat &img, cv::Scalar colour = cv::Scalar(250, 0, 0), int thickness = 1) const = 0;
	friend std::istream& operator >> (std::istream &i, Shape **s);
	virtual ~Shape();
};
std::ostream& operator << (std::ostream &o, const Shape& s);
std::istream& operator >> (std::istream &i, Shape **s);



class Circle : public Shape {
private:
	Geometry::Point2D c;
	double r;

	std::istream& readData(std::istream &i);
public:
	Geometry::Point2D getCentre() const;
	void setCentre(Geometry::Point2D c);
	double getRadius() const;

	virtual ImageShape getShapeType() const;
	std::ostream& write(std::ostream &o) const;
	void draw(cv::Mat &img, cv::Scalar colour = cv::Scalar(250, 250, 0), int thickness = 1) const;
};



class Rectangle : public Shape {
private:
	Geometry::Point2D r1;
	Geometry::Point2D r2;

	std::istream& readData(std::istream &i);
public:
	Geometry::Point2D getR1() const;
	Geometry::Point2D getR2() const;

	virtual ImageShape getShapeType() const;
	std::ostream& write(std::ostream &o) const;
	void draw(cv::Mat &img, cv::Scalar colour = cv::Scalar(0, 250, 0), int thickness = 1) const;
};

class ContourHistory;
typedef std::vector<cv::Point> CvPoints;
class IntPolygon : public Shape {
private:
	std::istream& readData(std::istream &i);
	CvPoints points;
	int maxDist;
	int minDist;
	double minAng;
public:
	IntPolygon();
	const CvPoints& getPoints() const;
	CvPoints& accessPoints();
	int getMaxDist();
	int getMinDist();
	double getMinAng();
	void assignPoints(CvPoints &newPoints);

	/**
	 * Calculates the left corner and right corner of a rectangle in <code>img</code>
	 * with margin <code>MARGIN</code> around <code>x1,x2,y1,y2</code>, as long
	 * as it doesn't go out the image.
	 * @param img
	 * @param x1
	 * @param x2
	 * @param y1
	 * @param y2
	 */
	void calculateMark(cv::Mat &img, int &x1, int &x2, int &y1, int &y2) const;

	/**
	 * Destroys and adds points so that the distance and angles between them are kept within
	 * the given constraints.
	 * @param maxDistance
	 * @param minDistance
	 * @param minAngle in degrees
	 */
	void makeRegular(int maxDistance, int minDistance = 100, int minAngle = 10, ContourHistory* ch = NULL);
	virtual ImageShape getShapeType() const;
	std::ostream& write(std::ostream &o) const;
	void draw(cv::Mat &img, cv::Scalar colour = cv::Scalar(0, 250, 0), int thickness = cv::LINE_AA) const;
};

}



#endif /* INCLUDE_PERCEPTION_SHAPE_H_ */
