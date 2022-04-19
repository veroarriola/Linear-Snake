/*
 * Tracker.cpp
 *
 *  Created on: 26/06/2017
 *      Author: blackzafiro
 */

#include "perception/Tracker.h"
#include "cvshortcuts/RingIterator.h"
#include <sstream>
#include <fstream>

#include "exceptions/Exceptions.h"
NotSupportedException nsexception("History keeping option");


#define mc_dbgmsg(msg)	std::cout << __FILE__ << ":" <<  __FUNCTION__ << ":" << __LINE__ << " \e[1;35m" << msg << "\e[0m" << std::endl;


namespace Perception{

///
/// ABSTRACT TRACKER
///

Tracker::~Tracker() {}

///
/// STUB TRACKER
///

int StubTracker::track(cv::Mat &img, Shape *s, HistoryKeeper* hk) { return 0; }

///
/// HUE BASED TRACKER
///

void selectHueSlice(int *bin, Shape *s, cv::Mat &src, int nslices)
{
	int HBINS = nslices;

	// Mask on the circle
	cv::Mat mask = cv::Mat(src.size(), CV_8UC1);
	s->draw(mask, cv::Scalar::all(255), -1);
	// Find hue interval
	cv::Mat hsv = cv::Mat(src.size(), CV_8UC3);
	cv::cvtColor(src, hsv, CV_BGR2HSV);

	int histSize[] = {HBINS};
	float hranges[] =	{0, HUENUMVALS};	// H values range from 0 to 179
	const float* ranges[] = {hranges};
	cv::SparseMat histogram;
	int channels[] = {0};					// H is channel 0
	cv::calcHist( &hsv, 1, channels, mask,	// do not use mask
		 histogram, 1, histSize, ranges,
		 true,								// the histogram is uniform
		 false );
	double maxVal=0;
	int maxIdx=-1;
	cv::minMaxLoc(histogram, 0, &maxVal, 0, &maxIdx);
	bin[0] = maxIdx * cvRound(hranges[1] / HBINS);
	bin[1] = bin[0] + cvRound(hranges[1] / HBINS);
}

///
/// SOLID CIRCLE'S TRACKER
///

HueCircleTracker::HueCircleTracker(Circle *c, cv::Mat &img, int sliceSize)
{
	/// Create histogram for circle at first frame.

	selectHueSlice(bin, c, img, sliceSize);

	std::cout << "Hue  circle tracker init: bin={" << bin[0] << "," << bin[1] << "}, sliceSize=" <<
			sliceSize << ", centre=" << c->getCentre() << " source size=" << img.size() << std::endl;
	bin[0] = 0; bin[1] = 10;
}

int HueCircleTracker::track(cv::Mat &img, Shape *s, HistoryKeeper* hk)
{
	if (hk != NULL)
	{
		throw nsexception;
	}
	Circle *c = dynamic_cast<Circle*>(s);

	// Region where to look for the circle.
	// FOR A MORE GENERIC REPRESENTATION SHOULD CHECK THE BOUNDARIES OF THE IMAGE AS WELL
	int x1 = c->getCentre().x() - 2 * (int)(c->getRadius()); if(x1 < 0) x1 = 0;
	int x2 = c->getCentre().x() + 2 * (int)(c->getRadius()); if(x2 >= img.cols) x2 = img.cols - 1; if(x2 < 0) x2 = 0;
	int y1 = c->getCentre().y() - 2 * (int)(c->getRadius()); if(y1 < 0) y1 = 0;
	int y2 = c->getCentre().y() + 2 * (int)(c->getRadius()); if(y2 >= img.rows) y2 = img.rows - 1; if(y2 < 0) y2 = 0;

	// HSV space
	cv::Mat hsv = cv::Mat(img.size(), CV_8UC3);
	cv::cvtColor(img, hsv, CV_BGR2HSV);

	//uchar *data = (uchar *)img.data;
	uchar *hsvData = (uchar *)hsv.data;
	int x = 0;
	int count = 0;
	int y = 0;
	for(int index, i = x1; i < x2; i++){ // column
		for(int j = y1; j < y2; j++){ // row
			index = j * img.step + i * img.channels(); // first channel
			if(hsvData[index] > bin[0] && hsvData[index] < bin[1]){
				x += i;
				y += j;
				count++;
				//data[index] = 255;
			}
		}
	}
	c->setCentre(Geometry::Point2D(x/count, y/count));
	/*{
		static int line = 0;
		std::ofstream myfile;
		myfile.open ("Sponge - Finger position", std::ios::app);
		myfile << line << " " << x/count << "," << y/count << std::endl;
		std::cout << line << " " << x/count << "," << y/count << std::endl;
		myfile.close();
		line++;
	}*/
	return 0;
}



///
/// TRACKER FOR DEFORMABLE POLYGONS
///

///
/// HUE
///

HueDeformableTracker::HueDeformableTracker(IntPolygon *p, cv::Mat &img, int sliceSize)
: sliceSize(sliceSize) {
	/// Create histogram for circle at first frame.
	selectHueSlice(bin, p, img, sliceSize);
}

int HueDeformableTracker::track(cv::Mat &img, Shape *s, HistoryKeeper* hk)
{
	if (hk != NULL)
	{
		throw nsexception;
	}
	// Region where to look for the polygon.
	IntPolygon *p = dynamic_cast<IntPolygon*>(s);
	int x1, x2, y1, y2;
	p->calculateMark(img, x1, x2, y1, y2);

	// HSV space
	cv::Mat hsv = cv::Mat(img.size(), CV_8UC3);
	cv::cvtColor(img, hsv, CV_BGR2HSV);
	cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);

