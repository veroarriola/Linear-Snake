/*
 * ReplayHistory.cpp
 *
 *  Created on: 13/07/2017
 *      Author: blackzafiro
 *
 * Receives a video bundle path and a history file to replay the tracking history over the original video.
 */

#include "perception/VideoFileReader.h"

using namespace Perception;
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    if(argc < 3){
    	cout << "Usage: " << argv[0] << " <video_file_name_root> <history_file>" << endl;
    	return -1;
    }

    // Load history
    ContourHistory chist;
    std::ifstream is (argv[2], std::ifstream::binary);
	if (is)
	{
		is >> &chist;
	}
	else
	{
		std::cerr << "Failed to open file " << argv[2] << std::endl;
	}

    // Reproduce video
	Mat src;
	Mat vFrame;

	VideoFileReader vr;
	string fname(argv[1]);
	string vname = fname + ".avi";
	string cname = fname + ".to";

	if(vr.init(vname.c_str(), cname.c_str()) < 0) return -1;
	src.create(vr.getDimensions(), CV_8UC3);
	vFrame.create(vr.getDimensions(), CV_8UC3);

	cv::namedWindow("Processed");
	cvMoveWindow("Processed", 350, 0);

	int frame = -1;
	int time = 0;
	bool broken = false;
	IntPolygon reconstructed_contour;
	while((frame = vr.readVideoFrame()) >= 0){
		std::cout << "Play 1 frame " << frame << std::endl;
		chist.reconstructContour(reconstructed_contour, time + 1); // TODO: Initial contour is not displayed
		vr.drawFrame(src);
		CVShortcuts::show("Source", src, 0.25);

		vr.getVideoFrame().draw(vFrame);
		reconstructed_contour.draw(vFrame, 0, 1);

		CVShortcuts::show("Processed", vFrame, 1.0);
		//cv::setMouseCallback("Processed", VAGUI::onMouseShowXY, 0);

		if(cv::waitKey(-2) == 'q') { broken = true; break; }
		time++;
	}
	if(!broken)
	{
		cv::waitKey(-2);
	}
	vr.close();

    return 0;
}


