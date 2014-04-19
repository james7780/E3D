// Tempest common data

// structure to hold structure data for a single web
typedef struct {
	int numVertices;
	bool closed;
	int startingSegment;
	E3D::Vector vertex[50];
}TUBEDATA;

// score value for each enemy (TODO: fix values)
#define FLIPPER_VALUE		100
#define TANKER_VALUE		100
#define SPIKER_VALUE		100
#define FUSEBALL_VALUE		100
#define ZAPPER_VALUE		100
#define UFO_VALUE			100
#define ASTEROID_VALUE		100

// These must be implemented by the host platform (in loadweb.cpp)
extern int GetWebFilename(int webIndex, char *filename);
extern int LoadWeb(char *filename, TUBEDATA *tube);
