/*
 * VisualObject.h
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_PERCEPTION_VISUALOBJECT_H_
#define INCLUDE_PERCEPTION_VISUALOBJECT_H_

#include "perception/Shape.h"
#include "perception/Tracker.h"
#include "perception/VisualHistoryKeeper.h"

namespace Perception {

class VisualObject
{
private:
	std::string label;
	bool movable;
	std::string type;   // solid, deformable or constraint
	Shape *shape;

	std::string _trackerParams;
	Tracker* _tracker;
	int setTracker(cv::Mat &img);

	bool _captureHistory;
	HistoryKeeper* _historyKeeper;

public:
	VisualObject(bool keep = false);
	void captureHistory();
	const std::string getLabel();
	bool isMovable();
	const std::string getType();
	const Shape* getShape();
	void draw(cv::Mat &img, int thickness = 1);
	/**
	 * Looks for itself in the new image.
	 * The tracking method depends on the type of the object.
	 * Returns -1 if it couldn't find itself.
	 */
	int track(cv::Mat &img);
	void writeHistory(const char* hFile);
	~VisualObject();
	friend std::ostream& operator << (std::ostream &o, const VisualObject& obj);
	friend std::istream& operator >> (std::istream &i, VisualObject& obj);
};
std::ostream& operator << (std::ostream &o, const VisualObject& obj);
std::istream& operator >> (std::istream &i, VisualObject& obj);

}



#endif /* INCLUDE_PERCEPTION_VISUALOBJECT_H_ */
