#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>
#include <windows.h>

using namespace std;

#define	RADIX 7
#define SIZE 8192

void hHashCalc(char *text, int length, unsigned int *rehash);
void textHash(char *text, int *textlen, unsigned int *texthas, int *patlen);
void HashSearch(char *text, int textlen, unsigned int texthas [], char *pattern, int patlen, unsigned int pathas, int flag []);
void Emphasis(char *text, int textlen, int patlen, int flag [], int Count);
void InsertChar(char *text, char *shift, int flag [], int mem [], int *counter, char *insert);
void ShiftChar(char *text, char *shift1, char *shift2, int flag [], int mem1 [], int mem2 [], int *counter, int inslen, int looptimes);

int main(){
	char text[SIZE * 2], pattern[SIZE];
	string inputtext;
	int textlen[1], patlen[1];
	unsigned int texthas[SIZE * 2] = { 0 }, pathas[1] = { 0 };
	int FoundFlag[SIZE] = { 0 }, FoundCount = 0;
	int i;
	float sum = 0.0f;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	LARGE_INTEGER start, stop;

	cout << "*Please input text." << endl;
	getline(cin, inputtext);
	const char *convert = inputtext.c_str();
	strcpy(text, convert);
	textlen[0] = strlen(text);

	do{
		cout << endl << "*Please input pattern." << endl;
		getline(cin, inputtext);
		convert = inputtext.c_str();
		strcpy(pattern, convert);
		patlen[0] = strlen(pattern);

		if (textlen[0] < patlen[0])
		{
			cout << "**Search pattern is larger than the text size.**" << endl;
		}
	} while (textlen[0] < patlen[0]);

//パターンのハッシュ値計算
	QueryPerformanceCounter(&start);
	hHashCalc(pattern, patlen[0], pathas);
	QueryPerformanceCounter(&stop);
	sum += (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart;
	cout << "Time required(pattern hash)\t:\t" << (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart * 1000.0 << " millseconds" << endl;

	cout << endl << "*Pattern Hash(" << pattern << ") = " << pathas[0] << endl << endl;

//テキストのハッシュ値計算
	QueryPerformanceCounter(&start);
	textHash(text, textlen, texthas, patlen);
	QueryPerformanceCounter(&stop);
	sum += (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart;
	cout << "Time required(text hash)\t:\t" << (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart * 1000.0 << " millseconds" << endl;

	cout << "Time required(sum)\t:\t" << sum * 1000.0 << " millseconds" << endl;

//ハッシュ値比較
	cout << "*Finding..." << endl;
	HashSearch(text, textlen[0], texthas, pattern, patlen[0], pathas[0], FoundFlag);

	for (i = 0; i < textlen[0]; i++)
		if (FoundFlag[i] == 1)	FoundCount++;
	cout << "*FoundCount = " << FoundCount << endl;
	if (FoundCount != 0)
	{
		Emphasis(text, textlen[0], patlen[0], FoundFlag, FoundCount);
		cout << endl << "**Found!!**" << endl << text << endl;
	}
	else
	{
		cout << endl << "**Not found**" << endl;
	}

	return 0;
}

void hHashCalc(char *text, int length, unsigned int *rehash)
{
	int scan_idx;
	*rehash = 0;

	for (scan_idx = 0; scan_idx < length; scan_idx++)
	{
		*rehash += (int) pow(RADIX, (double) scan_idx) * text[scan_idx];
		//*rehash += ((scan_idx + 1) * RADIX) * text[scan_idx];
	}

	/*
	cout << "Hash(";
	for(scan_idx = 0; scan_idx < length; scan_idx++) cout << text[scan_idx];
	cout << ") = " << rehash << endl;
	*/
}

void textHash(char *text, int *textlen, unsigned int *texthas, int *patlen)
{
	hHashCalc(text, *patlen, &texthas[0]);
	/*
	for (int loop = 1; loop < *textlen - *patlen + 1; loop++){
		texthas[loop] = (texthas[loop - 1] - text[loop - 1]) / RADIX
			+ text[*patlen + loop - 1] * (int) pow(RADIX, (double) (*patlen - 1));
	}
	*/

	///*
	for (int loop = 0; loop < *textlen - *patlen + 1; loop++){
		hHashCalc(text + loop, *patlen, &texthas[loop]);
	}
	//*/
}


void HashSearch(char *text, int textlen, unsigned int texthas [],
	char *pattern, int patlen, unsigned int pathas, int flag [])
{
	int i, j;

	for (i = 0; i < textlen - patlen + 1; i++)
	{
		if (pathas == texthas[i])
		{
			//cout << "Found the same hash!" << endl;
			cout << "Text Hash(";
			for (j = 0; j < patlen; j++) cout << text[i + j];
			cout << ") = " << texthas[i] << endl;
			j = 0;
			do{
				if (text[i + j] != pattern[j])	break;
			} while (++j < patlen);

			if (j == patlen)
			{
				flag[i] = 1;
			}
		}
	}
}

void Emphasis(char *text, int textlen, int patlen, int flag [], int Count)
{
	int i, looptimes;
	char shift1[SIZE], shift2[SIZE];
	int mem1[SIZE * 2], mem2[SIZE * 2];
	char insert1 [] = " << ", insert2 [] = " >> ";
	int inslen = strlen(insert1);

	looptimes = textlen - patlen + (8 * Count);

	for (i = 0; i < textlen - patlen + (8 * Count); i++)
	{
		if (flag[i] == 1)
		{
			InsertChar(text, shift1, flag, mem1, &i, insert1);
			ShiftChar(text, shift1, shift2, flag, mem1, mem2, &i, inslen, looptimes);
			i += patlen;

			InsertChar(text, shift1, flag, mem1, &i, insert2);
			ShiftChar(text, shift1, shift2, flag, mem1, mem2, &i, inslen, looptimes);

		}
	}
}

void InsertChar(char *text, char *shift, int flag [], int mem [], int *counter, char *insert)
{
	int inslen = strlen(insert), j;
	for (j = 0; j < inslen; j++)
	{
		shift[j] = text[*counter + j];
		mem[j] = flag[*counter + j];
	}
	for (j = 0; j < inslen; j++)
	{
		text[*counter + j] = insert[j];
	}
	*counter += inslen;
}

void ShiftChar(char *text, char *shift1, char *shift2, int flag [], int mem1 [], int mem2 [],
	int *counter, int inslen, int looptimes)
{
	int j;
	for (j = 0; j < looptimes; j++)
	{
		shift2[j] = text[*counter + j];
		mem2[j] = flag[*counter + j];
		if (j < inslen){
			text[*counter + j] = shift1[j];
			flag[*counter + j] = mem1[j];
		}
		else{
			text[*counter + j] = shift2[j - inslen];
			flag[*counter + j] = mem2[j - inslen];
		}
	}
}
