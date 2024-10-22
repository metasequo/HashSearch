﻿#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>
#include <windows.h>

using namespace std;

#define	RADIX 7
#define SIZE 9216
#define LOOP_NUM 100

void hHashCalc(char *text, int length, unsigned int *rehash);
void textHash(char *text, int *textlen, unsigned int *texthas, int *patlen);
void HashSearch(char *text, int textlen, unsigned int texthas [], char *pattern, int patlen, unsigned int pathas, bool flag []);
void Emphasis(char *text, int textlen, int patlen, bool flag [], int Count);
void InsertChar(char *text, char *shift, bool flag [], bool mem [], int *counter, char *insert);
void ShiftChar(char *text, char *shift1, char *shift2, bool flag [], bool mem1 [], bool mem2 [], int *counter, int inslen, int looptimes);

int main(){
	char text[SIZE * 2], pattern[SIZE];
	string inputtext;
	int textlen[1], patlen[1];
	unsigned int texthas[SIZE * 2] = { 0 }, pathas[1] = { 0 };
	bool FoundFlag[SIZE] = { 0 };
	int FoundCount = 0;
	int i;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	LARGE_INTEGER start, stop;

	float sum = 0.0f;
	float Time_pathas = 0;
	float Time_texthas = 0;
	float Time_sum = 0;
	float Time_HashSearch = 0;

//	cout << "*Please input text." << endl;
	getline(cin, inputtext);
	const char *convert = inputtext.c_str();
	strcpy(text, convert);
	textlen[0] = strlen(text);
//	cout << textlen[0] << endl;

	do{
//		cout << endl << "*Please input pattern." << endl;
		getline(cin, inputtext);
		convert = inputtext.c_str();
		strcpy(pattern, convert);
		patlen[0] = strlen(pattern);

		if (textlen[0] < patlen[0])
		{
			cout << "**Search pattern is larger than the text size.**" << endl;
		}
	} while (textlen[0] < patlen[0]);

	for (int loopcnt = 0; loopcnt < LOOP_NUM; loopcnt++){
		sum = 0.0f;

		//パターンのハッシュ値計算
		QueryPerformanceCounter(&start);
		hHashCalc(pattern, patlen[0], pathas);
		QueryPerformanceCounter(&stop);
		sum += (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart;
		Time_pathas += (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart;
		//	cout << "Time required(pattern hash)\t:\t" << (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart * 1000.0 << " millseconds" << endl;

		//	cout << endl << "*Pattern Hash(" << pattern << ") = " << pathas[0] << endl << endl;

		//テキストのハッシュ値計算
		QueryPerformanceCounter(&start);
		textHash(text, textlen, texthas, patlen);
		QueryPerformanceCounter(&stop);
		sum += (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart;
		Time_texthas += (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart;
		//	cout << "Time required(text hash)\t:\t" << (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart * 1000.0 << " millseconds" << endl;

		Time_sum += sum;
		//	cout << "Time required(sum)\t:\t" << sum * 1000.0 << " millseconds" << endl;

		//ハッシュ値比較
		//	cout << "*Finding..." << endl;

		QueryPerformanceCounter(&start);
		HashSearch(text, textlen[0], texthas, pattern, patlen[0], pathas[0], FoundFlag);
		QueryPerformanceCounter(&stop);

//		sum += (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart;
		Time_HashSearch += (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart;
		//	cout << "Time required(HashSearch)\t:\t" << (float) (stop.QuadPart - start.QuadPart) / freq.QuadPart * 1000.0 << " millseconds" << endl;


	}

	cout << "Time required(pattern hash)," << Time_pathas * 1000.0 / LOOP_NUM << endl;
	cout << "Time required(text hash)," << Time_texthas * 1000.0 / LOOP_NUM << endl;
	cout << "Time required(sum)," << Time_sum * 1000.0 / LOOP_NUM << endl;
	cout << "Time required(HashSearch)," << Time_HashSearch * 1000.0 / LOOP_NUM << endl;

	/*
	for (i = 0; i < textlen[0]; i++)
		if (FoundFlag[i] == true)	FoundCount++;
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
	*/

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
	char *pattern, int patlen, unsigned int pathas, bool flag [])
{
	int i, j;

	for (i = 0; i < textlen - patlen + 1; i++)
	{
		if (pathas == texthas[i])
		{
			//cout << "Found the same hash!" << endl;
/*			cout << "Text Hash(";
			for (j = 0; j < patlen; j++) cout << text[i + j];
			cout << ") = " << texthas[i] << endl;
*/
			j = 0;
			do{
				if (text[i + j] != pattern[j])	break;
			} while (++j < patlen);

			if (j == patlen)
			{
				flag[i] = true;
			}
		}
	}
}

void Emphasis(char *text, int textlen, int patlen, bool flag [], int Count)
{
	int i, looptimes;
	char shift1[SIZE], shift2[SIZE];
	bool mem1[SIZE * 2], mem2[SIZE * 2];
	char insert1 [] = " << ", insert2 [] = " >> ";
	int inslen = strlen(insert1);

	looptimes = textlen - patlen + (8 * Count);

	for (i = 0; i < textlen - patlen + (8 * Count); i++)
	{
		if (flag[i] == true)
		{
			InsertChar(text, shift1, flag, mem1, &i, insert1);
			ShiftChar(text, shift1, shift2, flag, mem1, mem2, &i, inslen, looptimes);
			i += patlen;

			InsertChar(text, shift1, flag, mem1, &i, insert2);
			ShiftChar(text, shift1, shift2, flag, mem1, mem2, &i, inslen, looptimes);

		}
	}
}

void InsertChar(char *text, char *shift, bool flag [], bool mem [], int *counter, char *insert)
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

void ShiftChar(char *text, char *shift1, char *shift2, bool flag [], bool mem1 [], bool mem2 [],
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
