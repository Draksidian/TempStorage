#include "StdAfx.h"

void InitLN( LargeNumber *N )
{
    N->Len = 0;
    memset( N->Digits, 0, sizeof( N->Digits ) );
}
void InitLN( LargeNumber &N ) {    InitLN( &N ); }

void SetLN( LargeNumber *N, int n )
{
    InitLN( N );
    while( n > 0 )
    {
        N->Digits[ N->Len++ ] = n % USE_BASE;
        n = n / USE_BASE;
    }
}
void SetLN( LargeNumber &N, int n ) { SetLN( &N, n ); }

void NormalizeLN( LargeNumber *N, int Start )
{
    for( int i = Start; i < N->Len; i++ )
        if( N->Digits[ i ] >= USE_BASE )
        {
            N->Digits[ i + 1 ] += N->Digits[ i ] / USE_BASE;
            N->Digits[ i ] = N->Digits[ i ] % USE_BASE;
        }
    while( N->Digits[ N->Len ] >= USE_BASE )
    {
        N->Digits[ N->Len + 1 ] += N->Digits[ N->Len ] / USE_BASE;
        N->Digits[ N->Len ] = N->Digits[ N->Len ] % USE_BASE;
        N->Len++;
    }
    if( N->Digits[ N->Len ] > 0 )
        N->Len++;
}
void NormalizeLN( LargeNumber &N, int Start ) { NormalizeLN( &N, Start ); }

void AddLN( LargeNumber *A, LargeNumber *B, LargeNumber *C )
{
    InitLN( C );
    int MaxLen = MAX( A->Len, B->Len );
    for( int i=0;i<MaxLen;i++)
    {
        C->Digits[i] += A->Digits[i] + B->Digits[i];
        C->Len = MAX( C->Len, i );
        NormalizeLN( C, i );
    }
    C->Len = MAX( C->Len, MaxLen );
    if( C->Digits[ C->Len ] > 0 )
        C->Len++;
}
void AddLN( LargeNumber &A, LargeNumber &B, LargeNumber &C ) { AddLN( &A, &B, &C ); }

void AddLN(LargeNumber *A, int B, LargeNumber *C)
{
	InitLN(C);
	C->Digits[0] = A->Digits[0] + B;
	for (int la = 1; la < A->Len; la++)
		C->Digits[la] = A->Digits[la];
	C->Len = A->Len;
	NormalizeLN(C);
}

void MulLN( LargeNumber *A, LargeNumber *B, LargeNumber *C )
{
    InitLN( C );
    for( int la=0;la<A->Len;la++)
        for( int lb=0;lb<B->Len;lb++)
        {
            C->Digits[la+lb] += A->Digits[la] * B->Digits[lb];
            C->Len = MAX( C->Len, la+lb );
            NormalizeLN( C, la + lb );
        }
    C->Len = MAX( C->Len, A->Len + B->Len - 1 );
}
void MulLN( LargeNumber &A, LargeNumber &B, LargeNumber &C ) { MulLN( &A, &B, &C ); }

void MulLN(LargeNumber *A, int B, LargeNumber *C)
{
	InitLN(C);
	for (int la = 0; la < A->Len; la++)
	{
		C->Digits[la] += A->Digits[la] * B;
		C->Len = la + 1;
		NormalizeLN(C, la);
	}
	EatLeadingZeros(C);
	NormalizeLN(C);
}

void ToIntLN( LargeNumber *N, int *n )
{
    *n = 0;
    for( int i = N->Len - 1; i >= 0; i-- )
        *n = *n * USE_BASE + N->Digits[ i ];
}
void ToIntLN( LargeNumber &N, int &n ) { ToIntLN( &N, &n ); }
int ToIntLN( LargeNumber *N ) 
{ 
    int n;
    ToIntLN( N, &n ); 
    return n;
}

void CopyLN( LargeNumber *From, LargeNumber *To )
{
    memcpy( To, From, sizeof( LargeNumber ) );
}
void CopyLN( LargeNumber &From, LargeNumber &To ) { CopyLN( &From, &To ); }

void HalfLN( LargeNumber *N )
{
    for( int i = N->Len - 1; i >= 0; i-- )
    {
        if( N->Digits[ i ] % 2 != 0 && i > 0 )
            N->Digits[ i - 1 ] += USE_BASE;
        N->Digits[ i ] = N->Digits[ i ] / 2;
    }
    if( N->Digits[ N->Len - 1 ] == 0 && N->Len > 0 )
        N->Len--;
}
void HalfLN( LargeNumber &N ) { HalfLN( &N ); }

// !! expects n < N. This function was first added for a simple decrementation
void SubLN( LargeNumber *N, int n )
{
    N->Digits[0] -= n;
    for( int i = 0; i < N->Len; i++ )
        if( N->Digits[i] < 0 )
        {
            while( N->Digits[i] < 0 )
            {
                N->Digits[i + 1] -= 1;
                N->Digits[i] += USE_BASE;
            }
        }
        else 
            break;
    if( N->Digits[ N->Len - 1 ] == 0 && N->Len > 0 )
        N->Len--;
}
void SubLN( LargeNumber &N, int n ) { SubLN( &N, n ); }

void EatLeadingZeros( LargeNumber *N )
{
    while( N->Len > 0 && N->Digits[ N->Len - 1 ] == 0 )
        N->Len--;
}
void EatLeadingZeros( LargeNumber &N ) { EatLeadingZeros( &N ); }

void PrintLN( LargeNumber *N, int EndPos )
{
    if( EndPos < 0 )
        EndPos = N->Len - 1;
    for( int i = EndPos; i >= 0; i--)
        printf( "%d", N->Digits[i] );
}
void PrintLN( LargeNumber &N, int EndPos ) { PrintLN( &N, EndPos ); }

int isqrt(int n)
{
  int b = 0;

  while(n >= 0)
  {
    n = n - b;
    b = b + 1;
    n = n - b;
  }

  return b - 1;
}

int IsLarger(LargeNumber *Larger, LargeNumber *Smaller)
{
	if (Larger->Len > Smaller->Len)
		return 1;
	if (Larger->Len < Smaller->Len)
		return 0;
	for (int i = Larger->Len; i >= 0; i--)
	{
		if (Larger->Digits[i] > Smaller->Digits[i])
			return 1;
		if (Larger->Digits[i] < Smaller->Digits[i])
			return 0;
	}
	return 0;
}

// DOES NOT SUPPORT NEGATIVE NUMBERS !
//C = A - B
void SubLN(LargeNumber *A, LargeNumber *B, LargeNumber *C)
{
	InitLN(C);
	int MaxLen = MAX(A->Len, B->Len);
	for (int i = 0; i < B->Len; i++)
	{
		C->Digits[i] += A->Digits[i] - B->Digits[i];
		while (C->Digits[i] < 0)
		{
			C->Digits[i + 1]--;
			C->Digits[i] += 10;
		}
	}
	C->Len = B->Len;
	for (int i = C->Len; i < MAX_DIGIT_COUNT && C->Digits[i] != 0; i++)
		C->Digits[i] = 0;
}