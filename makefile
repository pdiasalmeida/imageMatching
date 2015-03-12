CC=			g++
CFLAGS=		-c -g -O0 -Wall `pkg-config --cflags opencv libconfig++`
LDFLAGS=	`pkg-config --libs opencv libconfig++`

SOURCESCOM=	Common/Image.cpp Features/FeatureHandler.cpp
SOURCESTST=	SimpleTests.cpp
SOURCESCNI=	ContextIdentifier.cpp
SOURCESFVD=	FilterVideoThumbs.cpp

OBJECTSCOM=	$(SOURCESCOM:.cpp=.o)
OBJECTSTST=	$(SOURCESTST:.cpp=.o)
OBJECTSCNI=	$(SOURCESCNI:.cpp=.o)
OBJECTSFVD=	$(SOURCESFVD:.cpp=.o)

all: simpleTests contextIdentifier filterVideo
	
simpleTests: $(OBJECTSCOM) $(OBJECTSTST)
	$(CC) $(OBJECTSCOM) $(OBJECTSTST) -o $@ $(LDFLAGS)

contextIdentifier: $(OBJECTSCOM) $(OBJECTSCNI)
	$(CC) $(OBJECTSCOM) $(OBJECTSCNI) -o $@ $(LDFLAGS)

filterVideo: $(OBJECTSCOM) $(OBJECTSFVD)
	$(CC) $(OBJECTSCOM) $(OBJECTSFVD) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTSCOM) $(OBJECTSTST) $(OBJECTSCNI) $(OBJECTSFVD) \
	simpleTests contextIdentifier filterVideo
