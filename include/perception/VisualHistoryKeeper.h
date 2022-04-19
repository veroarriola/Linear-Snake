/*
 * VisualHistoryTracker.h
 *
 *  Created on: 28/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_PERCEPTION_VISUALHISTORYKEEPER_H_
#define INCLUDE_PERCEPTION_VISUALHISTORYKEEPER_H_

//#include <vector>
#include "perception/Shape.h"

namespace Perception {

class ControlPoint{
private:
	int _x;
	int _y;
	int _prev_neighbour_index;
	int _next_neighbour_index;

public:
	ControlPoint();
	ControlPoint(int x, int y, int prev_neighbour_index, int next_neighbour_index);
	ControlPoint(const ControlPoint& other);

	ControlPoint& operator=(const ControlPoint& other);

	void set(int x, int y, int prev_neighbour_index, int next_neighbour_index);
	void set(int x, int y);

	int x();
	int y();
	int prev_neighbour_index();
	int next_neighbour_index();
	void prev_neighbour_index(int prev);
	void next_neighbour_index(int next);

	std::ostream& write(std::ostream &o) const;

	~ControlPoint();
};

/** Array of control point historic values. */
typedef std::vector<ControlPoint> CPHistory;
class ControlPointHistory{
private :
	int _ident;
	int _birth_time;
	int _death_time;

	CPHistory _control_point_history;	// Pointer to array of control points

public:

	ControlPointHistory(int ident, int birth_time);

	/** Returns time of birth of control point. */
	int birthTime() const;

	/** Returns time of death of control point. */
	int deathTime() const;

	/** Returns control point at given time or throws an exception
	 * if this point was not alive at that time.
	 */
	ControlPoint& getControlPoint(int time);

	void queue(ControlPoint &cpoint);

	/**
	 * Appends a new control point with the same neighbours than in its previous
	 * state.
	 */
	void update(int x, int y);

	/**
	 * Returns a reference to the last state of the point in history.
	 */
	ControlPoint& back();

	/**
	 * Returns a copy of the last control point.
	 */
	//ControlPoint last();

	bool isAlive();

	/**
	 * Removes last values of control point and sets it's
	 * death time to the current time.
	 */
	void remove();

	~ControlPointHistory();

	friend std::istream& operator >> (std::istream &is, ControlPointHistory *pcph);
	friend std::ostream& operator << (std::ostream &o, const ControlPointHistory& s);
};
std::istream& operator >> (std::istream &is, ControlPointHistory *pcph);
std::ostream& operator << (std::ostream &o, const ControlPointHistory& s);

/**
 * Abstract class to keep history of tracking coordinates.
 */
class HistoryKeeper
{
public:
	/**
	 * Meant to do nothing.
	 */
	HistoryKeeper();
	virtual ~HistoryKeeper();
public:
	virtual std::ostream& writeHeaders(std::ostream &o) const = 0;
	virtual std::ostream& write(std::ostream &o) const = 0;
};

/**
 * Class in charge of creating the history for the whole polynomial.
 */
class IntPolygon;
extern const int UNDEAD;
typedef std::vector<ControlPointHistory> CHistory;
class ContourHistory : public HistoryKeeper {
private:
	CHistory _contour_history;	// Array of pointers to created histories.

	/**
	 * Index of current positions of each control point in the _contour_history
	 * for every point in the cv polygon.
	 * <polygon_index> -> <index_in_contour_history>.
	 * It has the same length than the last polygon received.
	 * It is used only while registering history.
	 */
	std::vector<int> _cp_polygon_index;
	unsigned int _last_poly_length;

	int _time_step;

public:

	/**
	 * Constructs an empty history.
	 */
	ContourHistory();
	/**
	 * Constructs list of control point histories starting with poly.
	 */
	ContourHistory(IntPolygon* poly);
	std::ostream& writeHeaders(std::ostream &o) const;
	std::ostream& write(std::ostream &o) const;

	/**
	 * Adds a new time layer to the history, assuming the number and position of
	 * the control points has not changed since the previous time step.
	 */
	void updatePositions(IntPolygon* poly);

	/**
	 * Creates a new control point with its history, at position
	 * poly_index in the current polynomial.
	 */
	void createControlPointHistory(int poly_index, int x, int y, unsigned int length);

	/**
	 * Registers the removal of a control point in history.
	 * @param length is the current length of the contour.
	 */
	void removeControlPoint(int poly_index);

	/**
	 * Reconstructs the contour at time <code>t</code>.
	 * It will clear any points already in <code>poly</code>.
	 */
	void reconstructContour(IntPolygon& poly, int time);

	~ContourHistory();

	friend std::istream& operator >> (std::istream &is, ContourHistory *pch);
};
std::istream& operator >> (std::istream &is, ContourHistory *pch);

}

#endif /* INCLUDE_PERCEPTION_VISUALHISTORYKEEPER_H_ */
