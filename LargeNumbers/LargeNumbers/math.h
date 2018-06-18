#pragma once

#define MAX_DIGIT_COUNT 500
#define USE_BASE        10

#ifndef MAX
    #define MAX( a, b ) ((a)>(b))?(a):(b)
    #define MAX3( a, b, c ) MAX( a, MAX( b, c ) )
#endif

struct LargeNumber
{
	LargeNumber()
	{
		Len = Pos = 0;
		memset(Digits, 0, sizeof(Digits));
	}
    int Digits[MAX_DIGIT_COUNT];
    int Len;    // not used yet ?
	int Pos;    // position of digit we are working on
};

void InitLN( LargeNumber *N );
void InitLN( LargeNumber &N );
void SetLN( LargeNumber *N, int n );
void SetLN( LargeNumber &N, int n );
void NormalizeLN( LargeNumber *N, int Start = 0 );
void NormalizeLN( LargeNumber &N, int Start = 0 );
void AddLN( LargeNumber *A, LargeNumber *B, LargeNumber *C );
void AddLN( LargeNumber &A, LargeNumber &B, LargeNumber &C );
void MulLN( LargeNumber *A, LargeNumber *B, LargeNumber *C );
void MulLN( LargeNumber &A, LargeNumber &B, LargeNumber &C );
void ToIntLN( LargeNumber *N, int *n );
void ToIntLN( LargeNumber &N, int &n );
int ToIntLN( LargeNumber *N );
void CopyLN( LargeNumber *From, LargeNumber *To );
void CopyLN( LargeNumber &From, LargeNumber &To );
void HalfLN( LargeNumber *N );
void HalfLN( LargeNumber &N );
void SubLN( LargeNumber *N, int n );
void SubLN( LargeNumber &N, int n );

void EatLeadingZeros( LargeNumber *N );
void EatLeadingZeros( LargeNumber &N );

void PrintLN( LargeNumber *N, int EndPos = -1 );
void PrintLN( LargeNumber &N, int EndPos = -1 );

int isqrt(int n);
int IsLarger(LargeNumber *Larger, LargeNumber *Smaller);