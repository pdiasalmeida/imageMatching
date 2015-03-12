#include "Image.h"
#include "Log.h"

#include <opencv2/imgproc/imgproc.hpp>

Image::Image()
{
	std::string _path("");
	cv::Mat _image;
	cv::Mat _roi;
	std::vector< cv::KeyPoint > _keypoints;
	cv::Mat _descriptors;
	std::map< std::string, image_match* > _matches;
}

Image::Image(std::string path, float pCutTop, float pCutLeft, float pCutRight, float pCutBottom)
{
	_path = path;
	_image = cv::imread( path, 1 );
	if( _image.empty() )
	{
		Log::exit_error("Unable to open image: '" + path + "'");
	}

	int difT = _image.rows * pCutTop;
	int difL = _image.cols * pCutLeft;
	int difR = _image.cols * pCutRight;
	int difB = _image.rows * pCutBottom;
	_roi = _image( cv::Rect( difL, difT, _image.cols - ( difR + difL ), _image.rows - ( difB + difT ) ) );

	std::vector< cv::KeyPoint > _keypoints;
	cv::Mat _descriptors;
	std::map< std::string, image_match* > _matches;
}

Image::Image( cv::Mat desc )
{
	_path = "descriptorAsImage";
	_image = desc;
	if( _image.empty() )
	{
		Log::exit_error("Unable to open image: '" + _path + "'");
	}
	_roi = _image;

	_descriptors = desc;
}

std::string Image::getPath()
{
	return std::string( _path );
}

cv::Mat Image::getImage()
{
	return _image.clone();
}

std::vector< cv::KeyPoint > Image::getKeypoints()
{
	std::vector< cv::KeyPoint > copy_keypoints;
	for( std::vector< cv::KeyPoint >::iterator it = _keypoints.begin(); it != _keypoints.end(); it++ )
	{
		copy_keypoints.push_back( cv::KeyPoint(
				it->pt.x, it->pt.y, it->size, it->angle, it->response, it->octave, it->class_id) );
	}

	return copy_keypoints;
}

cv::KeyPoint Image::getKeypoint( int index )
{
	cv::KeyPoint kp;
	kp = cv::KeyPoint( _keypoints.at( index ) );

	return kp;
}

cv::Mat Image::getDescriptors()
{
	return _descriptors.clone();
}

std::vector< cv::DMatch > Image::getGoodMatches( Image* toMatch )
{
	std::vector< cv::DMatch > res;
	std::map< std::string, image_match* >::iterator im = _matches.find( toMatch->_path );

	if( im != _matches.end() )
	{
		for( std::vector< cv::DMatch >::iterator it = im->second->goodMatches.begin();
				it != im->second->goodMatches.end(); it++ )
		{
			res.push_back( cv::DMatch( *it ) );
		}
	}

	return res;
}

float Image::getImageMatchConfidence(Image* imgToMatch)
{
	float result = -1.0f;

	std::map< std::string, image_match* >::iterator im = _matches.find( imgToMatch->_path );
	if( im != _matches.end() )
	{
		result = im->second->confidence;
	}

	return result;
}

void Image::setKeypoints( std::vector< cv::KeyPoint > keypoints )
{
	for( std::vector< cv::KeyPoint >::iterator it = keypoints.begin(); it != keypoints.end(); it++ )
		{
			_keypoints.push_back( cv::KeyPoint(
					it->pt.x, it->pt.y, it->size, it->angle, it->response, it->octave, it->class_id ) );
		}
}

void Image::setDescriptors( cv::Mat descriptors )
{
	_descriptors = descriptors.clone();
}

void Image::drawKeypoints( cv::Scalar color, int flag )
{
	( !_keypoints.size() > 0 ) ?
			Log::warning( "Empty keypoint vector" ):
			Log::notice( "Drawing keypoints in image " + _path );

	cv::drawKeypoints( _image, _keypoints, _image, color, flag );

}

void Image::drawMatches( Image* imgToMatch, int numberMatches, MatchesType type,
		cv::Mat &result, cv::Scalar color )
{
	std::map< std::string, image_match* >::iterator im = _matches.find( imgToMatch->_path );
	std::vector< cv::DMatch > matches;
	if( im != _matches.end() )
	{
		switch(type)
		{
			case 0:
				matches = im->second->simpleMatches;
				break;
			case 1:
				matches = im->second->goodMatches;
				break;
			default:
				matches = im->second->simpleMatches;
				break;

		}
		//std::nth_element( matches.begin(), matches.begin() + numberMatches, matches.end() );

		std::vector< cv::DMatch > copy_matches;
		int n = 0;
		for( std::vector< cv::DMatch >::iterator it = matches.begin();
				n < numberMatches && it != matches.end(); it++, n++ )
		{
			copy_matches.push_back( cv::DMatch( it->queryIdx, it->trainIdx, it->imgIdx, it->distance ) );
		}

		Log::notice( "Drawing matches between images " + _path + " and " + imgToMatch->_path );

		cv::drawMatches( _roi, _keypoints, imgToMatch->_roi, imgToMatch->_keypoints, copy_matches,
				result, color );
	}
	else
	{
		Log::warning("There is no matches to draw");
	}
}

