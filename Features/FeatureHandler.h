#ifndef FEATUREHANDLER_HPP_
#define FEATUREHANDLER_HPP_

#include "../Common/Image.h"
#include "../Common/Common.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class FeatureHandler {
public:
	FeatureHandler( Detector detectorID, DescriptorExtractor extractorID, DescriptorMatcher matcherID );
	~FeatureHandler();

	void detect( Image* img );
	void computeDescriptors( Image* img );
	void matchDescriptors( Image* first, Image* second );
	void findGoodMatches( Image* image1, Image* image2, float ratio );

	void setFeatureDetectorSIFT( double threshold, double edgeThreshold );
	void setFeatureDetectorSURF( double hessianThreshold, int octaves, int octaveLayers );
	void setFeatureDetectorFAST(int threshold, bool nmaxSupre);

	void setDescriptorExtractorSIFT();
	void setDescriptorExtractorSURF();
	void setDescriptorExtractorBRIEF( int bytes );
	void setDescriptorExtractorORB();
	void setDescriptorExtractorFREAK( bool orientationNormalized, bool scaleNormalized,
			float patternScale, int nOctaves);

protected:
	std::string _fd_name;
	std::string _de_name;
	std::string _dm_name;
	cv::FeatureDetector* _feature_detector;
	cv::DescriptorExtractor* _descriptor_extractor;
	cv::DescriptorMatcher* _descriptor_matcher;

private:
	void setTechiquesToUse( Detector detectorID, DescriptorExtractor extractorID, DescriptorMatcher matcherID );
	void setFeatureDetector( Detector detectorID );
	void setDescriptorExtractor( DescriptorExtractor extractorID );
	void setDescriptorMatcher( DescriptorMatcher matcherID );

	void setFeatureDetectorSIFT();
	void setFeatureDetectorSURF();
	void setFeatureDetectorFAST();
	void setFeatureDetectorGFTT();

	void setDescriptorExtractorBRIEF();
	void setDescriptorExtractorFREAK();

	void setDescriptorMatcherBF( int normType );
	void setDescriptorMatcherBFL1();
	void setDescriptorMatcherBFL2();
	void setDescriptorMatcherBFHM1();
	void setDescriptorMatcherBFHM2();
	void setDescriptorMatcherFB();

	void knnMatchDescriptors( Image* first, Image* second );
	void ratioTest(std::string image, image_match* im);
	int ratioTest( std::vector< std::vector<cv::DMatch> > &initialmatches,
			std::vector< std::vector<cv::DMatch> > &survivingmatches, float ratio );
	void symmetryTest( image_match* im1, image_match* im2 );
	void symmetryTest( std::vector< std::vector< cv::DMatch > > &matches1,
			std::vector< std::vector< cv::DMatch > > &matches2, std::vector< cv::DMatch > &symmetry_matches);

};

#endif
