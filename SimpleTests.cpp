#include "Common/Common.h"
#include "Common/Log.h"
#include "Common/Image.h"
#include "Features/FeatureHandler.h"

#include <opencv2/imgproc/imgproc.hpp>

bool Log::debug = true;

int main( int argc, char** argv )
{
	/**-------------------------- Image entity and image roi --------------------------------------*/
	Image* img1 = new Image( "data/videothumbs/img0001.jpeg", 0.20f, 0.10f, 0.0f, 0.35f );
	img1->show("image");
	img1->showROI("roi");
	cv::waitKey(0);
	cv::destroyAllWindows();

	/**-------------------------- Calculate and compare histograms --------------------------------------*/
	Image* img2 = new Image( "data/videothumbs/img0002.jpeg", 0.10f, 0.0f, 0.60f, 0.0f );

	int imageSections = 3;

	img1->calcHistogram(imageSections);
	img2->calcHistogram(imageSections);
	img1->compareHistograms( img2, CV_COMP_CHISQR );
	img1->printCompareHistograms( img2 );

	img1->showHistogramAsImage();

	cv::waitKey(0);
	cv::destroyAllWindows();

	/**-------------------------- Compute features and match images --------------------------------------*/
	FeatureHandler* fd = new FeatureHandler( DET_FAST, DES_SURF, DESM_FB );

	fd->detect( img1 );
	fd->computeDescriptors( img1 );
	fd->detect( img2 );
	fd->computeDescriptors( img2 );

	float ratio = 0.85f;

	fd->matchDescriptors( img1, img2 );
	fd->findGoodMatches( img1, img2, ratio );

	int numberOfMatches = 60;
	cv::Scalar color = cv::Scalar( 0, 255, 0 );

	cv::Mat resultin;
	cv::Mat resultgd;
	img1->drawMatches( img2, numberOfMatches, MA_IN, resultin, color );
	img1->drawMatches( img2, numberOfMatches, MA_GD, resultgd, color );

	cv::namedWindow("Image initial matches", CV_WINDOW_NORMAL);
	cv::namedWindow("Image good matches", CV_WINDOW_NORMAL);

	cv::imshow("Image initial matches", resultin);
	cv::imshow("Image good matches", resultgd);

	cv::waitKey(0);
}
