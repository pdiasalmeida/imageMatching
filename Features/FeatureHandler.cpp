#include "FeatureHandler.h"
#include "../Common/Log.h"

#include <opencv2/nonfree/features2d.hpp>

FeatureHandler::FeatureHandler(Detector detectorID, DescriptorExtractor extractorID,
		DescriptorMatcher matcherID)
{
	setTechiquesToUse( detectorID, extractorID, matcherID );
}

void FeatureHandler::setTechiquesToUse( Detector detectorID, DescriptorExtractor extractorID,
		DescriptorMatcher matcherID )
{
	if( ( (extractorID == DES_BRIEF) || (extractorID == DES_ORB) || (extractorID == DES_FRE) ) &&
			( matcherID == DESM_FB ) )
	{
		Log::warning( "Invalid combination of descriptors and matcher." );
		Log::warning( "FlannBased Descriptor Matcher is compatible with float based descriptors only" );
		Log::notice( "Setting default matcher" );
		matcherID = DESM_BFH2;
	}
	setFeatureDetector(detectorID);
	setDescriptorExtractor(extractorID);
	setDescriptorMatcher(matcherID);
}

void FeatureHandler::setFeatureDetector(Detector detectorID)
{
	switch(detectorID)
	{
		case 0:
			setFeatureDetectorSIFT();
			break;
		case 1:
			setFeatureDetectorSURF();
			break;
		case 2:
			setFeatureDetectorFAST();
			break;
		case 3:
			setFeatureDetectorGFTT();
			break;
		default:
			setFeatureDetectorFAST();
			break;
	}
}

void FeatureHandler::setDescriptorExtractor(DescriptorExtractor extractorID)
{
	switch(extractorID)
	{
		case 0:
			setDescriptorExtractorSIFT();
			break;
		case 1:
			setDescriptorExtractorSURF();
			break;
		case 2:
			setDescriptorExtractorBRIEF();
			break;
		case 3:
			setDescriptorExtractorORB();
			break;
		case 4:
			setDescriptorExtractorFREAK();
			break;
		default:
			setDescriptorExtractorSIFT();
			break;
	}
}

void FeatureHandler::setDescriptorMatcher( DescriptorMatcher matcherID )
{
	switch(matcherID)
		{
			case 0:
				setDescriptorMatcherBFL1();
				break;
			case 1:
				setDescriptorMatcherBFL2();
				break;
			case 2:
				setDescriptorMatcherBFHM1();
				break;
			case 3:
				setDescriptorMatcherBFHM2();
				break;
			case 4:
				setDescriptorMatcherFB();
				break;
			default:
				setDescriptorMatcherBFHM2();
				break;
		}
}

void FeatureHandler::detect( Image* img )
{
	Log::notice( "Applying " + _fd_name + " on image " + img->_path + " with dimensions: " +
			Log::to_string( img->_roi.cols ) + "x" + Log::to_string( img->_roi.rows ) );

	clock_t t;
	t = clock();

	_feature_detector->detect( img->_roi, img->_keypoints );

	t = clock() - t;
	float calcDuration = ( (float) t ) / CLOCKS_PER_SEC;

	Log::notice( "Finished feature detection in " + Log::to_string(calcDuration) + " seconds. Found " +
			Log::to_string( int( img->_keypoints.size() ) ) + " keypoints" );
}

void FeatureHandler::computeDescriptors( Image* img )
{
	if( img->_keypoints.size() > 0 )
	{
		Log::notice( "Applying " + _de_name + " on image " + img->_path );

		clock_t t;
		t = clock();

		_descriptor_extractor->compute( img->_roi, img->_keypoints, img->_descriptors );

		t = clock() - t;
		float calcDuration = ( (float) t ) / CLOCKS_PER_SEC;

		Log::notice( _de_name + " finished in " + Log::to_string( calcDuration ) + " seconds" );

		if( img->_descriptors.rows != int( img->_keypoints.size() ) )
		{
			Log::warning( "There was a problem applying " + _de_name );
		}
	}
	else
	{
		Log::warning( "Empty keypoint vector. Unable to apply descriptor extractor" );
	}
}

