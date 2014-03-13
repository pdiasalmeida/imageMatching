#ifndef VIDEOTRACK_HPP_
#define VIDEOTRACK_HPP_

#include "Features/FeatureHandler.h"
#include "Features/TrackFeature.h"
#include "Common/Log.h"

#include <string>
#include <set>
#include <map>

class VideoTrack {
public:
	VideoTrack( std::string path );
	~VideoTrack();

	void trackFeatures( Detector detID, DescriptorExtractor desID, DescriptorMatcher dmID, float ratio );
	void filterImagesByFeatures( Detector detID, DescriptorExtractor desID, DescriptorMatcher dmID,
			float ratio, float threshold );
	void filterImagesByHistograms( int sections, int compareMethod, float threshold );

	void printFeaturesInTrack();
	void printFileNames();
	void printRejectedImages();

protected:
	std::string _path;
	std::set< std::string > _files;
	std::set< Image* > _images;
	std::map< std::string, float > _rejectedimages;

	std::vector< TrackFeature* > _featuresInTrack;

private:
	void getFileList();
	void initImages();

	void proccessImagesFeatures( FeatureHandler* featureHandler );
	void calculateHistograms( int sections );

};

#endif
