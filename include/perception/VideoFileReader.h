/*
 * VideoFileReader.h
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_PERCEPTION_VIDEOFILEREADER_H_
#define INCLUDE_PERCEPTION_VIDEOFILEREADER_H_

#include "perception/VisualObject.h"
#include <opencv2/videoio.hpp>
#include <iomanip>
#include <fstream>
#include <map>

namespace Perception {

typedef std::map<std::string, VisualObject*> VisualObjectsMap;
/**
 * Class that contains a list of visual objects to be tracked.
 * It's track method is called by the VideoFileReader after each video frame
 * is read.
 */
class VideoFrame{
private:
	cv::Mat image;
	VisualObjectsMap vobjects; // There can only be one object with one shape, not even temporal copies.

public:
	VideoFrame();

	int init(const char* calFile);
	VisualObjectsMap& getVObjects() { return vobjects;}
	void track(cv::Mat &video);
	void draw(cv::Mat &img);
	~VideoFrame();
};

class VideoFileReader
{
private:
	cv::VideoCapture video_cap;
	cv::Mat video;
	VideoFrame vFrame;
	bool isOpen;
	int frame_number;
public:
	VideoFileReader();
	int init(const char* videoFile, const char* calFile);
	/**
	 * @returns The frame number or -1 if it finished reading or the file
	 *          is not open.
	 */
	int readVideoFrame();
	VideoFrame& getVideoFrame();
	const cv::Size getDimensions();
	void drawFrame(cv::Mat &img);
	void close();
};

}




#endif /* INCLUDE_PERCEPTION_VIDEOFILEREADER_H_ */
