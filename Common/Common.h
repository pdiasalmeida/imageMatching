#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <vector>

#include <opencv2/features2d/features2d.hpp>

enum Detector{DET_SIFT, DET_SURF, DET_FAST, DET_GFTT};
enum DescriptorExtractor{DES_SIFT, DES_SURF, DES_BRIEF, DES_ORB, DES_FRE};
enum DescriptorMatcher{DESM_BF1, DESM_BF2, DESM_BFH1, DESM_BFH2, DESM_FB};

enum MatchesType{MA_IN, MA_GD};

struct image_match
{
	std::string imageMatched;
	std::vector< cv::DMatch > simpleMatches;
	std::vector< std::vector< cv::DMatch > > initialKnnMatches;
	std::vector< std::vector< cv::DMatch > > survivingRTMatches;
	std::vector< cv::DMatch > goodMatches;
	float ratio;
	double confidence;
	int ratioTestRemoved;
	int symetryTestRemoved;

	image_match()
	{
		ratio = -1.0f;
		confidence = -1.0;
		ratioTestRemoved = -1;
		symetryTestRemoved = -1;
	}
};

struct keypointInImage
{
	std::string _image;
	cv::KeyPoint _kp;

	keypointInImage( std::string img, cv::KeyPoint kp )
	{
		_image = img;
		_kp = kp;
	}
};

#endif