void FeatureHandler::matchDescriptors( Image* first, Image* second )
{
	if( !first->_descriptors.empty() && !second->_descriptors.empty() )
	{
		Log::notice( "Applying " + _dm_name + " between images " + first->_path + " and " + second->_path );

		image_match* m1;
		image_match* m2;
		std::map< std::string, image_match* >::iterator im1 = first->_matches.find( second->_path );
		std::map< std::string, image_match* >::iterator im2 = second->_matches.find( first->_path );

		bool exists = !( im1 == first->_matches.end() ) && !( im2 == second->_matches.end() );
		if( !exists )
		{
			m1 = new image_match();
			m2 = new image_match();
		}
		else
		{
			m1 = im1->second;
			m2 = im2->second;
		}

		std::vector<cv::DMatch> matches1;
		std::vector<cv::DMatch> matches2;

		clock_t t;
		t = clock();

		_descriptor_matcher->match( first->_descriptors, second->_descriptors, m1->simpleMatches );
		_descriptor_matcher->match( second->_descriptors, first->_descriptors, m2->simpleMatches );

		t = clock() - t;
		float calcDuration = ( (float) t ) / CLOCKS_PER_SEC;

		m1->imageMatched = second->_path;
		m2->imageMatched = first->_path;

		if( !exists )
		{
			first->_matches.insert( std::pair< std::string, image_match* >( second->_path, m1 ) );
			second->_matches.insert( std::pair< std::string, image_match* >( first->_path, m2 ) );
		}

		Log::notice( _dm_name + " finished in " + Log::to_string( calcDuration ) + " seconds. Found " +
				Log::to_string( int( matches1.size() + matches2.size() ) ) + " matches" );
	}
	else
	{
		Log::warning( "Empty image descriptor. Unable to apply descriptor matcher" );
	}
}

void FeatureHandler::knnMatchDescriptors( Image* first, Image* second )
{
	if( !first->_descriptors.empty() && !second->_descriptors.empty() )
	{
		Log::notice( "Applying knn " + _dm_name + " between images " + first->_path + " and " + second->_path );

		image_match* m1;
		image_match* m2;
		std::map< std::string, image_match* >::iterator im1 = first->_matches.find( second->_path );
		std::map< std::string, image_match* >::iterator im2 = second->_matches.find( first->_path );

		bool exists = !( im1 == first->_matches.end() ) && !( im2 == second->_matches.end() );
		if( !exists )
		{
			m1 = new image_match();
			m2 = new image_match();
		}
		else
		{
			m1 = im1->second;
			m2 = im2->second;
		}

		clock_t t;
		t = clock();

		_descriptor_matcher->knnMatch( first->_descriptors, second->_descriptors, m1->initialKnnMatches, 2 );
		_descriptor_matcher->knnMatch( second->_descriptors, first->_descriptors, m2->initialKnnMatches, 2 );

		t = clock() - t;
		float calcDuration = ( (float) t ) / CLOCKS_PER_SEC;

		m1->imageMatched = second->_path;
		m2->imageMatched = first->_path;

		if( !exists )
		{
			first->_matches.insert( std::pair< std::string, image_match* >( second->_path, m1 ) );
			second->_matches.insert( std::pair< std::string, image_match* >( first->_path, m2 ) );
		}

		Log::notice( _dm_name + " finished in " + Log::to_string ( calcDuration ) + " seconds. Found " +
				Log::to_string( int( m1->initialKnnMatches.size() + m2->initialKnnMatches.size() ) ) +
				" matches" );
	}
	else
	{
		Log::warning( "Empty image descriptor. Unable to apply descriptor matcher" );
	}
}

void FeatureHandler::findGoodMatches( Image* image1, Image* image2, float ratio )
{
	knnMatchDescriptors( image1, image2 );

	image_match* m1 = ( image1->_matches.find( image2->_path ) )->second;
	image_match* m2 = ( image2->_matches.find( image1->_path ) )->second;

	m1->ratio = m2->ratio = &ratio;
	ratioTest( image1->_path, m1 );
	ratioTest( image2->_path, m2 );

	symmetryTest( m1, m2 );

	int kps = ( image1->_keypoints.size() > image2->_keypoints.size() ) ?
			image2->_keypoints.size() :
			image1->_keypoints.size();
	float thresh = (float) m1->goodMatches.size() / (float) kps;

	m1->threshold = &thresh;
	m2->threshold = &thresh;
}

