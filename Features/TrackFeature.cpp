#include "TrackFeature.h"

TrackFeature::TrackFeature( Image* image, cv::KeyPoint kp, cv::Mat desc )
{
	_origImage = image;
	_origKeypoint = kp;
	_origDescriptor = desc;
}

void TrackFeature::addFeatureImage( std::string image, cv::KeyPoint kp )
{
	_featureImages.push_back( new keypointInImage( image, kp ) );
}

Image* TrackFeature::asImage()
{
	return new Image( _origDescriptor );
}

std::string TrackFeature::to_string()
{
	std::ostringstream oss;

	oss << "Feature found in: " << _origImage->getPath() << std::endl;
	oss << "Feature also present in: {";
	for( std::vector< keypointInImage* >::iterator it = _featureImages.begin(); it != _featureImages.end(); it++ )
	{
		oss << (*it)->_image;
		oss << std::endl;
	}
	oss << "}" << std::endl;

	return oss.str();
}

TrackFeature::~TrackFeature()
{
	delete _origImage;

	for( std::vector< keypointInImage* >::iterator it = _featureImages.begin(); it != _featureImages.end(); it++ )
	{
		delete *it;
	}
}
