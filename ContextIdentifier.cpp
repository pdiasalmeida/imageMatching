#include "Common/Log.h"
#include "Common/Image.h"
#include "Features/FeatureHandler.h"

#include <iostream>
#include <set>
#include <dirent.h>

#include <libconfig.h++>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

bool parseConfig(char* path);
void setEntity( std::string name, std::string imagePath );
void findImageContext( std::string imagePath );

void setShowMatches(bool value);
void testDir( std::string path );

Image* processImage( std::string path );
void getFileList(std::string path);
void getEntities(std::string path);
void printFileNames();

FeatureHandler* _fH;
double _ratio;
float _matchesConfidenceThresh;
bool _showMatches;

std::string _entityPath;
std::string _testPath;

std::set< std::string > _files;
std::vector< std::pair< std::string, Image*  > > _entities;

bool Log::debug = true;

int main( int argc, char** argv )
{
	int status = EXIT_SUCCESS;

	if(argc >= 2)
	{
		if( parseConfig(argv[1]) )
		{
			getEntities(_entityPath);
			testDir(_testPath);
		}
		else status = EXIT_FAILURE;
	}
	else
	{
		status = EXIT_FAILURE;
		std::cout << "Problem lauching program." << std::endl
				<< "USAGE: contextIdentifier <path to config file>" << std::endl;
	}

	return status;
	/**setMovie( "The Grey", "./data/movies/grey.jpg" );
	setMovie( "The Silence of the Lambs", "./data/movies/lambs.jpg" );
	setMovie( "The Dark Knight", "./data/movies/batman.jpg" );

	testDir( "./data/movies/tests" );


	setMovie( "Bom Jesus", "./data/monuments/bom_jesus.jpg" );
	setMovie( "Sameiro", "./data/monuments/sameiro.jpg" );

	testDir( "./data/monuments/tests" );*/
}

bool parseConfig(char* path)
{
	bool status = true;

	libconfig::Config cfg;
	cfg.readFile(path);

	//----------------------------------------------- get global variables -------------------------------------//
	cfg.lookupValue("application.showMatches", _showMatches);
	cfg.lookupValue("application.matchesConfidenceThershold", _matchesConfidenceThresh);

	//----------------------------------------------- build feature handler -------------------------------------//
	int detector = 1;
	int descriptor = 1;
	int matcher = 1;
	cfg.lookupValue("application.featureHandler.detector", detector);
	cfg.lookupValue("application.featureHandler.descriptor", descriptor);
	cfg.lookupValue("application.featureHandler.matcher", matcher);
	cfg.lookupValue("application.featureHandler.simmetryTestRatio", _ratio);

	_fH = new FeatureHandler( (Detector)detector, (DescriptorExtractor)descriptor, (DescriptorMatcher)matcher );

	//-------------------------------------------- get entities and test paths ----------------------------------//
	cfg.lookupValue("application.entities.templatesPath", _entityPath);
	cfg.lookupValue("application.entities.testsPath", _testPath);

	return status;
}

void setEntity( std::string name, std::string imagePath )
{
	Image* img = processImage(imagePath);

	_entities.push_back( std::make_pair< std::string, Image* >( name, img) );
}

void findImageContext( std::string imagePath )
{
	Image* img = processImage(imagePath);

	bool found = false;
	std::pair< std::string, Image*  > match;
	int bestnMatches = 0;
	float bestMatchConfidence = 0;

	std::vector< std::pair< std::string, Image*  > >::iterator it = _entities.begin();
	for( ; !found && it != _entities.end(); it++ )
	{
		_fH->findGoodMatches( img, it->second, _ratio );
		int nMatches = img->getGoodMatches( it->second ).size();
		float mConf = img->getImageMatchConfidence(it->second);

		if( mConf > bestMatchConfidence )
		{
			bestMatchConfidence = mConf;
			bestnMatches = nMatches;
			match = *it;

			if( mConf > 2*_matchesConfidenceThresh )
				found = true;
		}
	}

	if( !found && (bestMatchConfidence < _matchesConfidenceThresh) )
	{
		std::cout << "Was not able to find context for image '" << imagePath << "'." << std::endl;
	}
	else
	{
		std::cout << "Found context for image '" << imagePath << "': " << match.first
				<< ". With " << bestnMatches << " feature matches (" << bestMatchConfidence << ")." << std::endl;

		if(_showMatches)
		{
			cv::Mat out;
			img->drawMatches( match.second, bestnMatches, MA_GD, out, cv::Scalar(255,0,0,255) );
			cv::namedWindow("Matches", CV_WINDOW_NORMAL);
			cv::imshow("Matches", out);
			cv::waitKey(0);
		}
	}
}

void testDir( std::string path )
{
	getFileList(path);

	std::set< std::string >::iterator it = _files.begin();
	for( ; it != _files.end(); it++ )
	{
		findImageContext( path+"/"+*it );
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

void getEntities( std::string path )
{
	DIR *dir;
	struct dirent *ent;
	if( ( dir = opendir( path.c_str() ) ) != NULL )
	{
		while( ( ent = readdir( dir ) ) != NULL)
		{
			if( ent->d_type == DT_REG )
			{
				setEntity( ent->d_name, path + "/"+ ent->d_name );
			}
		}
		closedir (dir);
	}
	else
	{
		Log::exit_error("Unable to access directory " + path );
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
