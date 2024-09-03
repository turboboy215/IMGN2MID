/*Absolute/Imagineering (GB/GBC) to MIDI converter*/
/*By Will Trowbridge*/
/*Portions based on code by ValleyBell*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define bankSize 16384

FILE* rom, * txt;
long bank;
long offset;
long tablePtrLoc;
long tableOffset;
long patOffset;
long tempoLoc;
long tempoOffset;
int i, j;
char outfile[1000000];
int format = 0;
int cmdFmt = 0;
int songNum;
long songPtrs[4];
long patPtrs[4];
long patList[4][50];
long bankAmt;
int seqDiff = 0;
int foundTable = 0;
int foundTempoTable = 0;
int songTempo = 0;
int highestSeq = 0;

unsigned long seqList[500];
unsigned static char* romData;
int totalSeqs;

const char TableFind[5] = { 0x0E, 0x08, 0x2A, 0x12 };
const char FindTempo[5] = { 0x22, 0x22, 0x22, 0x77, 0xEA };

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
static void Write8B(unsigned char* buffer, unsigned int value);
static void WriteBE32(unsigned char* buffer, unsigned long value);
static void WriteBE24(unsigned char* buffer, unsigned long value);
static void WriteBE16(unsigned char* buffer, unsigned int value);
void song2txt(int songNum, long ptrs[4]);
void seqs2txt(unsigned long list[]);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
}

static void Write8B(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = value;
}

static void WriteBE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF000000) >> 24;
	buffer[0x01] = (value & 0x00FF0000) >> 16;
	buffer[0x02] = (value & 0x0000FF00) >> 8;
	buffer[0x03] = (value & 0x000000FF) >> 0;

	return;
}

static void WriteBE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF0000) >> 16;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0x0000FF) >> 0;

	return;
}

static void WriteBE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0xFF00) >> 8;
	buffer[0x01] = (value & 0x00FF) >> 0;

	return;
}

int main(int args, char* argv[])
{
	printf("Absolute/Imagineering (GB/GBC) to TXT converter\n");
	if (args != 3)
	{
		printf("Usage: IMGN2TXT <rom> <bank>\n");
		return -1;
	}
	else
	{
		if ((rom = fopen(argv[1], "rb")) == NULL)
		{
			printf("ERROR: Unable to open file %s!\n", argv[1]);
			exit(1);
		}
		else
		{
			if ((rom = fopen(argv[1], "rb")) == NULL)
			{
				printf("ERROR: Unable to open file %s!\n", argv[1]);
				exit(1);
			}
			else
			{
				bank = strtol(argv[2], NULL, 16);
				if (bank != 1)
				{
					bankAmt = bankSize;
				}
				else
				{
					bankAmt = 0;
				}
			}
			if (bank != 1)
			{
				fseek(rom, ((bank - 1) * bankSize), SEEK_SET);
				romData = (unsigned char*)malloc(bankSize);
				fread(romData, 1, bankSize, rom);
				fclose(rom);
			}
			else
			{
				/*Banks 1 and 8 are used together for Berlitz Translator series*/
				fseek(rom, 0, SEEK_SET);
				romData = (unsigned char*)malloc(bankSize * 2);
				fread(romData, 1, bankSize, rom);
				fseek(rom, (7 * bankSize), SEEK_SET);
				fread(romData + bankSize, 1, bankSize, rom);
				fclose(rom);
			}


			/*Try to search the bank for song pattern table loader*/
			for (i = 0; i < bankSize; i++)
			{
				if ((!memcmp(&romData[i], TableFind, 4)) && foundTable == 0 && romData[i - 3] == 0x11)
				{
					tablePtrLoc = bankAmt + i - 6;
					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = ReadLE16(&romData[tablePtrLoc - bankAmt]);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					foundTable = 1;
					break;
				}
			}

			/*Now find the tempos*/
			for (i = 0; i < bankSize; i++)
			{
				if ((!memcmp(&romData[i], FindTempo, 5)) && foundTempoTable == 0)
				{
					tempoLoc = bankAmt + i + 20;
					printf("Found pointer to song tempo table at address 0x%04x!\n", tempoLoc);
					tempoOffset = ReadLE16(&romData[tempoLoc - bankAmt]);
					printf("Song tempo table starts at 0x%04x...\n", tempoOffset);
					foundTempoTable = 1;
					break;
				}
			}
		}

		if (foundTable == 1 && foundTempoTable == 1)
		{
			i = tableOffset + 8 - bankAmt;
			j = tempoOffset + 1 - bankAmt;
			songNum = 1;
			while ((ReadLE16(&romData[i]) > 0x100 && ReadLE16(&romData[i]) < bankSize * 2) && (ReadLE16(&romData[i + 2]) > 0x100 && ReadLE16(&romData[i + 2]) < bankSize * 2) && (ReadLE16(&romData[i + 4]) > 0x100 && ReadLE16(&romData[i + 4]) < bankSize * 2) && (ReadLE16(&romData[i + 6]) > 0x100 && ReadLE16(&romData[i + 6]) < bankSize * 2))
			{
				songPtrs[0] = ReadLE16(&romData[i]);
				printf("Song %i channel 1: 0x%04X\n", songNum, songPtrs[0]);
				songPtrs[1] = ReadLE16(&romData[i + 2]);
				printf("Song %i channel 2: 0x%04X\n", songNum, songPtrs[1]);
				songPtrs[2] = ReadLE16(&romData[i + 4]);
				printf("Song %i channel 3: 0x%04X\n", songNum, songPtrs[2]);
				songPtrs[3] = ReadLE16(&romData[i + 6]);
				printf("Song %i channel 4: 0x%04X\n", songNum, songPtrs[3]);
				songTempo = romData[j];
				printf("Song %i tempo: %i\n", songNum, songTempo);
				song2txt(songNum, songPtrs);
				i += 8;
				j++;
				songNum++;
			}
			seqs2txt(seqList);
		}
		else
		{
			printf("ERROR: Magic bytes not found!\n");
			exit(-1);
		}
	}
}

