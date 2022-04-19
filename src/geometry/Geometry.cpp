/*
 * Geometry.cpp
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#include "geometry/Geometry.h"

namespace Geometry{

	Point2D::Point2D() : du(0), dv(0) {}
	Point2D::Point2D(double u, double v) : du(u), dv(v) {}
	void Point2D::u(double u) { this->du = u; }
	void Point2D::v(double v) { this->dv = v; }
	double Point2D::u() const { return du; }
	double Point2D::v() const { return dv; }
	int Point2D::x() const {return (int)du; }
	int Point2D::y() const {return (int)dv; }
	Point2D Point2D::operator+(const Point2D &rhs) const{
		return Point2D(du + rhs.du, dv + rhs.dv);
	}
	Point2D Point2D::operator-(const Point2D &rhs) const{
		return Point2D(du - rhs.du, dv - rhs.dv);
	}
	Point2D Point2D::operator*(const Point2D &rhs) const{
		return Point2D(du * rhs.u(), dv * rhs.v());
	}
	Point2D Point2D::operator*(double rhs) const{
		return Point2D(du * rhs, dv * rhs);
	}
	Point2D Point2D::operator/(double rhs) const{
		return Point2D(du / rhs, dv / rhs);
	}
	double Point2D::dot(const Point2D &rhs){
		return du * rhs.du + dv * rhs.dv;
	}
	double Point2D::crossW(const Point2D &rhs){
		return du * rhs.dv - dv * rhs.du;
	}
	double Point2D::squaredLength() const { return du*du + dv*dv; }

	/**
     * If the norm of the vertex is zero it returns a unitary
	 * vector pointing up (0,1) otherwise returns a vector in the same
     * direction than <code>this</code> but unit length.
     */
	const Point2D Point2D::normalised(){
		double norm = sqrt(squaredLength());
		if(norm == 0) return Point2D(0, 1.0);
		return (*this) / norm;
	}

	cv::Point Point2D::getCVCoordinates(int height, Point2D offset, Point2D scale) const{
		double tx = du, ty = dv;
		if(scale.du != 1.0) tx = scale.du * du;
		if(scale.dv != 1.0) ty = scale.dv * dv;
		int x = (int)(tx + offset.u());
		int y = (int)(ty + offset.v());
		return cv::Point(x, height - y);
	}

	/**
	 * The point is assumed to be in an euclidean space.
	 * To draw it in OpenCv the object it belongs to may be scaled and placed
	 * in a different position in the environment, thus giving an offset.
	 * The offset is with respect to the world coordinates and before inverting
	 * the y coordinate to adjust to screen coordinates.
     * @param img
     * @param colour
     * @param offset
     * @param scale
     */
	void Point2D::draw(cv::Mat& img, cv::Scalar colour, Point2D offset, Point2D scale){
		cv::circle(img, getCVCoordinates(img.rows, offset, scale), 2, colour, -1);
	}
	void Point2D::drawLine(cv::Mat& img, const Point2D &p, cv::Scalar colour, Point2D offset, Point2D scale){
		cv::line(img, getCVCoordinates(img.rows, offset, scale),
			p.getCVCoordinates(img.rows, offset, scale), colour);

	}

	/**
	* Adapted from:
	* http://stackoverflow.com/questions/10161351/opencv-how-to-plot-velocity-vectors-as-arrows-in-using-single-static-image
	* @param to			Direction the arrow points to
	*/
	void Point2D::drawArrow(cv::Mat &img, Point2D to, double arrowScale,
		cv::Scalar colour, int thickness, Point2D offset, Point2D scale){
		if(arrowScale != 1.0){
			to.u(to.u() * arrowScale);
			to.v(to.v() * arrowScale);
		}
		to = to * scale;

		cv::Point pt1, pt2;
		pt1 = getCVCoordinates(img.rows, offset, scale);
		pt2.x = pt1.x + (int)to.u();
		pt2.y = pt1.y - (int)to.v();

		//std::cout << "Arrow 2 KD: fx = " << u << " fy = " << v << " p = " << pt1 << std::endl;
		cv::line(img, pt1, pt2, colour, thickness, cv::LINE_AA);  //Draw Line
		cv::circle(img, pt1, 2, cv::Scalar(50, 50, 255), -1);

		// Arrow wings
		double l = sqrt(to.squaredLength());
		double spinSize = l * 0.4; //double spinSize = 5.0 * l/maxLength;

		double angle;
		double PI = 3.1416;
		if(to.u() == 0 && to.v() == 0){
			return;
		}
		angle = atan2(-to.v(), -to.u());

		pt1.x = (int) (pt2.x + spinSize * cos(angle + PI / 6));
		pt1.y = (int) (pt2.y - spinSize * sin(angle + PI / 6));
		cv::line(img, pt1, pt2, colour, thickness, cv::LINE_AA);  //Draw Line

		pt1.x = (int) (pt2.x + spinSize * cos(angle - PI / 6));
		pt1.y = (int) (pt2.y - spinSize * sin(angle - PI / 6));
		cv::line(img, pt1, pt2, colour, thickness, cv::LINE_AA);  //Draw Line

	}
	void Point2D::drawCircle(cv::Mat &img, double radious, cv::Scalar colour, Point2D offset, Point2D scale){
		if(scale.u() == scale.v()){
			cv::circle(img, getCVCoordinates(img.rows, offset, scale), int(radious * scale.u()), colour, -1);
		}else std::cerr << "This is not a circle" << std::endl;
	}
	std::ostream& operator << (std::ostream &o, const Point2D& p) {
		return o << "(" << p.u() << "," << p.v() << ")";
	}
	std::istream& operator >> (std::istream &i, Point2D& p){
		char ch;
		i >> ch; if(ch != '(') std::cerr << "Format error: Point2D expected '('" << std::endl;
		i >> p.du;
		i >> ch; if(ch != ',') std::cerr << "Format error: Point2D expected ','" << std::endl;
		i >> p.dv;
		i >> ch; if(ch != ')') std::cerr << "Format error: Point2D expected ')'" << std::endl;
		return i;
	}

	/**
	 * Returns whether the points of the polygon are listed in counter-clockwise
	 * order.
     * @param polygon
     * @return
     */
	bool isPolygonCWW(std::vector<Point2D> &polygon){
		if (polygon.size() < 2) return true;
		double sum = 0;
		std::vector<Point2D>::iterator it = polygon.begin();
		std::vector<Point2D>::iterator end = polygon.end() - 1;
		if(it != end) { sum += (it->u() - end->u()) * (it->v() + end->v());}
		while(it != end){
			sum += ((it+1)->u() - it->u()) * ((it+1)->v() + it->v());
			++it;
		}
		if(sum > 0) return false;
		else return true;
	}

}