	//uchar *data = (uchar *)img.data;
	uchar *hsvData = (uchar *)hsv.data;
	uchar *maskData = (uchar *)mask.data;
	int x = 0;
	int y = 0;
	for(int index, i = x1; i < x2; i++){ // column
		for(int j = y1; j < y2; j++){ // row
			index = j * img.step + i * img.channels(); // first channel
			if(hsvData[index] > bin[0] && hsvData[index] < bin[1]){
				x += i;
				y += j;
				maskData[j * mask.step + i] = 255;
			}
		}
	}
	//VAGUI::show("HSV", mask, 0.50);
	std::vector< std::vector<cv::Point> > contours;
	cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//cv::drawContours(img, contours, -1, cv::Scalar(250, 0, 0));
	// Assume the longest contour in the region is the tracked object.
	int maxi = -1;
	int maxs = (contours.size() >= 0) ? contours[0].size() : -1;
	for(unsigned int i = 0; i < contours.size(); ++i){
		if((int)(contours[i].size()) > maxs){
			maxi = i;
			maxs = contours[i].size();
		}
	}
	if(maxi >= 0){
		p->assignPoints(contours[maxi]);
	}
	return 0;
}

///
/// HUE SATURATION TRACKER
///

void selectHueSatSlice(int *hbin, int *sbin, Shape *s, cv::Mat &src, int hSliceSize, int sSliceSize){
	// Much from cv.CalcHist documentation
	int HBINS = hSliceSize;
	int SBINS = sSliceSize;

	// Mask on the circle
	cv::Mat mask = cv::Mat(src.size(), CV_8UC1);
	s->draw(mask, cv::Scalar::all(255), -1);
	// Find hue interval
	cv::Mat hsv = cv::Mat(src.size(), CV_8UC3);
	cv::cvtColor(src, hsv, CV_BGR2HSV);

	int histSize[] = {HBINS, SBINS};
	float hranges[] =	{0, HUENUMVALS}; // H values range from 0 to 179
	float sranges[] = {0, SATNUMVALS};
	const float* ranges[] = {hranges, sranges};
	//cv::SparseMat histogram;
	cv::MatND histogram;
	int channels[] = {0, 1}; // H is channel 0
	cv::calcHist( &hsv, 1, channels, mask, // do not use mask
		 histogram, 2, histSize, ranges,
		 true, // the histogram is uniform
		 false );
	double maxVal = 0;
	int maxIdxi = -1;
	int maxIdxj = -1;
	for(int i = 0; i < histogram.size[0]; i++){
		for(int j = 0; j < histogram.size[1]; j++){
			if(histogram.at<float>(i, j) > maxVal){
				maxVal = histogram.at<float>(i, j);
				maxIdxi = i;
				maxIdxj = j;
			}
		}
	}
	hbin[0] = maxIdxi * cvRound(hranges[1] / HBINS);
	hbin[1] = hbin[0] + cvRound(hranges[1] / HBINS);
	sbin[0] = maxIdxj * cvRound(sranges[1] / SBINS);
	sbin[1] = sbin[0] + cvRound(sranges[1] / SBINS);
}

