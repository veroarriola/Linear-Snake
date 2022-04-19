/*
 * RingIterator.h
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#ifndef INCLUDE_CVSHORTCUTS_RINGITERATOR_H_
#define INCLUDE_CVSHORTCUTS_RINGITERATOR_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace CVShortcuts {

class RingIterator {
private:
	enum Direction{
		RIGHT,
		DOWN,
		LEFT,
		UP,
		END
	};

	cv::Point centre;
	int RINGS;
	int cols;
	int rows;
	int ring;
	Direction d;
	int x;
	int y;
	int end;

public:
	/**
	 *
	 * @param centre
	 * @param rings Must be greater than 1.
	 * @param img
	 */
	RingIterator(cv::Point centre, int rings, cv::Mat &img);
	bool nextTrimmed(cv::Point &p);
	bool next(cv::Point &p);
};

}

#endif /* INCLUDE_CVSHORTCUTS_RINGITERATOR_H_ */
