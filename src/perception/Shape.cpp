/*
 * Shape.cpp
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#include "perception/Shape.h"

#define mc_dbgmsg(msg)	std::cout << __FILE__ << ":" <<  __FUNCTION__ << ":" << __LINE__ << " \e[1;33m" << msg << "\e[0m" << std::endl;

namespace Perception{

	///
	/// Shape
	///

	Shape::~Shape() {}
	void Shape::setMargin(int margin) { this->margin = margin; }
	int Shape::getMargin() { return margin; }

	std::ostream& operator << (std::ostream &o, const Shape& s){
		s.write(o);
		return o;
	}

	std::istream& operator >> (std::istream &i, Shape** s){
		Shape *shape;
		char ch;
		i >> ch;  if(ch != '['){
			std::cerr << "Format error: Shape expected '['" << std::endl;
			return i;
		}
		std::string str;
		i >> str;
		std::cout << "Shape:" << str << std::endl;
		if(str.compare("circle") == 0){
			shape = new Circle();
			shape->readData(i);
		}else if(str.compare("rectangle") == 0){
			shape = new Rectangle();
			shape->readData(i);
		}else if(str.compare("intpolygon") == 0){
			shape = new IntPolygon();
			shape->readData(i);
		}else{
			shape = 0;
		}
		i >> ch;  if(ch != ']'){
			std::cerr << "Format error: Shape expected ']'" << std::endl;
			return i;
		}
		*s = shape;
		return i;
	}

	///
	/// CIRCLE
	///

	Geometry::Point2D Circle::getCentre() const { return c; }
	void Circle::setCentre(Geometry::Point2D c) { this->c = c; }
	double Circle::getRadius() const { return r; }

	ImageShape Circle::getShapeType() const { return CIRCLE; }
	std::ostream& Circle::write(std::ostream &o) const{
		return o << "[circle " << this->c << " " << this->r << "]";
	}
	std::istream& Circle::readData(std::istream &i){
		i >> this->c;
		i >> this->r;
		return i;
	}
	void Circle::draw(cv::Mat &img, cv::Scalar colour, int thickness) const{
		cv::circle(img, {c.x(), c.y()}, (int)r, colour, thickness);
	}

	///
	/// RECTANGLE
	///

	Geometry::Point2D Rectangle::getR1() const { return r1; }
	Geometry::Point2D Rectangle::getR2() const { return r2; }

	ImageShape Rectangle::getShapeType() const { return RECTANGLE; }
	std::ostream& Rectangle::write(std::ostream &o) const{
		return o << "[rectangle " << this->r1 << " " << this->r2 << "]";
	}
	std::istream& Rectangle::readData(std::istream &i){
		i >> this->r1;
		i >> this->r2;
		return i;
	}
	void Rectangle::draw(cv::Mat &img, cv::Scalar colour, int thickness) const{
		cv::rectangle(img, {(int)r1.u(), (int)r1.v()}, {(int)r2.u(), (int)r2.v()}, colour, thickness);
	}

	///
	/// INTPOLYGON
	///

	IntPolygon::IntPolygon() : maxDist(100), minDist(50), minAng(15) {}
	const CvPoints& IntPolygon::getPoints() const { return points; }
	CvPoints& IntPolygon::accessPoints() { return points; }
	int IntPolygon::getMaxDist() { return maxDist; }
	int IntPolygon::getMinDist() { return minDist; }
	double IntPolygon::getMinAng() { return minAng; }

	void IntPolygon::assignPoints(CvPoints &newPoints){
		points.clear();
		points.assign(newPoints.begin(), newPoints.end());
	}

	void IntPolygon::calculateMark(cv::Mat &img, int &x1, int &x2, int &y1, int &y2) const{
		const int MARGIN = 75;
		cv::Point lc, rc;
		CVShortcuts::minMaxLoc(points, lc, rc);
		x1 = lc.x - MARGIN;
		x2 = rc.x + MARGIN;
		y1 = lc.y - MARGIN;
		y2 = rc.y + MARGIN;
		if(x1 > x2) { int temp = x2; x2 = x1; x1 = temp; }
		if(y1 > y2) { int temp = y2; y2 = y1; y1 = temp; }
		if(x1 < 0) x1 = 0;
		if(x2 > img.cols) x2 = img.cols;
		if(y1 < 0) y1 = 0;
		if(y2 > img.rows) y2 = img.rows;
	}

	void IntPolygon::makeRegular(int maxDistance, int minDistance, int minAngle, ContourHistory* ch)
	{
		if(minDistance > maxDistance){
			std::cerr << "IntPolygon::makeRegular$ If minDistance > maxDistance this algorithm would not finish." << std::endl;
			return;
		}
		maxDist = maxDistance;
		minDist = minDistance;
		minAng = minAngle;
		double maxCos2 = cos(3.141592 * minAngle / 180);
		maxCos2 *= maxCos2;
		if(points.size() > 1)
		{
			points.push_back(points[0]);
		} else {
			return;
		}
		for(CvPoints::iterator start = points.begin() + 1, it = start;
			it < points.end();
			start = points.begin() + 1)
		{
			cv::Point p1 = *(it - 1);
			cv::Point p2 = *it;

			int x2 = (p2.x - p1.x);
			int y2 = (p2.y - p1.y);
			int length2 = x2*x2 + y2*y2;

			bool notlast = it != (points.end() - 1);
			cv::Point p3;
			int bx, by;
			double angle;
			double dot;
			if(notlast)
			{
				p3 = *(it + 1);
				//xO2 = (p3.x - p1.x);
				//yO2 = (p3.y - p1.y);
				bx = (p3.x - p2.x);
				by = (p3.y - p2.y);
				dot = -x2 * bx - y2 * by;
				angle = 1.0 * dot * dot / (length2 * (bx*bx + by*by));
			}
			if(length2 > maxDistance * maxDistance)
			{
				// Insert
				cv::Point newPoint((p2.x + p1.x) / 2, (p2.y + p1.y) / 2);
				if (ch != NULL)
				{
					ch->createControlPointHistory(it - points.begin(), newPoint.x, newPoint.y, points.size());
				}
				it = points.insert(it, newPoint);
				//mc_dbgmsg( "Inserted point" )
			} else if(length2 < minDistance * minDistance) {
				// Erase
				if(notlast)
				{
					it = points.erase(it);
					if (ch != NULL)
					{
						ch->removeControlPoint(it - points.begin());
					}
					//mc_dbgmsg( "Removed point from middle" )
				} else {
					points.erase(it);
					points.erase(points.begin());
					if(points.size() > 1)
					{
						points.push_back(points[0]);
					} else {
						return;
					}
					if (ch != NULL)
					{
						ch->removeControlPoint(0);
					}
					//mc_dbgmsg( "Removed first point" )
				}
			} else if(dot > 0 && angle > maxCos2) {
				// Erase if it is a degenerated peak, angle < minAng deg.
				if(it != start)
				{

					if (ch != NULL)
					{
						ch->removeControlPoint(it - points.begin());
					}
					it = (points.erase(it) - 1);
					//mc_dbgmsg( "Removed peek middle point" )

				} else {
					points.erase(points.end() - 1);
					points.erase(points.begin());
					if(points.size() > 1)
					{
						points.push_back(points[0]);
					} else {
						return;
					}
					if (ch != NULL)
					{
						ch->removeControlPoint(0);
					}
					//mc_dbgmsg( "Removed peek end point" )
				}
			} else {
				++it;
			}
		}
		if(points.size() > 1)
		{
			points.pop_back();
		}
		//mc_dbgmsg( std::endl << std::endl << std::endl << std::endl << std::endl )
	}

	ImageShape IntPolygon::getShapeType() const { return INTPOLYGON; }

	std::ostream& IntPolygon::write(std::ostream &o) const{
		o << "[intpolygon";
		for(std::vector<cv::Point>::const_iterator it = points.begin(); it < points.end(); ++it){
			//o << " " << *it;
			o << " [" << it->x << ", " << it->y << "]";
		}
		return o << "]";
	}

	std::istream& IntPolygon::readData(std::istream &i){
		if(points.size() > 0){
			points.clear();
		}
		char ch;
		ch = i.peek();
		while(ch != ']'){
			cv::Point p;
			i >> ch; if(ch != '['){
				std::cerr << "Format error: Shape expected '['" << std::endl;
				return i;
			}
			i >> p.x;
			i >> ch; if(ch != ','){
				std::cerr << "Format error: Shape expected ','" << std::endl;
				return i;
			}
			i >> p.y;
			i >> ch; if(ch != ']'){
				std::cerr << "Format error: Shape expected ']'" << std::endl;
				return i;
			}
			points.push_back(p);
			ch = i.peek();
		}
		return i;
	}

	void IntPolygon::draw(cv::Mat &img, const cv::Scalar colour, int thickness) const{
		int npts = points.size();
		const cv::Point *pts =(const cv::Point*)(points.data());
		if(thickness == -1){
			cv::fillPoly(img, &pts, &npts, 1, colour, cv::LINE_AA);
		}else{
			for(std::vector<cv::Point>::const_iterator it = points.begin();
				it < points.end() - 1; ++it){
				cv::line(img, *it, *(it + 1), colour, thickness);
				cv::circle(img, *it, 1, cv::Scalar(0, 0, 255));
			}
			if(points.size() > 1){
				cv::line(img, points[0], points[points.size() - 1], colour, thickness);
				cv::circle(img, points[points.size() - 1], 1, cv::Scalar(200, 200, 255));
			}
		}
	}
}