HueSatDeformableTracker::HueSatDeformableTracker(IntPolygon *p, cv::Mat &img, int hSliceSize, int sSliceSize)
: hSliceSize(hSliceSize), sSliceSize(sSliceSize) {
	/// Create histogram for circle at first frame.
	selectHueSatSlice(hbin, sbin, p, img, hSliceSize, sSliceSize);
}

int HueSatDeformableTracker::track(cv::Mat& img, Shape* s, HistoryKeeper* hk)
{
	if (hk != NULL)
	{
		throw nsexception;
	}
	// Region where to look for the polygon.
	IntPolygon *p = dynamic_cast<IntPolygon*>(s);
	int x1, x2, y1, y2;
	p->calculateMark(img, x1, x2, y1, y2);

	// HSV space
	cv::Mat hsv = cv::Mat(img.size(), CV_8UC3);
	cv::cvtColor(img, hsv, CV_BGR2HSV);
	cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1); // Fill the area in another image, not to modify the original

	//uchar *data = (uchar *)img.data;
	uchar *hsvData = (uchar *)hsv.data;
	//uchar *maskData = (uchar *)mask.data;
	int x = 0;
	int y = 0;
	for(int index, i = x1; i < x2; i++){ // column
		for(int j = y1; j < y2; j++){ // row
			index = j * img.step + i * img.channels(); // first channel
			if(hsvData[index] > hbin[0] && hsvData[index] < hbin[1]
				&& hsvData[index + 1] > sbin[0] && hsvData[index + 1] < sbin[1]){
				x += i;
				y += j;
				//maskData[j * mask.step + i] = 255;
			}
		}
	}
	//VAGUI::show("HSV", mask, 0.50);
	std::vector< std::vector<cv::Point> > contours;
	cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//cv::drawContours(img, contours, -1, cv::Scalar(250, 0, 0));
	// Assume the longest contour in the region is the tracked object.
	int maxi = -1;
	int maxs = (contours.size() >= 0) ? contours[0].size() : -1;
	for(unsigned int i = 0; i < contours.size(); ++i){
		if((int)(contours[i].size()) > maxs){
			maxi = i;
			maxs = contours[i].size();
		}
	}
	if(maxi >= 0){
		p->assignPoints(contours[maxi]);
	}
	return 0;
}

///
/// CANNY SNAKE TRACKER
///

void filterH(cv::Mat &dst, Contours &contours, const int *hbin){
	// HSV space
	cv::Mat hsv = cv::Mat(dst.size(), CV_8UC3);
	cv::cvtColor(dst, hsv, CV_BGR2HSV);
	//VAGUI::showChannels("HSV", hsv, 0.75);

	uchar *hsvData = (uchar *)hsv.data;
	int index;
	int hueElems = 0;
	Contours container;
	for(Contours::iterator it = contours.begin(); it < contours.end(); ++it){
		hueElems = 0;
		for(CvPoints::iterator cit = it->begin(); cit < it->end(); ++cit){
			index = cit->y * hsv.step + cit->x * hsv.channels(); // first channel
			if(hsvData[index] > hbin[0] && hsvData[index] < hbin[1]) hueElems++;
		}
		double hueMark = 100.0 * hueElems / it->size();
		//std::cout << "Hue:" << hueMark << ";";
		if(hueMark < 60){
			container.push_back(*it);
			cv::drawContours(dst, container, -1, cv::Scalar(0, 0, 0));
			container.pop_back();
		}
	}
}

void moveLinearSnake(cv::Mat &img, IntPolygon *polygon, cv::Point offset, int rings, HistoryKeeper* hk = NULL){
	CvPoints &p = polygon->accessPoints();
	uchar *data = (uchar *)img.data;
	int index;
	bool edge;
	for(CvPoints::iterator it = p.begin(); it < p.end(); ++it){
		CVShortcuts::RingIterator rit(*it - offset, rings, img);
		cv::Point neighbour;
		while(rit.nextTrimmed(neighbour)){
			index = neighbour.y * img.step + neighbour.x * img.channels();
			if(img.channels() == 3){
				edge = data[index] > 0 || data[index + 1] > 0 || data[index + 2] > 0;
			}else{
				edge = data[index] > 0;
			}
			if(edge){
				*it = neighbour + offset;
				break;
			}
		}
	}
	if(hk != NULL)
	{
		ContourHistory* ch = dynamic_cast<ContourHistory*>(hk);
		ch->updatePositions(polygon);
		polygon->makeRegular(polygon->getMaxDist(), polygon->getMinDist(), polygon->getMinAng(), ch);
	}
	else
	{
		polygon->makeRegular(polygon->getMaxDist(), polygon->getMinDist(), polygon->getMinAng());
	}
}