void FeatureHandler::ratioTest( std::string image, image_match* im )
{
	Log::notice( "Applying ratioTest[ratio=" + Log::to_string( (*im->ratio) ) + "] to matches between image '" +
			image + "' and image '" + im->imageMatched + "'" );

	std::vector< std::vector<cv::DMatch> > survivingmatches;

	clock_t t;
	t = clock();

	int removed = ratioTest( im->initialKnnMatches, survivingmatches, *im->ratio );

	t = clock() - t;
	float calcDuration = ( (float) t ) / CLOCKS_PER_SEC;

	im->ratioTestRemoved = &removed;
	im->survivingRTMatches = survivingmatches;

	Log::notice( "Ratio test finished in " + Log::to_string( calcDuration ) + " seconds. Removed " +
			Log::to_string( *im->ratioTestRemoved ) + " matches, retained " +
			Log::to_string( int( im->survivingRTMatches.size() ) ) );
}

int FeatureHandler::ratioTest( std::vector< std::vector<cv::DMatch> > &initialmatches,
		std::vector< std::vector<cv::DMatch> > &survivingmatches, float ratio )
{
	int removed = 0;
	for(std::vector< std::vector< cv::DMatch > >::iterator it = initialmatches.begin();
			it != initialmatches.end(); it++)
	{
		if( it->size() > 1 )
		{
			if( !( (*it)[0].distance / (*it)[1].distance > ratio ) )
			{
				survivingmatches.push_back( (*it) );
			}
			else
			{
				removed++;
			}
		}
		else
		{
			removed++;
		}
	}
	return removed;
}

void FeatureHandler::symmetryTest( image_match* im1, image_match* im2 )
{
	Log::notice("Applying symmetryTest to the matches between image '" + im2->imageMatched +
			           " and image '" + im1->imageMatched + "'");

	std::vector<cv::DMatch> goodMatches;

	clock_t t;
	t = clock();
	symmetryTest( im1->survivingRTMatches, im2->survivingRTMatches, goodMatches );
	t = clock() - t;
	float calcDuration = ( (float) t ) / CLOCKS_PER_SEC;

	im1->goodMatches = goodMatches;
	im2->goodMatches = goodMatches;

	Log::notice( "SymmetryTest finished in " + Log::to_string( calcDuration ) + " seconds. Retained " +
			Log::to_string( int( im1->goodMatches.size() ) ) );
}

void FeatureHandler::symmetryTest( std::vector< std::vector< cv::DMatch > > &matches1,
		std::vector< std::vector< cv::DMatch > > &matches2, std::vector< cv::DMatch > &symmetry_matches )
{
	for( std::vector< std::vector< cv::DMatch > >::const_iterator it_matches1 = matches1.begin();
			it_matches1 != matches1.end(); it_matches1++ )
	{
		if( it_matches1->size() < 2 )
		{
			continue;
		}
		for( std::vector< std::vector< cv::DMatch > >::const_iterator it_matches2 = matches2.begin();
				it_matches2 != matches2.end(); it_matches2++ )
		{
			if ( it_matches2->size() < 2 )
			{
				continue;
			}

			if( (*it_matches1)[0].queryIdx == (*it_matches2)[0].trainIdx &&
					(*it_matches2)[0].queryIdx == (*it_matches1)[0].trainIdx )
			{
				symmetry_matches.push_back( cv::DMatch(
						(*it_matches1)[0].queryIdx, (*it_matches1)[0].trainIdx, (*it_matches1)[0].distance) );
				break;
			}
		}
	}
}

void FeatureHandler::setFeatureDetectorSIFT(double threshold, double edgeThreshold)
{
	cv::SiftFeatureDetector* sift = new cv::SiftFeatureDetector(threshold, edgeThreshold);
	_feature_detector = sift;
	_fd_name = "SIFT Feature Detector";

	Log::notice("Feature detector set to SIFT: [" +
		            Log::to_string(threshold) + ", " +
					Log::to_string(edgeThreshold) + "]");
}

void FeatureHandler::setFeatureDetectorSIFT()
{
	setFeatureDetectorSIFT(0.04, 11.0);
}

void FeatureHandler::setFeatureDetectorSURF(double hessianThreshold, int octaves, int octaveLayers)
{
	cv::SurfFeatureDetector* surf = new cv::SurfFeatureDetector(hessianThreshold, octaves, octaveLayers);
	_feature_detector = surf;
	_fd_name = "SURF Feature Detector";

	Log::notice("Feature detector set to SURF: [" +
		            Log::to_string(hessianThreshold) + ", " +
					Log::to_string(octaves) + ", " +
					Log::to_string(octaveLayers) +"]");
}

void FeatureHandler::setFeatureDetectorSURF()
{
	setFeatureDetectorSURF(400, 3, 4);
}

