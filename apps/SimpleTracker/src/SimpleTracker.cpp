/*
 * SimpleTracker.cpp
 *
 *  Created on: 27/06/2017
 *      Author: blackzafiro
 */

#include "perception/SyncSensorsReader.h"

using namespace Perception;
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    if(argc < 4)
    {
    	cout << "Usage: " << argv[0] << " <video_file_name_root> <material_to_keep> <history_file_name>" << endl;
    	return -1;
    }

    SyncSensorsReader dr;
    if(dr.init(argv[1]) < 0) return -1;

    cv::Mat src(dr.getImageDimensions(), CV_8UC3);
    cout << "Video size: " << dr.getImageDimensions().width << ","
                           << dr.getImageDimensions().height << endl;
    cv::Mat small(Size(644, 482), CV_8UC3);

    VideoFrame& vf = dr.getVideoFrame(); // Watch out! This reference is valid only while the SyncSensorsReader exists.
    if(argc >= 3)
    {
    	std::cout << "Will capture history of " << argv[2] << std::endl;
    	vf.getVObjects()[argv[2]]->captureHistory();
    }
    int videoFrameNumber;
    bool broken = false;
    while((videoFrameNumber = dr.readData()) != -1)
    {
        vf.draw(src);
        //dr.getVideoFrame().draw(src);	// Equivalent but with more function calls.
        cv::resize(src, small, cv::Size(644, 482));
        cv::imshow("Image", small);
        cout << videoFrameNumber << ", " << dr.getTimestamp() << ": " << dr.getForce() << endl;

        if(cv::waitKey(1) == 'q') { broken = true; break; }
    }
    if(!broken)
    {
    	cv::waitKey(-2);
    }
    dr.close();
    if(argc >= 4)
	{
    	std::cout << "Writing history in " << argv[3] << std::endl;
		vf.getVObjects()[argv[2]]->writeHistory(argv[3]);
	}
    return 0;
}
