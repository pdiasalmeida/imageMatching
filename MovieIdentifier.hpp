#ifndef MOVIEIDENTIFIER_HPP_
#define MOVIEIDENTIFIER_HPP_

#include "Common/Image.h"
#include "Features/FeatureHandler.h"

#include <string>
#include <set>
#include <utility>

class MovieIdentifier {
public:
	MovieIdentifier( Detector detID, DescriptorExtractor desID, DescriptorMatcher dmID );

		void setMovie( std::string name, std::string imagePath );
		void findMovie( std::string imagePath );

		void testDir( std::string path );

		virtual ~MovieIdentifier();

	protected:
		std::set< std::string > _files;
		std::vector< std::pair< std::string, Image*  > > _monuments;

		FeatureHandler* _fH;
		double _ratio;
		int _nMatchesThresh;

	private:
		Image* processImage( std::string path );
		void getFileList(std::string path);
		void printFileNames();
};

#endif /* MOVIEIDENTIFIER_HPP_ */
