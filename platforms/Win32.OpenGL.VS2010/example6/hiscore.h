// hiscore.h - Generic high score table
// James Higgs 2006

// hiscore structure
typedef struct {
	char name[20];
	int score;
	int level;
	int stage;
} HI_SCORE;

extern void HiScore_Init(void);
extern void HiScore_Load(void);
extern void HiScore_Save(void);
extern bool HiScore_Check(int newscore);
extern void HiScore_Add(int newscore, char *newname, int newlevel, int newstage);
extern int HiScore_GetScore(int index);
extern char *HiScore_GetName(int index);
extern int HiScore_GetLevel(int index);
extern int HiScore_GetStage(int index);
