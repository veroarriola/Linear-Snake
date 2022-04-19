/*
 * cvShortcuts.cpp
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#include "cvshortcuts/cvShortcuts.h"
#include <assert.h>

namespace CVShortcuts {

	void drawLine(cv::Mat &img, cv::Point pt1, cv::Point pt2, const cv::Scalar &colour, int type, int thickness, int lineType, int shift){
		if(type == -1) return;
		if (type == 0){
			cv::line(img, pt1, pt2, colour, thickness, lineType, shift);
			return;
		}
		int s1,s2,s3,s4;
		switch(type){
			case 1:
				s1 = 2, s2 = 3, s3 = 2, s4 = 3;
				break;
			case 2:
				s1 = 7, s2 = 4, s3 = 7, s4 = 4;
				break;
			case 3:
				s1 = 7, s2 = 3, s3 = 1, s4 = 3;
				break;
			default:
				s1 = 10, s2 = 5, s3 = 5, s4 = 3;
				break;
		}

		// Draw dashed line
		cv::LineIterator li(img, pt1, pt2);
		cv::Point p1, p2;
		int count;
		for(int i = 0; i < li.count; i++, ++li){
			count = 0;
			p1 = li.pos();
			while(i < li.count - 1 && count < s1) { i++; ++li; count++;}
			p2 = li.pos();
			cv::line(img, p1, p2, colour, 1, lineType);
			if(i == li.count - 1) return;

			count = 0;
			while(i < li.count - 1 && count < s2) { i++; ++li; count++;}
			if(i == li.count - 1) return;

			count = 0;
			p1 = li.pos();
			while(i < li.count - 1 && count < s3) { i++; ++li; count++;}
			p2 = li.pos();
			cv::line(img, p1, p2, colour, 1, lineType);
			if(i == li.count - 1) return;

			count = 0;
			while(i < li.count - 1 && count < s4) { i++; ++li; count++;}
			if(i == li.count - 1) return;
		}
	}

	int squaredDistance(const cv::Point &p1, const cv::Point &p2){
		return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
	}

    void show(const char *window, cv::Mat &img, double scale){
		cv::Mat timg;
		cv::resize(img, timg, cv::Size(), scale, scale);
		cv::imshow(window, timg);
    }

	void showChannels(const char *window, cv::Mat &img, double scale){
		cv::Mat resized;
		cv::resize(img, resized, cv::Size(), scale, scale);
		cv::Mat img3 = cv::Mat::zeros(resized.rows, resized.cols * resized.channels(), resized.type());
		uchar *data = (uchar *)resized.data;
		uchar *data3 = (uchar *)img3.data;
		for(int index, index3, i = 0; i < resized.cols; i++){ // column
			for(int j = 0; j < resized.rows; j++){ // row
				for(int c = 0; c < resized.channels(); c++){
					index = j * resized.step + i * resized.channels() + c;
					index3 = j * img3.step + (i + c * resized.cols) * img3.channels() + c;  // Diff colour for diff channels :)
					data3[index3] = data[index];
				}
			}
		}
		cv::imshow(window, img3);
	}

	void minMaxLoc(const std::vector<cv::Point> &points, cv::Point &min, cv::Point &max){
		min.x = INT_MAX; min.y = INT_MAX;
		max.x = 0; max.y = 0;
		for(std::vector<cv::Point>::const_iterator it = points.begin();
				it < points.end(); ++it){
			if(it->x < min.x) { min.x = it->x; }
			if(it->y < min.y) { min.y = it->y; }
			if(it->x > max.x) { max.x = it->x; }
			if(it->y > max.y) { max.y = it->y; }
		}
	}

	double meanHistogram(cv::Mat &hist, int MAX){
		assert(hist.cols == 1);
		double acc = 0;
		double elements = 0;
		//Assume histogram has doubles;
		float *data = (float *)hist.data;
		int iSize = cvRound(MAX / hist.rows);
		double sliceMean;
		int index;
		for(int j = 0; j < hist.rows; j++){
			index = j * hist.step;
			elements += data[index];
			sliceMean = ((2.0 * j + 1) * iSize) / 2;
			acc += (sliceMean * data[index]);
		}
		return acc / elements;
	}

	void onMouseShowXY(int event, int x, int y, int, void*){
		cv::Mat img = cv::Mat::zeros(100, 300, CV_8UC3);
		std::stringstream s;
		s << x << "," << y;
		cv::Point pt = cv::Point(10, 90);
		cv::putText(img, s.str(), pt, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(250, 250, 250));
		cv::imshow("Coordinates", img);
	}
}

