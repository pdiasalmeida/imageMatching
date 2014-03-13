CC=g++
CFLAGS=-c -g -O0 -Wall
LDFLAGS=-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_nonfree -lopencv_flann
SOURCES=Common/Image.cpp Features/FeatureHandler.cpp Features/TrackFeature.cpp VideoTrack.cpp GeocodeMonuments.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=imageMatching

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)