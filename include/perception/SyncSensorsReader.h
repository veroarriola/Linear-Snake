/*
 * SyncSensorsReader.hpp
 *
 *  Created on: 27/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_PERCEPTION_SYNCSENSORSREADER_H_
#define INCLUDE_PERCEPTION_SYNCSENSORSREADER_H_

#include "perception/VideoFileReader.h"
#include "perception/ForceFileReader.h"
#include <math.h>

namespace Perception {

class SyncSensorsReader
{
private:
	SensorFileReader file_sens;
	VideoFileReader file_video;
	int frames;
	int frameNumber;

public:
	/**
	 * @param frames to be read, it can be used to limit the number of frames to be read.
	 *        If it is negative, all frames are read.  If it is greater than the available
	 *        frames it keeps returning the same image.
	 */
	SyncSensorsReader(int frames = -1);

	/**
	 * Initializes streams, requires the common base for the name of the following files:
	 * txt: Force sensor data.
	 * avi:	Video file.
	 * to:	Parameters for the objects to be tracked.
	 */
	int init(std::string filesNameBase);

	int init(const char* sensorFile,
			  const char* videoFile,
			  const char* trackFile);

	const cv::Size getImageDimensions();

	/**
	 * Reads video frame, without reading the force line.
	 * Used to synchronise with force reader, since this reads two lines at the beginning.
	 * @return
	 */
	/*
	int readVideoFrame(){
		return file_video.readVideoFrame();
	}*/

	/** Returns the frame number or -1 is the file is over */
	int readData();

	// Getters
	SensedForce getForce();
	float getTimestamp();
	SensedForce getPrevForce();
	float getPrevTimestamp();
	VideoFrame& getVideoFrame();
	int getFrameNumber();

	/**
	 * Close underlying streams.
	 */
	void close();
};

}

#endif /* INCLUDE_PERCEPTION_SYNCSENSORSREADER_H_ */
