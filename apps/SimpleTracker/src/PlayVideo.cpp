/*
 * PlayVideo.cpp
 *
 *  Created on: 27/06/2017
 *      Author: blackzafiro
 */

#include "perception/VideoFileReader.h"
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace Perception;
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	if(argc < 2){
		cout << "Usage: " << argv[0] << " <video_file_name_root>" << endl;
		return -1;
	}

	// Reproduce video
	Mat src, img;
	Mat vFrame;

	VideoFileReader vr;
	string fname(argv[1]);
	string vname = fname + ".avi";
	string cname = fname + ".to";
	if(vr.init(vname.c_str(), cname.c_str()) < 0) return -1;
		src.create(vr.getDimensions(), CV_8UC3);
		vFrame.create(vr.getDimensions(), CV_8UC3);

		int frame = -1;
		while((frame = vr.readVideoFrame()) >= 0){
			//std::cout << "Play 1, frame " << frame << std::endl;
			vr.drawFrame(src);
			//cv::resize(src, img, cv::Size(644, 482));
			cv::resize(src, img, Size(), 0.25, 0.25);
			cv::imshow("Source", img);

			vr.getVideoFrame().draw(vFrame);
			//cv::resize(vFrame, img, Size(), 0.75, 0.75);
			cv::resize(vFrame, img, Size(), 1.0, 1.0);
			//cvMoveWindow("Processed",720,0);
			CVShortcuts::show("Processed", img, 0.7); //cv::imshow("Processed", img);
			//cv::setMouseCallback("Processed", VAGUI::onMouseShowXY, 0);

			if((char)cv::waitKey(2) == 'q') { vr.close(); return 0; }
			//char c = cv::waitKey(-2); if(c == 'q') break;

			/*
			if(c == 's'){
				std::cout << c << "s=" << int('s') << std::endl;
				cv::imwrite("results/processed.png", img);
			}
			*/
		}
		//cv::waitKey(-2);
		vr.close();

		/*
		Mat src2, img2;
		Mat vFrame2;
		VideoFileReader vr2;
		if(vr2.init(vname.c_str(), cname.c_str()) < 0) return -1;
		src2.create(vr2.getDimensions(), CV_8UC3);
		vFrame2.create(vr2.getDimensions(), CV_8UC3);

		frame = -1;
		while((frame = vr2.readVideoFrame()) >= 0){
			//std::cout << "Play 2, frame " << frame << std::endl;
			vr2.drawFrame(src2);
			cv::resize(src2, img2, Size(), 0.25, 0.25);
			cv::imshow("Source", img2);

			vr2.getVideoFrame().draw(vFrame2);
			cv::resize(vFrame2, img2, Size(), 1.0, 1.0);
			CVShortcuts::show("Processed2", img2, 0.7);

			if((char)cv::waitKey(2) != -1) { vr2.close(); return 0; }
			//char c = cv::waitKey(-2); if(c == 'q') break;
		}
		vr2.close();
		 */

		return 0;

}