//	CannyDeformableTracker::CannyDeformableTracker(IntPolygon *p, cv::Mat &img,
//		int hSlices) : edgeThresh(95),
//		//lowThreshold(6), max_lowThreshold(100), ratio(3), kernel_size(3),
//		lowThreshold(6), max_lowThreshold(100), ratio(3), kernel_size(3),
//		hSlices(hSlices){
//		p->makeRegular(14, 3, 33);
//		selectHueSlice(hbin, p, img, hSlices);
//		track(img, p);
//	}
int CannyDeformableTracker::setAndParse(std::string &params){
	std::istringstream i(params);
	char ch;
	i >> ch;  if(ch != '['){
		std::cerr << "Format error: Canny parameters expected '[' got '" << ch << "'" << std::endl;
		return -1;
	}
	i >> hSlices;
	i >> rings;
	i >> maxDistance;
	i >> minDistance;
	i >> minAngle;
	i >> blur_kernel_size;
	i >> ratio;
	i >> ch;  if(ch != ']'){
		std::cerr << "Format error: end of Canny parameters expected ']' got '" << ch << "'" << std::endl;
		return -1;
	}
	return 0;
}
CannyDeformableTracker::CannyDeformableTracker(IntPolygon *p, cv::Mat &img, std::string &params)
	 : edgeThresh(95), ratio(3), kernel_size(3), blur_kernel_size(9),
	lowThreshold(6), max_lowThreshold(100),
	hSlices(9), maxDistance(14), minDistance(3), minAngle(33), rings(15){
	if(setAndParse(params)<0){
		std::cerr << "Parsing tracker parameters failed, using default";
	}
	p->makeRegular(maxDistance, minDistance, minAngle);
	selectHueSlice(hbin, p, img, hSlices);
	track(img, p);

}

int CannyDeformableTracker::track(cv::Mat &img, Shape *s, HistoryKeeper* hk)
{
	// Region where to look for the polygon.
	IntPolygon *p = dynamic_cast<IntPolygon*>(s);
	int x1, x2, y1, y2;
	p->calculateMark(img, x1, x2, y1, y2);

	cv::Mat src = img(cv::Rect(x1, y1, x2-x1, y2-y1));
	cv::Mat detected_edges;
	cv::Mat dst;

	// From: canny.cpp opencv samples.
	cv::cvtColor(src, detected_edges, CV_BGR2GRAY); /// Convert the image to grayscale
	//cv::blur(detected_edges, detected_edges, cv::Size(3, 3)); /// Reduce noise with a kernel 3x3
	cv::blur(detected_edges, detected_edges, cv::Size(blur_kernel_size, blur_kernel_size)); /// Reduce noise with a kernel 3x3
	//VAGUI::show("Blur", detected_edges, 1.0);
	cv::Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size); /// Canny detector
	src.copyTo(dst, detected_edges); /// Using Canny’s output as a mask, recover colour of edges
	//VAGUI::show("Canny I", dst, 1.0);

	//VAGUI::showChannels("BGR", img, 0.25);
	//VAGUI::showChannels("BGREdges", dst, 0.75);

	/// What about Laplacian? Laplacian of the Gaussian, difference of Gaussians. Those are biologically plausible.


	Contours contours;
	//cv::findContours(detected_edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(x1, y1));
	//cv::drawContours(img, contours, -1, cv::Scalar(250, 0, 0));
	//cv::findContours(detected_edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//filterH(dst, contours, hbin);
	//VAGUI::show("Canny II", dst, 1.0);

	if (hk != NULL)
	{
		mc_dbgmsg("I'm goin' to remember this!")
		moveLinearSnake(dst, p, cv::Point(x1, y1), rings, hk);
	}
	else
	{
		moveLinearSnake(dst, p, cv::Point(x1, y1), rings);
	}

	return 0;
}

}