void song2txt(int songNum, long ptrs[4])
{
	int curTrack = 0;
	long romPos = 0;
	long patPos = 0;
	long patLoc = 0;
	long patLoop = 0;
	int numPats = 0;
	long curSeq = 0;
	long command[3];
	int k = 0;
	int l = 0;

	sprintf(outfile, "song%d.txt", songNum);
	if ((txt = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file song%d.txt!\n", songNum);
		exit(2);
	}
	else
	{
		/*First, get pattern locations*/
		for (curTrack == 0; curTrack < 4; curTrack++)
		{
			numPats = 0;
			fprintf(txt, "Channel %i:\n", curTrack + 1);
			k = ptrs[curTrack] - bankAmt;
			while (ReadLE16(&romData[k]) != 0x0000 && ReadLE16(&romData[k]) != 0x00FF && ReadLE16(&romData[k]) != 0x0001 && ReadLE16(&romData[k]) != 0x0002 && ReadLE16(&romData[k]) != 0x0003)
			{
				patList[curTrack][numPats] = ReadLE16(&romData[k]);
				fprintf(txt, "Pattern %i: 0x%04X\n", numPats + 1, patList[curTrack][numPats]);
				k += 2;
				numPats++;
			}

			patLoop = ReadLE16(&romData[k]);

			fprintf(txt, "Loop: ");
			if (patLoop == 0x0000)
			{
				fprintf(txt, "Normal loop mode\n");
			}
			else if (patLoop == 0x00FF)
			{
				fprintf(txt, "No loop\n");
			}
			else if (patLoop == 0x0001)
			{
				fprintf(txt, "Loop 1\n");
			}
			else if (patLoop == 0x0002)
			{
				fprintf(txt, "Loop 2\n");
			}
			else if (patLoop == 0x0003)
			{
				fprintf(txt, "Loop 3\n");
			}
			else
			{
				fprintf(txt, "0x%04X\n", patLoop);
			}

			fprintf(txt, "\n");

			/*Now, the actual patterns*/
			for (k = 0; k < numPats; k++)
			{
				romPos = patList[curTrack][k] - bankAmt;
				fprintf(txt, "Pattern %i:\n", k + 1);
				patLoc = ReadLE16(&romData[romPos]);
				while (patLoc != 0x0000 && patLoc > 0x1000)
				{
					patLoc = ReadLE16(&romData[romPos]);
					if (patLoc != 0x0000)
					{
						fprintf(txt, "0x%04X\n", patLoc);
						curSeq = patLoc;
						for (j = 0; j < 500; j++)
						{
							if (seqList[j] == curSeq)
							{
								break;
							}
						}
						if (j == 500)
						{
							seqList[highestSeq] = curSeq;
							highestSeq++;
						}
					}
					romPos += 2;
				}
				fprintf(txt, "\n");

			}
		}
		fclose(txt);
	}
}

void seqs2txt(unsigned long list[])
{
	int seqPos = 0;
	int songEnd = 0;
	int lowestSeq = 0;
	int curSeq = 0;

	long command[3];
	unsigned char lowNibble = 0;
	unsigned char highNibble = 0;
	int curChan = 0;
	int endSeq = 0;
	int endChan = 0;
	int curTempo = 0;
	int transpose = 0;
	int globalTranspose = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int curNoteSize = 0;
	int curInst = 0;
	int curFX = 0;
	int curVol = 0;

	sprintf(outfile, "seqs.txt");
	if ((txt = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file seqs.txt!\n");
		exit(2);
	}
	else
	{
		seqPos = seqList[curSeq];

		while (seqList[curSeq] != 0 && endSeq == 0 && seqPos < bankSize * 2 && seqPos > 0x1000)
		{
			if (seqPos == seqList[curSeq])
			{
				fprintf(txt, "Sequence 0x%04X:\n", seqList[curSeq]);
			}
			command[0] = romData[seqPos - bankAmt];
			command[1] = romData[seqPos + 1 - bankAmt];
			command[2] = romData[seqPos + 2 - bankAmt];

			if (command[0] == 0x00)
			{
				fprintf(txt, "End of sequence\n\n");
				curSeq++;
				seqPos = seqList[curSeq];
			}

			else if (command[0] == 0xFF)
			{
				curInst = command[1];
				curFX = command[2];
				fprintf(txt, "Change instrument: %i, effect: %i\n", curInst, curFX);
				seqPos += 3;
			}

			else
			{
				curNote = command[1];
				curNoteLen = command[0];
				curNoteSize = command[2];
				if (curNote != 0x3F)
				{
					fprintf(txt, "Note: %i, length: %i, size: %i\n", curNote, curNoteLen, curNoteSize);
				}
				else
				{
					fprintf(txt, "Rest, length: %i, size: %i\n", curNoteLen, curNoteSize);
				}

				seqPos += 3;
			}
		}
		fclose(txt);
	}
}