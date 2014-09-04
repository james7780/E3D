// Tempest common definitions and data
#define WEBFEST_VERSION "1.2"

#define PI 3.141592653589793f

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// General
#define TUBE_LENGTH		160.0f
#define STEP_SIZE		6.0f
#define HIT_RADIUS		2.5f		// must be slightly less than half STEP_SIZE!
#define CHARGE_FULL		100

// score value for each enemy (TODO: fix values)
#define FLIPPER_VALUE		100
#define TANKER_VALUE		200
#define SPIKER_VALUE		50
#define FUSEBALL_VALUE		100
#define ZAPPER_VALUE		100
#define UFO_VALUE			1000
#define ASTEROID_VALUE		100

// object list defines
enum {
	PLAYER_LIST	= 0,
	BULLET_LIST = 1,
	ENEMY_LIST	= 2,
	FRAG_LIST	= 3,
	STAR_LIST	= 4
};

// model map
// NB: Models must be added to the scene in this order:
enum {
	TUBE_MODEL		= 0,
	PLAYER1_MODEL	= 1,
	PLAYER2_MODEL	= 2,
	BULLET_MODEL	= 3,
	FRAG_MODEL		= 4,
	FLIPPER_MODEL	= 5,
	TANKER_MODEL	= 6,
	EBULLET_MODEL	= 7,
	SPIKE1_MODEL	= 8,
	SPIKE2_MODEL	= 9,
	BARRIER_MODEL	= 10,
	UFO_MODEL		= 11,
	ASTEROID_MODEL	= 12
};

// object behaviour map
enum {
	BEHAVE_NONE		= 0,				// object inactive/off
	BEHAVE_CIRCLE	= 1,				// object circling/wating for launch onto web
	BEHAVE_NORMAL	= 2,				// object's normal behaviour
	BEHAVE_ALT		= 3,				// object's alternative behaviour
	BEHAVE_RETREAT	= 4					// enemy retreating
};

// structure to hold structure data for a single web
#define TUBE_NAME_LENGTH	64
typedef struct {
	char name[TUBE_NAME_LENGTH];
	int numVertices;
	bool closed;
	int startingSegment;
	E3D::Vector vertex[50];
} TUBEDATA;

// Audio Sample Map
enum {
	SAMPLE_1UP,
	SAMPLE_BOMB,
	SAMPLE_BLEEP,
	SAMPLE_PLAYERFIRE,
	SAMPLE_PLAYEREXPLODE,
	SAMPLE_PLAYERCAUGHT,
	SAMPLE_ENEMYEXPLODE,
	SAMPLE_TANKERSPAWN,
	SAMPLE_SPIKEHIT,
	SAMPLE_FLIPPERFLIP,
	SAMPLE_ZAPPERZAP,
	SAMPLE_ENEMYFIRE,
	SAMPLE_UFOFIRE,
	SAMPLE_LEVELCOMPLETE,
	SAMPLE_LEVELSTART,
	SAMPLE_ENEMYONWEB,
	SAMPLE_ENEMYONRIM,
	SAMPLE_CHARGEFULL,
	SAMPLE_BARRIERHIT,
	SAMPLE_MAX
};

// These must be implemented by the host platform (in loadweb.cpp)
extern int GetWebFilename(int webIndex, char *filename);
extern int LoadWeb(char *filename, TUBEDATA *tube);

// In behaviours.cpp
extern void DoBehaviours(TUBEDATA *tube, unsigned int level);
extern void AddFrags(Vector p, Vector v, int count);

// In webfest.cpp
extern E3D_Scene Scene;
extern void AddScore(unsigned int value);
extern void AddCharge(int value);
extern void WFPlaySound(int channel, int sampleIndex);



