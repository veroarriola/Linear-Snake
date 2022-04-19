/*
 * SyncSensorsReader.cpp
 *
 *  Created on: 27/06/2017
 *      Author: blackzafiro
 */

#include "perception/SyncSensorsReader.h"

namespace Perception {

SyncSensorsReader::SyncSensorsReader(int frames) : frames(frames), frameNumber(frames) { }

int SyncSensorsReader::init(std::string filesNameBase){
	std::string sname = filesNameBase + ".txt";	// Force Sensor Data
	std::string vname = filesNameBase + ".avi";	// Video
	std::string cname = filesNameBase + ".to";	// Objects to be Tracked

	return init(sname.c_str(), vname.c_str(), cname.c_str());
}

int SyncSensorsReader::init(const char* sensorFile,
	const char* videoFile,
	const char* trackFile){
	if(file_sens.init(sensorFile) < 0){
		std::cerr << "Error opening file: " << sensorFile << std::endl;
		return -1;
	}
	if(file_video.init(videoFile, trackFile) < 0){
		std::cerr << "Error opening file: " << videoFile << std::endl;
		std::cerr << "Or error opening file: " << trackFile << std::endl;
		return -2;
	}

	/**
	 * Old code in case I want to skip some frames.
	 */
	/*
	for(int i = 0; i < 1; i++){
	//for(int i = 0; i < 65; i++){ // In the first frame the finger is too far.
	//	if(file_video.readVideoFrame() < 0) return -3; // First image is not used for evaluation
		//file_sens.readLine();      // The force sensor reads two lines for the first step
	}*/

	return 0;
}

const cv::Size SyncSensorsReader::getImageDimensions(){
	return file_video.getDimensions();
}

int SyncSensorsReader::readData(){
	if((frameNumber = file_video.readVideoFrame()) >= 0 && (frames != 0) && (file_sens.readLine() >=0)){
		frames--;
		return frameNumber;
	}else{
		return -1;
	}
}

SensedForce SyncSensorsReader::getForce(){
	return file_sens.getForce();
}

float SyncSensorsReader::getTimestamp(){
	return file_sens.getTimestamp();
}

SensedForce SyncSensorsReader::getPrevForce(){
	return file_sens.getPrevForce();
}

float SyncSensorsReader::getPrevTimestamp(){
	return file_sens.getPrevTimestamp();
}

VideoFrame& SyncSensorsReader::getVideoFrame(){
	return file_video.getVideoFrame();
}

int SyncSensorsReader::getFrameNumber() { return frameNumber; }

void SyncSensorsReader::close(){
	file_video.close();
	file_sens.close();
}

}
