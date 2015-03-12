# Image Features & Feature Matching #

This project has the goal of introducing and exploring the different feature detectors, descriptors, and matchers available in OpenCV. A simple framework is provided to handle feature detection and matching and several sample applications using image features are then presented.

## Dependencies ##
* OpenCV 2.4.9
* libconfig++

## Build ##

Simply run make on the base directory to build the sample applications.

## Available Applications ##

Several sample applications are made available in order to explore possible use cases using image features in OpenCV.  

### Simple Tests ###

Presents some of the main functionalities of the framework.

### Object Detect ###

Identifies an object and detects it across several images. Based on the tutorial available in the following [site](http://docs.opencv.org/doc/tutorials/features2d/feature_homography/feature_homography.html).

Run the program with the following command:

    objectDetect <path to object image> <path to test dir>
    (i.e.) objectDetect ./data/box.png ./data/objectInScene

### Filter Video ###

Application to identify the frames from a video stream where there is insignificant changes in the scene represented. Possibly useful as a preemptive step to eliminate frames from further processing in a more complex program.

Explore the applicability of image features against other image characteristics.

### Entity Identifier ###

Program to identify the entity represented in an image. First, image templates are defined as representatives of each of the entities considered, then, a test set is matched against the templates.

Two use cases are provided:

    * Identify monuments across different views (entityIdentifier data/findMonuments.cfg);
    * Identify movies from different poster representations (entityIdentifier data/findMovies.cfg).

Further work should be explored in the decision process for identifying each match and in the efficiency of the overall process. As it is it can not scale to a real world database of entities. (See ["Scalable Recognition with a Vocabulary Tree"](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=1641018&tag=1) for a possibility of a scalable image matching algorithm.)
