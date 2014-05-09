// hiscore.cpp

//#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File For Standard Input/Output
#include <string.h>
#include "hiscore.h"

HI_SCORE hiscores[10];

void HiScore_Init(void)
{
	memset(&hiscores[0], 0, sizeof(HI_SCORE) * 10);
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
			memcpy(&hiscores[i], &hiscores[i-1], sizeof(HI_SCORE));
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
