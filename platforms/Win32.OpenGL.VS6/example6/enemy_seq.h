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

ENEMYSPAWNDATA enemySpawnData[100]= {
	//Flip  Tank  Spike Fuse  Zapp  Barr  Ufo   Ast   
	{ 12,   0,    0,    0,    0,    0,    0,    0   },	// level 1
	{ 20,   0,    0,    0,    0,    0,    0,    0   },	// level 2
	{ 0,    10,   0,    0,    0,    0,    0,    0   },	// level 3
	{ 12,   8,    0,    0,    0,    0,    0,    0   },	// level 4
	{ 0,    0,    10,   0,    0,    0,    0,    0   },	// level 5
	{ 12,   0,    8,    0,    0,    0,    0,    0   },	// level 6
	{ 12,   8,    8,    0,    0,    0,    0,    0   },	// level 7
	{ 12,   0,    0,    0,    0,    0,    0,    20  },	// level 8
	{ 12,   5,    5,    0,    0,    0,    0,    10  },	// level 9
	{ 12,   0,    0,    0,    0,    0,    0,    5   },	// level 10
	{ 12,   0,    0,    1,    0,    0,    0,    5   },	// level 11
	{ 12,   0,    0,    0,    0,    0,    0,    0   },	// level 12
	{ 12,   0,    0,    1,    0,    0,    0,    0   },	// level 13
	{ 12,   0,    0,    0,    0,    0,    0,    0   },	// level 14
	{ 12,   0,    0,    1,    0,    0,    0,    0   },	// level 15
	{ 12,   0,    0,    0,    0,    0,    0,    0   },	// level 16
	{ 12,   0,    0,    1,    0,    0,    0,    0   },	// level 17
	{ 12,   0,    0,    0,    0,    0,    0,    0   },	// level 18
	{ 12,   0,    0,    1,    0,    0,    0,    0   },	// level 19
	{ 12,   0,    0,    0,    0,    0,    0,    0   },	// level 20
	{ 12,   0,    0,    2,    0,    0,    0,    0   },	// level ?
	{ 12,   0,    0,    0,    0,    0,    0,    0   },	// level ?
	{ 12,   0,    0,    2,    0,    0,    0,    0   },	// level ?
	{ 12,   0,    0,    0,    0,    0,    0,    0   },	// level ?
	{ 12,   0,    0,    2,    0,    0,    0,    0   }		// level ?
};
