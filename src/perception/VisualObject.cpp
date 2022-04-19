/*
 * VisualObject.cpp
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#include "perception/VisualObject.h"
#include <iostream>
#include <fstream>

namespace Perception{
	std::ostream& operator << (std::ostream &o, const VisualObject& obj){
		o << obj.label << "\t" << obj.movable << "\t" << obj.type << "\t";
		if(obj.shape){
			o << *(obj.shape);
		}else{
			o << "[]";
		}
		return o;
	}
	std::istream& operator >> (std::istream &i, VisualObject& obj){
		std::string str;
		i >> str;			// Label
		obj.label = str;
		i >> obj.movable;	// Movable
		i >> str;			// Type
		obj.type = str;
		if(obj.shape != 0){
			delete obj.shape;
			obj.shape = 0;
		}
		i >> &(obj.shape);
		char trackerParams[256];
		i.getline(trackerParams, 256);
		obj._trackerParams.assign(trackerParams);
		std::cout << "TP:" <<  obj._trackerParams << std::endl;
		return i;
	}

	VisualObject::VisualObject(bool keep) : movable(false), shape(0), _tracker(0), _captureHistory(keep), _historyKeeper(NULL) {}

	void VisualObject::captureHistory()
	{
		if (type.compare("deformable") == 0 && shape->getShapeType() == INTPOLYGON)
		{
			_captureHistory = true;
		}
		else
		{
			std::cerr << "Keeping history of " << type << " " << shape->getShapeType() << " is not supported." << std::endl;
		}
	}

	const std::string VisualObject::getLabel() { return label; }
	bool VisualObject::isMovable() { return movable; }
	const std::string VisualObject::getType() { return type; }
	const Shape* VisualObject::getShape() { return shape; }
	VisualObject::~VisualObject()
	{
		if(shape)
		{
			delete shape;
			shape = 0;
		}
		if(_tracker)
		{
			delete _tracker;
			_tracker = 0;
		}
		if(_historyKeeper)
		{
			delete _historyKeeper;
			_historyKeeper = 0;
		}
	}

	void VisualObject::draw(cv::Mat &img, int thickness)
	{
		if(shape){
			cv::Scalar colour = cv::Scalar(250, 250, 250);
			if(type.compare("solid") == 0){
				colour = cv::Scalar(0, 250, 0);
			}else if(type.compare("deformable") == 0){
				colour = cv::Scalar(250, 250, 0);
			}
			shape->draw(img, colour, thickness);
		}
    }

	int VisualObject::track(cv::Mat &img)
	{
		if(movable){
			if(_tracker == 0){
				if(setTracker(img) < 0) return -1;
				if(_captureHistory)
				{
					_historyKeeper = new ContourHistory(dynamic_cast<IntPolygon*>(shape));
				}
			}
			if(_captureHistory)
			{
				return _tracker->track(img, shape, _historyKeeper);
			}
			else
			{
				return _tracker->track(img, shape);
			}
		}else{
			// It doesn't move, so we will rely that it remains there.
			return 0;
		}
	}

	int VisualObject::setTracker(cv::Mat &img)
	{
		if(_tracker != 0) {
			delete _tracker;
			_tracker = 0;
		}
		if(type.compare("solid") == 0 || type.compare("constraint") == 0){
			if(shape->getShapeType() == CIRCLE){
				_tracker = new HueCircleTracker(dynamic_cast<Circle*>(shape), img);
			}else{
				// TODO
				_tracker = new StubTracker();
			}
			// TODO
			return 0;
		}else if(type.compare("deformable") == 0){
			if(shape->getShapeType() == INTPOLYGON){
				////_tracker = new HueDeformableTracker(dynamic_cast<IntPolygon*>(shape), img);
				//_tracker = new HueSatDeformableTracker(dynamic_cast<IntPolygon*>(shape), img);
				_tracker = new CannyDeformableTracker(dynamic_cast<IntPolygon*>(shape), img, _trackerParams);
			}else{
				// TODO
				_tracker = new StubTracker();
			}
			return 0;
		}
		_tracker = new StubTracker();
		return -1; // Object with unknown type
	}

	void VisualObject::writeHistory(const char* hFile)
	{
		if (_historyKeeper)
		{
			std::ofstream sfile;
			sfile.open(hFile);
			_historyKeeper->writeHeaders(sfile);
			_historyKeeper->write(sfile);
			sfile.close();
		}
		else
		{
			std::cerr << "No history was captured to save in " << hFile << std::endl;
		}
	}
}



