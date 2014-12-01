// hiscore.h - Generic high score table
// James Higgs 2006

#define NUM_HIGH_SCORES	10

/// A high-score table entry
class HiScore
{
public:
	HiScore();

	char name[20];
	int score;
	int level;
	int stage;
};

class HiScoreTable
{
public:
	HiScoreTable();

	//void HiScore_Init(void);
	void Load();
	void Save();
	bool Check(int newscore);
	void Add(int newscore, char *newname, int newlevel, int newstage);
	int GetScore(int index);
	char *GetName(int index);
	int GetLevel(int index);
	int GetStage(int index);

	// Atrributes
	HiScore m_hiScore[10];
};


/* OLD code
// hiscore structure
typedef struct {

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
*/