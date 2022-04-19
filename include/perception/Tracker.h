/*
 * Tracker.h
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 *
 *  With tracking ideas from Rustam Stolkin
 */

#ifndef INCLUDE_PERCEPTION_TRACKER_H_
#define INCLUDE_PERCEPTION_TRACKER_H_

#include "Shape.h"
#include "VisualHistoryKeeper.h"

namespace Perception{

class Tracker{
public:
	virtual int track(cv::Mat &img, Shape *s, HistoryKeeper* hk = NULL) = 0;
	virtual ~Tracker();
};

class StubTracker : public Tracker {
public:
	int track(cv::Mat &img, Shape *s, HistoryKeeper* hk = NULL);
};

class HueCircleTracker : public Tracker {
private:
	int bin[2];
public:
	HueCircleTracker(Circle *c, cv::Mat &img, int sliceSize = 18);
	int track(cv::Mat &img, Shape *s, HistoryKeeper* hk = NULL);
};

class HueDeformableTracker : public Tracker {
private:
	int bin[2];
	int sliceSize;
public:
	HueDeformableTracker(IntPolygon *p, cv::Mat &img, int sliceSize = 15);
	int track(cv::Mat &img, Shape *s, HistoryKeeper* hk = NULL);
};

const int HUENUMVALS = 180;
const int SATNUMVALS = 256;
class HueSatDeformableTracker : public Tracker {
private:
	int hbin[2];
	int sbin[2];
	int hSliceSize;
	int sSliceSize;
public:
	HueSatDeformableTracker(IntPolygon *p, cv::Mat &img, int hSliceSize = 15, int sSliceSize = 2);
	int track(cv::Mat &img, Shape *s, HistoryKeeper* hk = NULL);
};

typedef std::vector< std::vector<cv::Point> > Contours;
class CannyDeformableTracker : public Tracker{
private:
	int edgeThresh;
	int ratio;
	int kernel_size;
	int blur_kernel_size;

	int lowThreshold;
	int const max_lowThreshold;

	// Hue cleaning
	int hbin[2];
	int hSlices; // Should divide 180

	// Snake elements
	int maxDistance, minDistance, minAngle;
	int rings; // The maximum distance to look for an edge

	int setAndParse(std::string &params);
public:
	//CannyDeformableTracker(IntPolygon *p, cv::Mat &img, int hSlices = 9);
	CannyDeformableTracker(IntPolygon *p, cv::Mat &img, std::string &params);
	int track(cv::Mat &img, Shape *s, HistoryKeeper* hk = NULL);
};
}



#endif /* INCLUDE_PERCEPTION_TRACKER_H_ */
