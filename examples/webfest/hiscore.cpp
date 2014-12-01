// hiscore.cpp

//#include <windows.h>		// Header File For Windows
#include <string.h>
#include <stdio.h>			// Header File For Standard Input/Output
#include "hiscore.h"

HiScore::HiScore()
: score(0),
	level(0),
	stage(0)
{
	strcpy_s(name, 20, "AAA");
}

HiScoreTable::HiScoreTable()
{
	// HiScore contructor should have initialised these?
	//for(int i=0; i < NUM_HIGH_SCORES; i++)
	//	{
	//	m_hiScore[i].score = 0;
	//	m_hiScore[i].level = 0;
	//	m_hiScore[i].stage = 0;
	//	}
}


//HiScoreTable::~HiScoreTable()
//{
//
//}

// load high scores from disk
void HiScoreTable::Load()
{
	FILE *pf;

	pf = fopen("hiscores.dat", "rb");
	if(pf) {
		fread(&m_hiScore[0], NUM_HIGH_SCORES, sizeof(HiScore), pf);
		fclose(pf);
	}
}

// save high scores to disk
void HiScoreTable::Save()
{
	FILE *pf;

	pf = fopen("hiscores.dat", "wb");
	if(pf) {
		fwrite(&m_hiScore[0], NUM_HIGH_SCORES, sizeof(HiScore), pf);
		fclose(pf);
	}
}

// check if this score should be in high score table
bool HiScoreTable::Check(int newscore)
{
	int i;

	for(i = 0; i < NUM_HIGH_SCORES; i++) {
		if(newscore > m_hiScore[i].score)
			return true;
	}

	return false;
}

// add this score to the high scores table
void HiScoreTable::Add(int newscore, char *newname, int newlevel, int newstage)
{
	int i, pos;

	pos = 11;	// not in table
	for(i = NUM_HIGH_SCORES - 1; i >= 0; i--) {
		if(newscore > m_hiScore[i].score)
			pos = i;
	}

	// put in table, shift others down
	if(pos < NUM_HIGH_SCORES) {
		// shift lower scores down
		for(i=9; i>pos; i--)
			{
			//memcpy(&hiscores[i], &hiscores[i-1], sizeof(HI_SCORE));
			m_hiScore[i] = m_hiScore[i-1];
			}
		// add new high score
		m_hiScore[pos].score = newscore;
		m_hiScore[pos].level = newlevel;
		m_hiScore[pos].stage = newstage;
		strcpy_s(m_hiScore[pos].name, 20, newname);
	}
}

int HiScoreTable::GetScore(int index)
{
	return m_hiScore[index].score;
}

char *HiScoreTable::GetName(int index)
{
	return m_hiScore[index].name;
}

int HiScoreTable::GetLevel(int index)
{
	return m_hiScore[index].level;
}

int HiScoreTable::GetStage(int index)
{
	return m_hiScore[index].stage;
}

/* OLD

HI_SCORE hiscores[10];

void HiScore_Init(void)
{
	//memset(&hiscores[0], 0, sizeof(HI_SCORE) * 10);
	for(int i=0; i < 10; i++)
		{
		strcpy(hiscores[i].name, "");
		hiscores[i].score = 0;
		hiscores[i].level = 0;
		hiscores[i].stage = 0;
		}
}

// load high scores from disk
void HiScore_Load(void)
{
	FILE *pf;

	pf = fopen("hiscores.dat", "rb");
	if(pf) {
		fread(&hiscores[0], 10, sizeof(HI_SCORE), pf);
		fclose(pf);
	}

}

// save high scores to disk
void HiScore_Save(void)
{
	FILE *pf;

	pf = fopen("hiscores.dat", "wb");
	if(pf) {
		fwrite(&hiscores[0], 10, sizeof(HI_SCORE), pf);
		fclose(pf);
	}


}

// check if this score should be in high score table
bool HiScore_Check(int newscore)
{
	int i;

	for(i = 0; i<10; i++) {
		if(newscore > hiscores[i].score)
			return true;
	}

	return false;
}

// add this score to the high scores table
void HiScore_Add(int newscore, char *newname, int newlevel, int newstage)
{
	int i, pos;

	pos = 11;	// not in table
	for(i = 9; i>=0; i--) {
		if(newscore > hiscores[i].score)
			pos = i;
	}

	// put in table, shift others down
	if(pos < 10) {
		// shift lower scores down
		for(i=9; i>pos; i--)
			{
			//memcpy(&hiscores[i], &hiscores[i-1], sizeof(HI_SCORE));
			strcpy(hiscores[i].name, hiscores[i-1].name);
			hiscores[i].score = hiscores[i-1].score;
			hiscores[i].level = hiscores[i-1].level;
			hiscores[i].stage = hiscores[i-1].stage;
			}
		// add new high score
		hiscores[pos].score = newscore;
		hiscores[pos].level = newlevel;
		hiscores[pos].stage = newstage;
		strcpy(hiscores[pos].name, newname);
	}

}

int HiScore_GetScore(int index)
{
	return hiscores[index].score;
}

char *HiScore_GetName(int index)
{
	return hiscores[index].name;
}

int HiScore_GetLevel(int index)
{
	return hiscores[index].level;
}

int HiScore_GetStage(int index)
{
	return hiscores[index].stage;
}
*/