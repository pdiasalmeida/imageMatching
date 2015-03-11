CC=			g++
CFLAGS=		-c -g -O0 -Wall `pkg-config --cflags opencv`
LDFLAGS=	`pkg-config --libs opencv`

SOURCESCOM=	Common/Image.cpp Features/FeatureHandler.cpp
SOURCESTST=	SimpleTests.cpp
SOURCESGEO=	MonumentIdentifier.cpp
SOURCESMOV=	MovieIdentifier.cpp
SOURCESFVD=	FilterVideoThumbs.cpp

OBJECTSCOM=	$(SOURCESCOM:.cpp=.o)
OBJECTSTST=	$(SOURCESTST:.cpp=.o)
OBJECTSGEO=	$(SOURCESGEO:.cpp=.o)
OBJECTSMOV=	$(SOURCESMOV:.cpp=.o)
OBJECTSFVD=	$(SOURCESFVD:.cpp=.o)

all: simpleTests monumentIdentifier movieIdentifier filterVideo
	
simpleTests: $(OBJECTSCOM) $(OBJECTSTST)
	$(CC) $(OBJECTSCOM) $(OBJECTSTST) -o $@ $(LDFLAGS)

monumentIdentifier: $(OBJECTSCOM) $(OBJECTSGEO)
	$(CC) $(OBJECTSCOM) $(OBJECTSGEO) -o $@ $(LDFLAGS)

movieIdentifier: $(OBJECTSCOM) $(OBJECTSMOV)
	$(CC) $(OBJECTSCOM) $(OBJECTSMOV) -o $@ $(LDFLAGS)

filterVideo: $(OBJECTSCOM) $(OBJECTSFVD)
	$(CC) $(OBJECTSCOM) $(OBJECTSFVD) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTSCOM) $(OBJECTSTST) $(OBJECTSGEO) $(OBJECTSMOV) $(OBJECTSFVD) \
	simpleTests monumentIdentifier movieIdentifier filterVideo
