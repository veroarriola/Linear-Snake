/*
 * VideoFileReader.cpp
 *
 *  Created on: 27/06/2017
 *      Author: blackzafiro
 */

#include "perception/VideoFileReader.h"

#define mc_dbgmsg(msg)	std::cout << __FILE__ << ":" <<  __FUNCTION__ << ":" << __LINE__ << std::endl << "\e[1;35m" << msg << "\e[0m" << std::endl;

namespace Perception {

	///
	/// VIDEO FRAME
	///

	VideoFrame::VideoFrame() {}

	int VideoFrame::init(const char* calFile)
	{
		std::ifstream cFile;
		std::string line;
		cFile.open(calFile);
		if (cFile.is_open()){
			char c;
			while(!cFile.eof() && (c = cFile.peek()) > -1){
				if(c == '#') {
					std::getline(cFile, line); // Comment line
					std::cout << "Comment: " << line << std::endl;
				}else if(c == '\n' || c == '\0'){
					//std::cout << "char:" << c << std::endl;
					std::getline(cFile, line); // Comment line
				}else{
					VisualObject *obj = new VisualObject();  // Watch out! Each vobj must have its own shape.  I would need to redefine 'clone';
					cFile >> *obj;
					vobjects[obj->getLabel()] = obj;
					std::cout << "obj: " << *obj << std::endl;
					//std::getline(cFile, line);
				}
			}
			std::cout << "Tracking: " << vobjects.size() << " objects..." << std::endl;
			cFile.close();
		}
		return 0;
	}

	void VideoFrame::track(cv::Mat &video)
	{
		image = video;
		for(VisualObjectsMap::iterator it = vobjects.begin();
				it != vobjects.end(); ++it){
			it->second->track(image);
		}
	}

	void VideoFrame::draw(cv::Mat &img)
	{
		image.copyTo(img);
		for(VisualObjectsMap::iterator it = vobjects.begin();
				it != vobjects.end(); ++it){
			it->second->draw(img);
		}
	}

	VideoFrame::~VideoFrame()
	{
		for(VisualObjectsMap::iterator it = vobjects.begin();
				it != vobjects.end(); ++it){
			delete it->second;
			vobjects.erase(it);
		}
	}

	///
	/// VIDEO FILE READER
	///

	VideoFileReader::VideoFileReader() : isOpen(false), frame_number(-1) {}

	int VideoFileReader::init(const char* videoFile, const char* calFile){
		// Open video file;
		if(video_cap.open(videoFile))
		{
			vFrame.init(calFile);
			isOpen = true;
			return 0;
		}
		return -1;
	}

	/**
	 * @returns The frame number or -1 if it finished reading or the file
	 *          is not open.
	 */
	int VideoFileReader::readVideoFrame(){
		if(isOpen){
			if(video_cap.read(video))
			{
				vFrame.track(video);

				++frame_number;
				//mc_dbgmsg( "Read frame " << frame_number )
				return frame_number;
			}else return -1;
		}else return -1;
	}

	VideoFrame& VideoFileReader::getVideoFrame(){
		return vFrame;
	}

	const cv::Size VideoFileReader::getDimensions(){
		return cv::Size(video_cap.get(cv::CAP_PROP_FRAME_WIDTH),
				        video_cap.get(cv::CAP_PROP_FRAME_HEIGHT));
	}

	void VideoFileReader::drawFrame(cv::Mat &img){
		img = video.clone();
	}

	void VideoFileReader::close(){
		if(isOpen){
			video_cap.release();
		}
	}
}
