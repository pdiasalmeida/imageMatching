#include "VideoTrack.h"

#include <iostream>
#include <dirent.h>

#include <opencv2/imgproc/imgproc.hpp>

VideoTrack::VideoTrack( std::string path )
{
	_path = path;
	getFileList();
}

void VideoTrack::trackFeatures( Detector detID, DescriptorExtractor desID, DescriptorMatcher dmID,
		float ratio )
{
	FeatureHandler* featureHandler = new FeatureHandler( detID, desID, dmID );

	Image* currentPic = NULL;
	for( std::set< std::string >::iterator it = _files.begin(); it != _files.end(); it++ )
	{
		std::string path( _path + "/" + (*it) );
		Image* img = new Image( path, 0.0f, 0.0f, 0.0f, 0.40f );
		_images.insert( img );

		featureHandler->detect( img );
		featureHandler->computeDescriptors( img );

		if( currentPic )
		{
			featureHandler->findGoodMatches( currentPic, img, ratio );
			std::vector< cv::DMatch > matches = currentPic->getGoodMatches( img );

			for( std::vector< cv::DMatch >::iterator itm = matches.begin(); itm != matches.end(); itm++ )
			{
				int currImgIndex = itm->queryIdx;
				cv::Mat cImgDesc = currentPic->getDescriptors().row( currImgIndex );
				cImgDesc.push_back(cImgDesc);
				cv::KeyPoint cImgkp = currentPic->getKeypoint( currImgIndex );

				TrackFeature* tf = new TrackFeature( currentPic, cImgkp, cImgDesc );
				Image* asImage = tf->asImage();

				bool foundInStorage = false;
				for( std::vector< TrackFeature* >::iterator itt = _featuresInTrack.begin();
								!foundInStorage && itt != _featuresInTrack.end(); itt++ )
				{
					Image* stFeatasImage = (*itt)->asImage();
					featureHandler->findGoodMatches( stFeatasImage , asImage, 1.0 );

					std::vector< cv::DMatch> gm = stFeatasImage->getGoodMatches( asImage );
					if( gm.size() > 0 && gm.at( 0 ).distance == 0 )
					{
						(*itt)->addFeatureImage( img->getPath(), cImgkp );
						foundInStorage = true;
					}
				}
				if( !foundInStorage ) _featuresInTrack.push_back( tf );
			}
		}
		currentPic = img;
	}
}

void VideoTrack::proccessImagesFeatures( FeatureHandler* featureHandler )
{
	initImages();
	for( std::set< Image* >::iterator it = _images.begin(); it != _images.end(); it++ )
	{
		featureHandler->detect( *it );
		featureHandler->computeDescriptors( *it );
	}
}

void VideoTrack::filterImagesByFeatures( Detector detID, DescriptorExtractor desID, DescriptorMatcher dmID,
		float ratio, float threshold )
{
	FeatureHandler* featureHandler = new FeatureHandler( detID, desID, dmID );
	proccessImagesFeatures( featureHandler );

	for( std::set< Image* >::iterator it = _images.begin(); it != _images.end(); it++ )
	{
		bool detectedChange = false;
		std::set< Image* >::iterator aux = it; aux++;
		while( !detectedChange && aux != _images.end() )
		{
			featureHandler->findGoodMatches( *it, *aux, ratio );
			float matchThreshold = (*it)->getImageMatchThreshold( *aux );

			if( matchThreshold > threshold )
			{
				Log::notice( "filterImagesByFeatures rejected image " + (*aux)->getPath() );
				_rejectedimages.insert( std::pair< std::string, float >( (*aux)->getPath(), matchThreshold ) );
				aux++;
			}
			else
			{
//				it = aux; --it;
				detectedChange = true;
			}
		}
	}
}

void VideoTrack::filterImagesByHistograms( int sections, int compareMethod, float threshold )
{
	calculateHistograms(sections);
	std::set< Image* >::iterator it = _images.begin();
	cv::imwrite( "videothumbsA/" + (*it)->getPath().substr(12), (*it)->getImage() );
	for( ; it != _images.end(); it++ )
	{
		bool detectedChange = false;
		std::set< Image* >::iterator aux = it; aux++;
		while( !detectedChange && aux != _images.end() )
		{
			(*it)->compareHistograms( *aux, compareMethod );
			std::vector< float > compareThresholds = (*it)->getHistogramCompareThresholds( *aux );

			int n = 0;
			bool match = true;
			float front = compareThresholds.at( n );
			while( match && n < (int) compareThresholds.size() )
			{
				if( compareMethod == CV_COMP_CORREL || compareMethod == CV_COMP_INTERSECT )
				{
					match = ( compareThresholds.at( n ) > threshold );
					front = (compareThresholds.at( n ) < front ) ? compareThresholds.at( n ) : front;
				}
				else
				{
					match = ( compareThresholds.at( n ) < threshold );
					front = (compareThresholds.at( n ) > front ) ? compareThresholds.at( n ) : front;
				}
				n++;
			}
			if( match )
			{
				Log::notice( "filterImagesByHistograms rejected image " + (*aux)->getPath() );
				_rejectedimages.insert( std::pair< std::string, float >( (*aux)->getPath(), front ) );
				aux++;
			}
			else
			{
				cv::imwrite( "videothumbsA/" + (*aux)->getPath().substr(12), (*aux)->getImage() );
				it = aux; --it;
				detectedChange = true;
			}
		}
	}
}

void VideoTrack::calculateHistograms(int sections)
{
	initImages();
	for( std::set< Image* >::iterator it = _images.begin(); it != _images.end(); it++ )
	{
		(*it)->calcHistogram(sections);
	}
}

void VideoTrack::initImages()
{
	for( std::set< std::string >::iterator it = _files.begin(); it != _files.end(); it++ )
		{
			std::string path( _path + "/" + (*it) );
			Image* img = new Image( path, 0.0f, 0.0f, 0.0f, 0.40f );
			_images.insert( img );
		}
}

void VideoTrack::getFileList()
{
	DIR *dir;
	struct dirent *ent;
	if( ( dir = opendir( _path.c_str() ) ) != NULL )
	{
		while( ( ent = readdir( dir ) ) != NULL)
		{
			if( ent->d_type == DT_REG )
			{
				_files.insert( std::string( ent->d_name ) );
			}
		}
		closedir (dir);
	}
	else
	{
	  Log::exit_error("Unable to access directory " + _path );
	}
}

void VideoTrack::printFeaturesInTrack()
{
	for( std::vector< TrackFeature* >::iterator it = _featuresInTrack.begin();
			it != _featuresInTrack.end(); it++ )
	{
		std::cout << (*it)->to_string() << std::endl;
	}
}

void VideoTrack::printFileNames()
{
	for( std::set< std::string >::iterator it = _files.begin(); it != _files.end(); it++ )
	{
		std::cout << (*it) << std::endl;
	}
}

void VideoTrack::printRejectedImages()
{
	Log::notice( "Rejected " + Log::to_string( (int) _rejectedimages.size() ) + " images" );
	for( std::map< std::string, float >::iterator it = _rejectedimages.begin(); it != _rejectedimages.end(); it++ )
	{
		Log::notice( (*it).first + " [threshold = " + Log::to_string( (*it).second )  + "]" );
	}
}

VideoTrack::~VideoTrack()
{
	for( std::set< Image* >::iterator it = _images.begin(); it != _images.end(); it++ )
	{
		delete (*it);
	}
}