void Image::calcHistogram( int sections )
{
	Log::notice( "Calculating histogram of image " + _path );
	clock_t t;
	t = clock();

	int ccp = 0;
	int step = ( _roi.cols / sections );
	for(int s = 0; s < sections; s++)
	{
		cv::Mat tocalc = _roi( cv::Rect( ccp, 0, step , _roi.rows ));
		ccp += step;
		std::vector< cv::Mat > chan;
		cv::split(tocalc, chan);

		for( int i = 0; i < (int) chan.size(); i++)
		{
			int histSize[] = { 256 };
			float hranges[] = { 0.0f, 255.0f };
			const float* ranges[] = { hranges };
			int channels[] = { 0 };

			cv::MatND hist;
			cv::calcHist( &chan.at( i ), 1, channels, cv::Mat(), hist, 1, histSize, ranges );
			_histograms.push_back( hist );
		}
	}

	t = clock() - t;
	float calcDuration = ( (float) t ) / CLOCKS_PER_SEC;

	Log::notice( "Finished calculating histogram in " + Log::to_string(calcDuration) + " seconds" );
}

void Image::compareHistograms( Image* img, int compareMethod )
{
	Log::notice( "Comparing histograms of images " + _path + " and " + img->_path );
	clock_t t;
	t = clock();

	if( _histograms.size() != img->_histograms.size() )
	{
		Log::exit_error( "Unable to compare histograms of given images" );
	}
	else
	{
		std::vector< float > hcv1;
		std::vector< float > hcv2;
		std::map< std::string, std::vector< float > >::iterator hcp1 = _histogramCompare.find( img->_path );
		std::map< std::string, std::vector< float > >::iterator hcp2 = img->_histogramCompare.find( _path );
		bool exists = !( hcp1 == _histogramCompare.end() ) && !( hcp2 == img->_histogramCompare.end() );
		if( exists )
		{
			hcv1 = hcp1->second;
			hcv2 = hcp2->second;
		}

		for( int i = 0; i < (int) _histograms.size(); i++ )
		{
			float value = cv::compareHist( _histograms.at( i ), img->_histograms.at( i ), compareMethod );
			hcv1.push_back( value );
			hcv2.push_back( value );
		}

		if( !exists )
		{
			_histogramCompare.insert( std::pair< std::string, std::vector< float > >( img->_path, hcv1 ) );
			img->_histogramCompare.insert( std::pair< std::string, std::vector< float > >( _path, hcv2 ) );
		}
	}

	t = clock() - t;
	float calcDuration = ( (float) t ) / CLOCKS_PER_SEC;
	Log::notice( "Finished comparing histograms in " + Log::to_string(calcDuration) + " seconds" );
}

std::vector< float > Image::getHistogramCompareThresholds( Image* img )
{
	std::vector< float > result;
	std::map< std::string, std::vector< float > >::iterator hcp = _histogramCompare.find( img->_path );
	if( hcp != _histogramCompare.end() )
	{
		result = hcp->second;
	}

	return result;
}

void Image::printCompareHistograms( Image* img )
{
	std::vector< float > hcv;
	std::map< std::string, std::vector< float > >::iterator hcp = _histogramCompare.find( img->_path );
	if( hcp != _histogramCompare.end() )
	{
		std::string to_print;
		for( int i = 0; i < (int) hcp->second.size(); i++ )
		{
			to_print += Log::to_string( hcp->second.at( i ) ) + " ";
		}
		Log::notice( to_print );
	}
}

void Image::showHistogramAsImage()
{
	int section = 0;
	std::string channels[] = {"B", "G", "R"};

	for( int n = 0; n < (int) _histograms.size(); n++ )
	{
		double maxVal = 0;
		double minVal = 0;
		cv::minMaxLoc( _histograms.at( n ), &minVal, &maxVal );

		cv::Mat histImg( 256, 256, CV_8U, cv::Scalar(255) );

		int hpt = static_cast< int >( 0.9 * 256 );
		for( int h = 0; h < 256; h++ )
		{
			float binVal = _histograms.at( n ).at< float >( h );
			int intensity = static_cast< int >( binVal * hpt/maxVal );
			cv::line( histImg, cv::Point( h, 256 ), cv::Point( h, 256 - intensity ),
					cv::Scalar::all( 0 ) );
		}
		if(n != 0 && n%3 == 0) section++;
		int c = n - (3*section);
		std::string name = "Histogram of channel " + channels[c] + " section " + Log::to_string( section );
		cv::namedWindow( name, 0 );
		cv::imshow( name, histImg );
	}
}

void Image::show( std::string name )
{
	cv::namedWindow( name, 0 );
	cv::imshow( name, _image );
}

void Image::showROI( std::string name )
{
	cv::namedWindow( name, 0 );
	cv::imshow( name, _roi );
}

Image::~Image()
{
	for( std::map< std::string, image_match* >::iterator it = _matches.begin(); it != _matches.end(); it++ )
	{
		delete it->second;
	}
}
