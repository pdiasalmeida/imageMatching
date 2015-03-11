#include "Common/Log.h"
#include "Common/Image.h"
#include "Features/FeatureHandler.h"

#include <iostream>
#include <set>
#include <dirent.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

void setMonument( std::string name, std::string imagePath );
void findMonument( std::string imagePath );

void setShowMatches(bool value);
void testDir( std::string path );

Image* processImage( std::string path );
void getFileList(std::string path);
void printFileNames();

std::set< std::string > _files;
std::vector< std::pair< std::string, Image*  > > _monuments;

FeatureHandler* _fH;
double _ratio;
int _nMatchesThresh;
bool _showMatches;

bool Log::debug = true;

int main( int argc, char** argv )
{
	_fH = new FeatureHandler( DET_SIFT, DES_SIFT, DESM_BF2 );

	_ratio = 0.85;
	_nMatchesThresh = 18;
	_showMatches = true;

	setMonument( "Bom Jesus", "./data/monuments/bom_jesus.jpg" );
	setMonument( "Sameiro", "./data/monuments/sameiro.jpg" );

	testDir( "./data/monuments/tests" );
}

void setMonument( std::string name, std::string imagePath )
{
	Image* img = processImage(imagePath);

	_monuments.push_back( std::make_pair< std::string, Image* >( name, img) );
}

void findMonument( std::string imagePath )
{
	Image* img = processImage(imagePath);

	bool found = false;
	std::vector< std::pair< std::string, Image*  > >::iterator it = _monuments.begin();
	for( ; !found && it != _monuments.end(); it++ )
	{
		_fH->findGoodMatches( img, it->second, _ratio );
		int matchThreshold = img->getGoodMatches( it->second ).size();

		if( matchThreshold >= _nMatchesThresh )
		{
			std::cout << "Found Monument for image '" << imagePath << "': " << it->first
					<< ". With " << matchThreshold << " feature matches."<< std::endl;

			if(_showMatches)
			{
				cv::Mat out;
				img->drawMatches( it->second, matchThreshold, MA_GD, out, cv::Scalar(255,0,0,255) );
				cv::namedWindow("Matches", CV_WINDOW_NORMAL);
				cv::imshow("Matches", out);
				cv::waitKey(0);
			}
			found = true;
		}
	}
	if( !found ) std::cout << "Was not able to find monument for image '" << imagePath << "'." << std::endl;
}

void testDir( std::string path )
{
	getFileList(path);

	std::set< std::string >::iterator it = _files.begin();
	for( ; it != _files.end(); it++ )
	{
		findMonument( path+"/"+*it );
	}
}

Image* processImage( std::string path )
{
	Image* img = new Image( path, 0.0f, 0.0f, 0.0f, 0.0f );

	_fH->detect( img );
	_fH->computeDescriptors( img );

	return img;
}

void setShowMatches(bool value)
{
	_showMatches = value;
}

void printFileNames()
{
	for( std::set< std::string >::iterator it = _files.begin(); it != _files.end(); it++ )
	{
		std::cout << (*it) << std::endl;
	}
}

void getFileList( std::string path )
{
	DIR *dir;
	struct dirent *ent;
	if( ( dir = opendir( path.c_str() ) ) != NULL )
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
	  Log::exit_error("Unable to access directory " + path );
	}
}
