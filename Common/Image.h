#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include "Common.h"

#include <map>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class Image {
public:
	Image();
	Image( std::string path, float pCutTop, float pCutLeft, float pCutRight, float pCutBottom );
	Image( cv::Mat desc );
	~Image();

	std::string getPath();
	cv::Mat getImage();
	std::vector< cv::KeyPoint > getKeypoints();
	cv::KeyPoint getKeypoint( int index );
	cv::Mat getDescriptors();
	std::vector< cv::DMatch > getGoodMatches( Image* toMatch );

	float getImageMatchThreshold(Image* imgToMatch);
	std::vector< float > getHistogramCompareThresholds( Image* img );

	void setKeypoints(std::vector< cv::KeyPoint > keypoints);
	void setDescriptors(cv::Mat descriptors);

	void calcHistogram(int sections);
	void compareHistograms( Image* img, int compareMethod );

	void drawKeypoints(cv::Scalar color=cv::Scalar(255,255,255),
				int flag=cv::DrawMatchesFlags::DRAW_OVER_OUTIMG|cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	void drawMatches(Image* imgToMatch, int numberMatches, MatchesType type, cv::Mat &result,
				cv::Scalar color=cv::Scalar(255,255,255));
	void show(std::string name);
	void showROI( std::string name );
	void showHistogramAsImage();

	void printCompareHistograms( Image* img );

	friend class FeatureHandler;

protected:
	std::string _path;
	cv::Mat _image;
	cv::Mat _roi;

	std::vector< cv::KeyPoint > _keypoints;
	cv::Mat _descriptors;
	std::map< std::string, image_match* > _matches;

	std::vector< cv::Mat > _histograms;
	std::map< std::string, std::vector< float > > _histogramCompare;
};

#endif