void FeatureHandler::setFeatureDetectorFAST(int threshold, bool nmaxSupre)
{
	cv::FastFeatureDetector* fast = new cv::FastFeatureDetector( threshold, nmaxSupre );
	_feature_detector = fast;
	_fd_name = "FAST Feature Detector";

	std::string nmaxS = nmaxSupre ? "true" : "false";
	Log::notice("Feature detector set to FAST: [" +
			Log::to_string(threshold) + ", " +
			nmaxS +"]");
}

void FeatureHandler::setFeatureDetectorFAST()
{
	setFeatureDetectorFAST(20, true);
}

void FeatureHandler::setFeatureDetectorGFTT()
{
	cv::GoodFeaturesToTrackDetector* gftt = new cv::GoodFeaturesToTrackDetector;
	_feature_detector = gftt;
	_fd_name = "GoodFeaturesToTrack Feature Detector";

	Log::notice( "Feature detector set to GoodFeaturesToTrack" );
}

void FeatureHandler::setDescriptorExtractorSIFT()
{
	cv::SiftDescriptorExtractor* sift_de = new cv::SiftDescriptorExtractor;
	_descriptor_extractor = sift_de;
	_de_name = "SIFT Descriptor Extractor";

	Log::notice("Descriptor Extractor set to SIFT");
}

void FeatureHandler::setDescriptorExtractorSURF()
{
	cv::SurfDescriptorExtractor* surf_de = new cv::SurfDescriptorExtractor;
	_descriptor_extractor = surf_de;
	_de_name = "SURF Descriptor Extractor";

	Log::notice("Descriptor Extractor set to SURF");
}

void FeatureHandler::setDescriptorExtractorBRIEF(int bytes)
{
	cv::BriefDescriptorExtractor* brief_de = new cv::BriefDescriptorExtractor( bytes );
	_descriptor_extractor = brief_de;
	_de_name = "BRIEF Descriptor Extractor";

	Log::notice("Descriptor Extractor set to BRIEF");
}

void FeatureHandler::setDescriptorExtractorBRIEF()
{
	setDescriptorExtractorBRIEF(32);
}

void FeatureHandler::setDescriptorExtractorORB()
{
	cv::OrbDescriptorExtractor* orb_de = new cv::OrbDescriptorExtractor;
	_descriptor_extractor = orb_de;
	_de_name = "ORB Descriptor Extractor";

	Log::notice("Descriptor Extractor set to ORB");
}

void FeatureHandler::setDescriptorExtractorFREAK( bool orientationNormalized, bool scaleNormalized,
		float patternScale, int nOctaves)
{
	cv::FREAK* freak_de = new cv::FREAK( true, true, 22.0f, 4 );
	_descriptor_extractor = freak_de;
	_de_name = "FREAK Descriptor Extractor";

	Log::notice("Descriptor Extractor set to FREAK");
}

void FeatureHandler::setDescriptorExtractorFREAK()
{
	setDescriptorExtractorFREAK( true, true, 22.0f, 4 );
}

void FeatureHandler::setDescriptorMatcherBF( int normType )
{
	cv::BFMatcher* bf_dm = new cv::BFMatcher( normType );
	_descriptor_matcher = bf_dm;
	_dm_name = "BruteForce Descriptor Matcher";

	Log::notice("Descriptor Matcher set to BruteForce");
}

void FeatureHandler::setDescriptorMatcherBFL1()
{
	setDescriptorMatcherBF( cv::NORM_L1 );
}

void FeatureHandler::setDescriptorMatcherBFL2()
{
	setDescriptorMatcherBF( cv::NORM_L2 );
}

void FeatureHandler::setDescriptorMatcherBFHM1()
{
	setDescriptorMatcherBF( cv::NORM_HAMMING );
}

void FeatureHandler::setDescriptorMatcherBFHM2()
{
	setDescriptorMatcherBF( cv::NORM_HAMMING2 );
}

void FeatureHandler::setDescriptorMatcherFB()
{
	cv::FlannBasedMatcher* fb_dm = new cv::FlannBasedMatcher();
	_descriptor_matcher = fb_dm;
	_dm_name = "FlannBased Descriptor Matcher";

	Log::notice("Descriptor Matcher set to FlannBased");
}

FeatureHandler::~FeatureHandler()
{
	delete _feature_detector;
	delete _descriptor_extractor;
	delete _descriptor_matcher;
}
