/// Enemies per level, and enemy sequencing per level
//
// 1. Defines the total number of each enemy to spawn per level
// 2. Also defines enemy "wave" or "group" sequencing
// (Note: The two are separate)

// Number of each enemy to spawn per level
typedef struct {
	int numFlippers;
	int numTankers;
	int numSpikers;
	int numFuseballs;
	int numZappers;
	int numBarriers;
	int numUfos;
	int numAsteroids;
} ENEMYSPAWNDATA;

typedef struct {
	int enemyId;
	int spawnTimeRangeStart;			// earliest time at which to spawn
	int spawnTimeRangeEnd;				// latest time at which to spawn
} ENEMYSEQDATA;

#define NUM_LEVELS 20

ENEMYSPAWNDATA enemySpawnData[NUM_LEVELS]= {
	//Flip  Tank  Spike Fuse  Zapp  Barr  Ufo   Ast   
	{ 10,   0,    0,    0,    0,    0,    0,    0   },	// level 1
	{ 20,   0,    0,    0,    0,    0,    0,    0   },	// level 2
	{ 0,    10,   0,    0,    0,    0,    0,    0   },	// level 3
	{ 12,   8,    0,    0,    0,    0,    1,    0   },	// level 4
	{ 0,    5,    5,    0,    0,    0,    0,    0   },	// level 5    spikers appear
	{ 12,   0,    5,    0,    0,    0,    0,    0   },	// level 6
	{ 12,   8,    5,    0,    0,    0,    0,    0   },	// level 7
	{ 12,   0,    5,    0,    0,    0,    1,    20  },	// level 8
	{ 20,   0,    0,    0,    0,    0,    0,    10  },	// level 9
	{ 10,   5,    0,    0,    1,    0,    0,    5   },	// level 10   zappers appear
	{ 12,   5,    0,    1,    2,    0,    0,    5   },	// level 11
	{ 12,   0,    0,    0,    3,    1,    1,    0   },	// level 12   barriers appear
	{ 12,   0,   10,    1,    4,    2,    0,    0   },	// level 13
	{ 12,   5,   10,    0,    5,    3,    0,    0   },	// level 14
	{ 12,   5,   10,    1,    5,    4,    0,    0   },	// level 15
	{ 10,  10,   10,    0,    5,    0,    1,    0   },	// level 16
	{ 20,   0,    0,    1,    5,    2,    0,    0   },	// level 17
	{ 12,   8,    4,    0,    5,    2,    4,    0   },	// level 18
	{ 4 ,  10,    4,    1,    5,    2,    0,    0   },	// level 19
	{ 10,  10,   10,    2,    5,    2,    1,    0   }	// level 20
};
