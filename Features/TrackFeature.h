#ifndef TRACKFEATURE_HPP_
#define TRACKFEATURE_HPP_

#include "../Common/Image.h"

#include <map>
#include <opencv2/core/core.hpp>

class TrackFeature {
public:
	TrackFeature( Image* image, cv::KeyPoint kp, cv::Mat desc );
	~TrackFeature();

	void addFeatureImage( std::string image, cv::KeyPoint kp );
	Image* asImage();

	std::string to_string();

protected:
	Image* _origImage;
	cv::KeyPoint _origKeypoint;
	cv::Mat _origDescriptor;

	std::vector< keypointInImage* > _featureImages;

private:

};

#endif
