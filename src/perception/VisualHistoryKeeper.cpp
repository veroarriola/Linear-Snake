/*
 * VisualHistoryKeeper.cpp
 *
 *  Created on: 28/06/2017
 *      Author: blackzafiro
 */

#include "perception/VisualHistoryKeeper.h"
#include "exceptions/Exceptions.h"
#include <cassert>

#define mc_dbgmsg(msg)	std::cout << __FILE__ << ":" <<  __FUNCTION__ << ":" << __LINE__ << std::endl << "\e[1;36m" << msg << "\e[0m" << std::endl;

FileFormatException ffexception("Found wrong character while parsing file");
IllegalArgumentException iargexception("Control point was not alive at given time");

namespace Perception {

const int UNDEAD = -1;

///
/// CONTROL POINT
///

ControlPoint::ControlPoint() :
		_x(-1), _y(-1), _prev_neighbour_index(-1), _next_neighbour_index(-1) {}
ControlPoint::ControlPoint(int x, int y, int prev_neighbour_index, int next_neighbour_index) :
	_x(x), _y(y), _prev_neighbour_index(prev_neighbour_index), _next_neighbour_index(next_neighbour_index) {}
ControlPoint::ControlPoint(const ControlPoint& other) :
		_x(other._x), _y(other._y),
		_prev_neighbour_index(other._prev_neighbour_index),
		_next_neighbour_index(other._next_neighbour_index) {}

ControlPoint& ControlPoint::operator=(const ControlPoint& other)
{
	if (this != &other)		// self-assignment check expected
	{
		this->_x = other._x;
		this->_y = other._y;
		this->_prev_neighbour_index = other._prev_neighbour_index;
		this->_next_neighbour_index = other._next_neighbour_index;
	}
	return *this;
}

void ControlPoint::set(int x, int y, int prev_neighbour_index, int next_neighbour_index)
{
	this->_x = x;
	this->_y = y;
	this->_prev_neighbour_index = prev_neighbour_index;
	this->_next_neighbour_index = next_neighbour_index;
}
void ControlPoint::set(int x, int y)
{
	this->_x = x;
	this->_y = y;
}

int ControlPoint::x() { return _x; }
int ControlPoint::y() { return _y; }
int ControlPoint::prev_neighbour_index() { return _prev_neighbour_index; }
int ControlPoint::next_neighbour_index() { return _next_neighbour_index; }
void ControlPoint::prev_neighbour_index(int prev) { this->_prev_neighbour_index = prev; }
void ControlPoint::next_neighbour_index(int next) { this->_next_neighbour_index = next; }

std::ostream& ControlPoint::write(std::ostream &o) const
{
	return o << "[" << _x << " " << _y << " " << _prev_neighbour_index << " " << _next_neighbour_index << "]";
}

ControlPoint::~ControlPoint() {}


///
/// CONTROL POINT HISTORY
///

ControlPointHistory::ControlPointHistory(int ident, int birth_time) : //, int current_cvindex) :
		_ident(ident), _birth_time(birth_time), _death_time(UNDEAD) {} //, _current_cvindex(current_cvindex) {}

int ControlPointHistory::birthTime() const
{
	return _birth_time;
}

int ControlPointHistory::deathTime() const
{
	return _death_time;
}

ControlPoint& ControlPointHistory::getControlPoint(int time)
{
	if (time < _birth_time || (_death_time != UNDEAD && time >= _death_time))
	{
		std::cerr << "Time:"  << time << " Birth time:" << _birth_time << " Death time:" << _death_time << std::endl;
		throw iargexception;
	}
	return _control_point_history[time - _birth_time];
}

void ControlPointHistory::queue(ControlPoint &cpoint)
{
	_control_point_history.push_back(cpoint);
}
/*
ControlPoint ControlPointHistory::last()
{
	return _control_point_history.back();
}*/
void ControlPointHistory::update(int x, int y)
{
	ControlPoint cp = _control_point_history.back();
	cp.set(x, y);
	_control_point_history.push_back(cp);
}

ControlPoint& ControlPointHistory::back()
{
	return _control_point_history.back();
}

bool ControlPointHistory::isAlive()
{
	return _death_time == UNDEAD;
}

void ControlPointHistory::remove()
{
	_control_point_history.pop_back();
	_death_time = this->_birth_time + _control_point_history.size();
}

std::ostream& operator << (std::ostream &o, const ControlPointHistory& s)
{
	o << s._ident << "\t" << s._birth_time << "\t" << s._death_time << "\t[";
	CPHistory::const_iterator it = s._control_point_history.begin();
	CPHistory::const_iterator end = s._control_point_history.end();

	if(it != end)
	{
		it->write(o);
		++it;
	}
	for(; it < end; ++it)
	{
		o << " ";
		it->write(o);
	}
	return o << "]";
}

ControlPointHistory::~ControlPointHistory() {}

std::istream& operator >> (std::istream &is, ControlPointHistory *pcph)
{
	is >> pcph->_ident;
	is >> pcph->_birth_time;
	is >> pcph->_death_time;
	char ch;
	is >> ch;
	if (ch != '[')
	{
		std::cerr << "'[' was expected found "  << ch << " instead." << std::endl;
		throw ffexception;
	}

	// Read control points
	int x, y, prev, next;
	is >> ch;
	while (ch == '[')
	{
		is >> x;
		is >> y;
		is >> prev;
		is >> next;
		pcph->_control_point_history.push_back(ControlPoint(x, y, prev, next));
		is >> ch;
		if (ch != ']')
		{
			std::cerr << "']' was expected found "  << ch << " instead." << std::endl;
			throw ffexception;
		}
		is >> ch;
	}
	if (ch != ']')
	{
		std::cerr << "']' was expected found "  << ch << " instead." << std::endl;
		throw ffexception;
	}
	return is;
}


///
/// HISTORY KEEPER
///

HistoryKeeper::HistoryKeeper() {}
HistoryKeeper::~HistoryKeeper() {}

///
/// CONTOUR HISTORY
///
ContourHistory::ContourHistory() : HistoryKeeper(), _last_poly_length(0), _time_step(-1) {}
ContourHistory::ContourHistory(IntPolygon* poly) : HistoryKeeper(), _time_step(0)
{
	CvPoints& cvpoints = poly->accessPoints();
	CvPoints::const_iterator it = cvpoints.begin();
	CvPoints::const_iterator end = cvpoints.end();
	int length = (int)cvpoints.size();
	ControlPoint cp;	// Placeholder
	for(int i = 0; it != end; ++i, ++it)
	{
		ControlPointHistory cph(i, 0);//, i);
		_contour_history.push_back(cph);
		ControlPointHistory& hist = _contour_history[i];
		cp.set(it->x, it->y,
				(i - 1 < 0) ? length - 1 : i - 1,
				(i + 1) % length
		);
		hist.queue(cp);
		_cp_polygon_index.push_back(i);
	}
	_last_poly_length = length;
}

std::ostream& ContourHistory::writeHeaders(std::ostream &o) const
{
	return o << "Id\t\"Birth time\"\t\"Death time\"\t\"Control points [x y prev next]+\"" << std::endl;
}

std::ostream& ContourHistory::write(std::ostream &o) const
{
	CHistory::const_iterator it = _contour_history.begin();
	CHistory::const_iterator end = _contour_history.end();
	for(; it != end; ++it)
	{
		o << *it << std::endl;
	}
	return o;
}

void ContourHistory::updatePositions(IntPolygon *poly)
{
	CvPoints& cvpoints = poly->accessPoints();
	assert(cvpoints.size() == _last_poly_length);

	CvPoints::const_iterator it = cvpoints.begin();
	CvPoints::const_iterator end = cvpoints.end();

	_time_step++;
	for(int i = 0; it != end; ++i, ++it)
	{
		ControlPointHistory& hist = _contour_history[_cp_polygon_index[i]];
		hist.update(it->x, it->y);
	}
	//mc_dbgmsg("History time " << _time_step)
}

void ContourHistory::createControlPointHistory(int poly_index, int x, int y, unsigned int length)
{
	//mc_dbgmsg("Adding point " << poly_index << x << " " << y)

	// New history
	ControlPointHistory cph(_contour_history.size(), _time_step);//, poly_index);
	_contour_history.push_back(cph);

	// Set location of new history and update indexes greater than poly_index
	_cp_polygon_index.insert(
			_cp_polygon_index.begin() + poly_index,
			_contour_history.size() - 1);

	// Placeholder for control point data
	// remember index of neightbour must be index of control point in history
	int cvprev = (poly_index - 1 < 0) ? length - 1 : poly_index - 1;
	int cvnext = (poly_index + 1) % length;
	int prev = _cp_polygon_index[cvprev];
	int next = _cp_polygon_index[cvnext];
	ControlPoint newcp(x, y, prev, next);
	_contour_history.back().queue(newcp);
	_last_poly_length++;

	// Update neighbours of poly_index point neighbours.
	int newident = (int)(_contour_history.size() - 1);
	_contour_history[prev].back().next_neighbour_index(newident);
	_contour_history[next].back().prev_neighbour_index(newident);

}

void ContourHistory::removeControlPoint(int poly_index)
{
	//if (_cp_polygon_index[poly_index] == 0) { mc_dbgmsg( "!!!!!!!!!!!!!!! A T E N C I O N !!!!!!!!!!!!!!!!!!!") }
	//mc_dbgmsg("Removing point " << poly_index << " in " << _cp_polygon_index[poly_index] << " len=" << _contour_history.size())

	// Set death time
	ControlPointHistory& cphist = _contour_history[_cp_polygon_index[poly_index]];

	// Update neighbours of poly_index point neighbours.
	int prev = cphist.back().prev_neighbour_index();
	int next = cphist.back().next_neighbour_index();
	cphist.remove();
	_contour_history[prev].back().next_neighbour_index(next);
	_contour_history[next].back().prev_neighbour_index(prev);

	// Update index map
	//unsigned int dbgsize = _cp_polygon_index.size(); std::vector<int>::iterator dbgbegin = _cp_polygon_index.begin();
	//mc_dbgmsg("cvprev:" << cvprev << " in " << prev << "  cvnext:" << cvnext << " in " << next << "  cp in store:" << dbgsize << " " << " " << poly_index)
	_cp_polygon_index.erase(_cp_polygon_index.begin() + poly_index);
	_last_poly_length--;

}


void ContourHistory::reconstructContour(IntPolygon& poly, int time)
{
	CvPoints& points = poly.accessPoints();
	points.clear();

	// Find first point
	CHistory::const_iterator it = _contour_history.begin();
	//if (time == 18) mc_dbgmsg(time << " b->" << it->birthTime() << " d->" << it->deathTime() )
	while(it->birthTime() > time || (it->deathTime() <= time && it->deathTime() != UNDEAD))
	{
		// The point was not born or was already death
		++it;
		//if (time == 18) mc_dbgmsg( "b->" << it->birthTime() << " d->" << it->deathTime() )
	}
	mc_dbgmsg(time << " birth->" << it->birthTime() << " death->" << it->deathTime() )

	// Start polygon with first point found in history that belongs to it
	int index_of_first = it - _contour_history.begin();
	ControlPoint& point = _contour_history[index_of_first].getControlPoint(time);
	points.push_back(cv::Point(point.x(), point.y()));
	int index_of_next = point.next_neighbour_index();
	while(index_of_next != index_of_first)
	{
		//if (time == 18) mc_dbgmsg( index_of_next << " " << index_of_first )

		point = _contour_history[index_of_next].getControlPoint(time);
		points.push_back(cv::Point(point.x(), point.y()));
		index_of_next = point.next_neighbour_index();
	}
}

ContourHistory::~ContourHistory() {}

std::istream& operator >> (std::istream &is, ContourHistory *pch)
{
	std::string line;

	// Read header
	std::getline(is, line);
	// mc_dbgmsg( line )

	// Read control point history
	ControlPointHistory cph(-1,-1);
	while(is)
	{
		std::getline(is, line);
		if(is)
		{
			std::stringstream iss(line);
			pch->_contour_history.push_back(cph);
			ControlPointHistory& most_recent = pch->_contour_history.back();
			iss >> &most_recent;

			//mc_dbgmsg( pch->_contour_history.back() )
		}
	}
	return is;
}

}


