#include "Common/Common.h"
#include "Common/Log.h"
#include "Common/Image.h"
#include "Features/FeatureHandler.h"
#include "VideoTrack.h"
#include "GeocodeMonuments.h"
#include "MovieIdentifier.hpp"

#include <opencv2/imgproc/imgproc.hpp>

bool Log::debug = false;

int main( int argc, char** argv ){
//	GeocodeMonuments* gc = new GeocodeMonuments( DET_SURF, DES_SIFT, DESM_FB);
//	gc->setShowMatches(true);
//
//	gc->setMonument( "Bom Jesus", "./monuments/bom_jesus.jpg" );
//	gc->setMonument( "Sameiro", "./monuments/sameiro.jpg" );
//
//	gc->testDir( "./monuments/tests" );

	MovieIdentifier* mv = new MovieIdentifier( DET_SURF, DES_SIFT, DESM_FB );
	mv->setShowMatches(true);

	mv->setMovie( "The Grey", "./movies/grey.jpg" );
	mv->setMovie( "The Silence of the Lambs", "./movies/lambs.jpg" );
	mv->setMovie( "The Dark Knight", "./movies/batman.jpg" );

	mv->testDir( "./movies/tests" );

//	VideoTrack* vt = new VideoTrack( "track" );
//	vt->trackFeatures( DET_FAST, DES_SURF, DESM_FB, 0.65f );
//	vt->printFeaturesInTrack();

//	VideoTrack* vt = new VideoTrack( "videothumbs" );
//	vt->filterImagesByFeatures( DET_FAST, DES_SURF, DESM_FB, 0.65f, 0.05f );
//	vt->printRejectedImages();

//	VideoTrack* vt = new VideoTrack( "videothumbs" );
//	vt->filterImagesByHistograms(4, CV_COMP_CORREL, 0.92f);
//	vt->printRejectedImages();

//	VideoTrack* vt = new VideoTrack( "videothumbs" );
//	vt->filterImagesByHistograms(3, CV_COMP_INTERSECT, 610000.0f);
//	vt->printRejectedImages();

//	VideoTrack* vt = new VideoTrack( "videothumbs" );
//	vt->filterImagesByHistograms(3, CV_COMP_CHISQR, 93000.0f);
//	vt->printRejectedImages();

//	Image* img1 = new Image( "videothumbs/img0001.jpeg", 0.20f, 0.10f, 0.0f, 0.35f );
//	img1->show("image");
//	img1->showROI("roi");
	//Image* img2 = new Image( "videothumbs/img0002.jpeg", 0.10f, 0.0f, 0.60f, 0.0f );
//
//	img1->calcHistogram(3);
//	img2->calcHistogram(3);
//
//	img1->compareHistograms( img2, CV_COMP_CHISQR );
//	img1->printCompareHistograms( img2 );
//
//	img1->showHistogramAsImage();

//
//	FeatureHandler* fd = new FeatureHandler( DET_FAST, DES_SURF, DESM_FB );
//
//	fd->detect( img1 );
//	fd->computeDescriptors( img1 );
//	fd->detect( img2 );
//	fd->computeDescriptors( img2 );
//
//	fd->matchDescriptors( img1, img2 );
//	fd->findGoodMatches( img1, img2 );
//
//	cv::Mat resultin;
//	cv::Mat resultgd;
//	img1->drawMatches( img2, 60, MA_IN, resultin, cv::Scalar( 0, 255, 0 ) );
//	img1->drawMatches( img2, 60, MA_GD, resultgd, cv::Scalar( 0, 255, 0 ) );
//
//	cv::namedWindow("Image initial matches", CV_WINDOW_NORMAL);
//	cv::namedWindow("Image good matches", CV_WINDOW_NORMAL);
//
//	cv::imshow("Image initial matches", resultin);
//	cv::imshow("Image good matches", resultgd);
//
	cv::waitKey();

	return 0;
}
