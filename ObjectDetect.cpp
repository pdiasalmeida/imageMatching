#include "Common/Common.h"
#include "Common/Log.h"
#include "Common/Image.h"
#include "Features/FeatureHandler.h"

#include <dirent.h>
#include <set>

#include <opencv2/calib3d/calib3d.hpp>

Image* processImage( std::string path );
void findObjectInScene( std::string path );

void testDir( std::string path );
void getFileList(std::string path);

FeatureHandler* _fH;
float _ratio;
Image* _object;
std::string _path;

std::set< std::string > _files;

bool Log::debug = true;

int main( int argc, char** argv )
{
	int status = EXIT_SUCCESS;

	if(argc >= 3)
	{
		_fH = new FeatureHandler( DET_SIFT, DES_SIFT, DESM_FB );
		_object = processImage(argv[1]);
		_ratio = 0.85f;

		testDir(argv[2]);
	}
	else
	{
		status = EXIT_FAILURE;
		std::cout << "Problem lauching program." << std::endl
				<< "USAGE: objectDetector <path object image> <path to test dir>" << std::endl;
	}

	return status;
}

void findObjectInScene( std::string path )
{
	Image* scene = processImage(path);

	_fH->findGoodMatches( _object, scene, _ratio );
	std::vector< cv::DMatch > goodMatches = _object->getGoodMatches( scene );
	int nMatches = goodMatches.size();

	cv::Mat out;
	_object->drawMatches( scene, nMatches, MA_GD, out );

	//-- Localize the object
	std::vector<cv::Point2f> inObj;
	std::vector<cv::Point2f> inScene;

	for( int i = 0; i < nMatches; i++ )
	{
		//-- Get the keypoints from the good matches
		inObj.push_back( _object->getKeypoints()[ goodMatches[i].queryIdx ].pt );
		inScene.push_back( scene->getKeypoints()[ goodMatches[i].trainIdx ].pt );
	}

	cv::Mat H = cv::findHomography( inObj, inScene, CV_RANSAC );

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<cv::Point2f> obj_corners(4);
	obj_corners[0] = cv::Point(0,0);
	obj_corners[1] = cv::Point( _object->getImage().cols, 0 );
	obj_corners[2] = cv::Point( _object->getImage().cols, _object->getImage().rows );
	obj_corners[3] = cv::Point( 0, _object->getImage().rows );
	std::vector<cv::Point2f> scene_corners(4);

	perspectiveTransform( obj_corners, scene_corners, H);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	cv::line( out, scene_corners[0] + cv::Point2f( _object->getImage().cols, 0),
			scene_corners[1] + cv::Point2f( _object->getImage().cols, 0), cv::Scalar(0, 255, 0), 4 );
	cv::line( out, scene_corners[1] + cv::Point2f( _object->getImage().cols, 0),
			scene_corners[2] + cv::Point2f( _object->getImage().cols, 0), cv::Scalar( 0, 255, 0), 4 );
	cv::line( out, scene_corners[2] + cv::Point2f( _object->getImage().cols, 0),
			scene_corners[3] + cv::Point2f( _object->getImage().cols, 0), cv::Scalar( 0, 255, 0), 4 );
	cv::line( out, scene_corners[3] + cv::Point2f( _object->getImage().cols, 0),
			scene_corners[0] + cv::Point2f( _object->getImage().cols, 0), cv::Scalar( 0, 255, 0), 4 );

	cv::namedWindow("Matches", CV_WINDOW_NORMAL);
	cv::imshow("Matches", out);
	cv::waitKey(0);
}

Image* processImage( std::string path )
{
	Image* img = new Image( path, 0.0f, 0.0f, 0.0f, 0.0f );

	_fH->detect( img );
	_fH->computeDescriptors( img );

	return img;
}

void testDir( std::string path )
{
	getFileList(path);

	std::set< std::string >::iterator it = _files.begin();
	for( ; it != _files.end(); it++ )
	{
		findObjectInScene( path+"/"+*it );
	}
}

void getFileList(std::string path)
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
	  Log::exit_error("Unable to access directory " + _path );
	}
}
