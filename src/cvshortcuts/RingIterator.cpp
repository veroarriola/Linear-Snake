/*
 * RingIterator.cpp
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#include "cvshortcuts/RingIterator.h"
#include "Exception.h"

namespace CVShortcuts {

IllegalStateException illegalStateException;

RingIterator::RingIterator(cv::Point centre, int rings, cv::Mat &img) :
		centre(centre),
		RINGS(rings), cols(img.cols), rows(img.rows),
		ring(1), d(RIGHT)
{
	x = centre.x - ring;
	y = centre.y - ring;
	end = centre.x + ring;
}

bool RingIterator::nextTrimmed(cv::Point &p)
{
	do{
		if(!next(p)) return false;
	}while(p.x >= cols || p.y >= rows || p.x < 0 || p.y < 0);
	return true;
}

bool RingIterator::next(cv::Point &p)
{
	//std::cout << x << "," << y << " end:" << end << " direction: " << d << std::endl;
	p.x = x; p.y = y;
	switch (d){
		case RIGHT:
			x++;
			if(x == end){
				d = DOWN;
				end = centre.y + ring;
			}
			return true;
		case DOWN:
			y++;
			if(y == end){
				d = LEFT;
				end = centre.x - ring;
			}
			return true;
		case LEFT:
			x--;
			if(x == end){
				d = UP;
				end = centre.y - ring;
			}
			return true;
		case UP:
			y--;
			if(y == end){
				ring++;
				if(ring > RINGS){
					d = END;
				}else{
					d = RIGHT;
					end = centre.x + ring;
				}
				x = centre.x - ring;
				y = centre.y - ring;
			}
			return true;
		case END:
			return false;
	}
	throw illegalStateException;
}

}
