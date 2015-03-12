#include "Common/Log.h"
#include "Common/Image.h"
#include "Features/FeatureHandler.h"

#include <iostream>
#include <set>
#include <dirent.h>

#include <opencv2/imgproc/imgproc.hpp>

void filterImagesByFeatures( Detector detID, DescriptorExtractor desID, DescriptorMatcher dmID,
		float ratio, float threshold );
void filterImagesByHistograms( int sections, int compareMethod, float threshold );

void printFileNames();
void printRejectedImages();

void getFileList();
void initImages();

void proccessImagesFeatures( FeatureHandler* featureHandler );
void calculateHistograms( int sections );

std::string _path;
std::set< std::string > _files;
std::set< Image* > _images;
std::map< std::string, float > _rejectedimages;

bool Log::debug = true;

int main(int argc, char** argv)
{
	_path = "data/videothumbs";
	getFileList();

	//filterImagesByFeatures( DET_FAST, DES_SURF, DESM_FB, 0.65f, 0.05f );
	//printRejectedImages();

	filterImagesByHistograms(4, CV_COMP_CORREL, 0.92f);
	printRejectedImages();

	//filterImagesByHistograms(3, CV_COMP_INTERSECT, 610000.0f);
	//printRejectedImages();

	//filterImagesByHistograms(3, CV_COMP_CHISQR, 93000.0f);
	//printRejectedImages();
}

void proccessImagesFeatures( FeatureHandler* featureHandler )
{
	initImages();
	for( std::set< Image* >::iterator it = _images.begin(); it != _images.end(); it++ )
	{
		featureHandler->detect( *it );
		featureHandler->computeDescriptors( *it );
	}
}

void filterImagesByFeatures( Detector detID, DescriptorExtractor desID, DescriptorMatcher dmID,
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
			float matchThreshold = (*it)->getImageMatchConfidence( *aux );

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

void filterImagesByHistograms( int sections, int compareMethod, float threshold )
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

void calculateHistograms(int sections)
{
	initImages();
	for( std::set< Image* >::iterator it = _images.begin(); it != _images.end(); it++ )
	{
		(*it)->calcHistogram(sections);
	}
}

void initImages()
{
	for( std::set< std::string >::iterator it = _files.begin(); it != _files.end(); it++ )
		{
			std::string path( _path + "/" + (*it) );
			Image* img = new Image( path, 0.0f, 0.0f, 0.0f, 0.40f );
			_images.insert( img );
		}
}

void getFileList()
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

void printFileNames()
{
	for( std::set< std::string >::iterator it = _files.begin(); it != _files.end(); it++ )
	{
		std::cout << (*it) << std::endl;
	}
}

void printRejectedImages()
{
	Log::notice( "Rejected " + Log::to_string( (int) _rejectedimages.size() ) + " images" );
	for( std::map< std::string, float >::iterator it = _rejectedimages.begin(); it != _rejectedimages.end(); it++ )
	{
		Log::notice( (*it).first + " [threshold = " + Log::to_string( (*it).second )  + "]" );
	}
}
