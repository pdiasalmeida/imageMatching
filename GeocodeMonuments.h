#ifndef GEOCODEMONUMENTS_H_
#define GEOCODEMONUMENTS_H_

#include "Common/Image.h"
#include "Features/FeatureHandler.h"

#include <string>
#include <set>
#include <utility>

class GeocodeMonuments {
public:
	GeocodeMonuments( Detector detID, DescriptorExtractor desID, DescriptorMatcher dmID );

	void setMonument( std::string name, std::string imagePath );
	void findMonument( std::string imagePath );

	void setShowMatches(bool value);

	void testDir( std::string path );

	virtual ~GeocodeMonuments();

protected:
	std::set< std::string > _files;
	std::vector< std::pair< std::string, Image*  > > _monuments;

	FeatureHandler* _fH;
	double _ratio;
	int _nMatchesThresh;
	bool _showMatches;

private:
	Image* processImage( std::string path );
	void getFileList(std::string path);
	void printFileNames();
};

#endif /* GEOCODEMONUMENTS_H_ */
