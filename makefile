CC=			g++
CFLAGS=		-c -g -O0 -Wall `pkg-config --cflags opencv`
LDFLAGS=	`pkg-config --libs opencv`
SOURCES=	Common/Image.cpp \
			Features/FeatureHandler.cpp Features/TrackFeature.cpp \
			VideoTrack.cpp \
			GeocodeMonuments.cpp MovieIdentifier.cpp \
			main.cpp
OBJECTS=	$(SOURCES:.cpp=.o)
EXECUTABLE=	imageMatching

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)