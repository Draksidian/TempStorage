#include "StdAfx.h"

namespace SquareToBrick
{
	LargeNumber SQN2;
	LargeNumber m;

	int CheckCandidateMatch_aaToN(LargeNumber *tN, LargeNumber **vLN, int Params, int pos, LargeNumber *TempRes1, LargeNumber *TempRes2)
	{
		// a * a + m = 2 * x * SQN + x * x
		// a range ( 0, SQN )
		// b range ( SQN , N / 2 )
		if (vLN[0]->Len > (tN->Len + 1) / 2 || vLN[1]->Len > (tN->Len + 1) / 2)
			return 0;

		LargeNumber aa;
		MulLN(vLN[0], vLN[0], &aa);
		AddLN(&m, &aa, TempRes1);

		LargeNumber xx;
		MulLN(vLN[1], vLN[1], &xx);
		LargeNumber xsqn;
		MulLN(vLN[1], &SQN2, &xsqn);
		AddLN(&xx, &xsqn, TempRes2);

		for (int i = 0; i <= pos; i++)
			if (TempRes1->Digits[i] != TempRes2->Digits[i])
				return 0;

		EatLeadingZeros(TempRes1);
		EatLeadingZeros(TempRes2);

		return 1;
	}

	void DivTest_aaToN(__int64 iA, __int64 iB)
	{
		// a * a + m = 2 * x * SQN + x * x
		LargeNumber N;
		__int64 iN = iA * iB;
		__int64 iSQN = isqrt4(iN);
		unsigned int im = iN - iSQN * iSQN;
		SetLN(N, iN);
		SetLN(SQN2, iSQN * 2);
		SetLN(m, im);
		int ia = (iB - iA) / 2;
		int ib = (iA + iB) / 2 - iSQN;
		printf("Expecting solution a = %d, b = %d. N = %lld, SQRT(N) = %lld. Bruteforce trycount %lld\n", ia, ib, iN, iSQN, iSQN / 2);

		LargeNumber a, b;

		LargeNumber EndSignal;
#define ParamCount 3
		LargeNumber *vLN[ParamCount];
		vLN[0] = &a;
		vLN[1] = &b;
		vLN[2] = &EndSignal;

		for (int i = 0; i < ParamCount; i++)
			SetLN(vLN[i], 0);
		InitLN(vLN[ParamCount - 1]);

		//start generating combinations and check if it's a feasable candidate
		int AtPos = 0;
		int SolutionsFound = 0;
		int CandidatesFound = 0;
		int StepsTaken = 0;
		char DEBUG_Combinations_generated[ParamCount][99 + 1];
		memset(DEBUG_Combinations_generated, 0, sizeof(DEBUG_Combinations_generated));
		do
		{
			LargeNumber TempRes1, TempRes2;
			int GenNextCandidate = 0;
			StepsTaken++;
			int Match = CheckCandidateMatch_aaToN(&N, vLN, ParamCount, AtPos, &TempRes1, &TempRes2);
			if (Match == 1)
			{
				CandidatesFound++;
				if (CandidatesFound % 100 == 0)
				{
					int chars[4] = { '\\','|','/','-' };
					printf("\r%c", chars[(CandidatesFound / 100) % 4]);
				}
				int SolutionFound = CheckSolution(TempRes1, TempRes2);
				if (SolutionFound == 1)
				{
					SolutionsFound++;
					printf("\r%d / %d )sol : \t a:", SolutionsFound, CandidatesFound);
					PrintLN(a);
					printf("\t b:");
					PrintLN(b);
					printf("\n");
					GenNextCandidate = 1;
					break;
				}
				AtPos++;
				ResetCandidateAtPos(vLN, ParamCount, AtPos, 1);
			}
			else
				GenNextCandidate = 1;
			if (AtPos > 0 && vLN[ParamCount - 1]->Digits[AtPos] > 0)
			{
				ResetCandidateAtPos(vLN, ParamCount, AtPos);
				AtPos--;
				GenNextCandidate = 1;
			}
			if (GenNextCandidate == 1)
				GenerateNextCandidateAtPos(vLN, ParamCount, AtPos);
			//    }while( SolutionsFound == 0 && vLN[ParamCount-1]->Digits[0] == 0 );
		} while (vLN[ParamCount - 1]->Digits[0] == 0);

		if (SolutionsFound == 0)
			printf("\rNo Luck finding a solution\n");
		else
			printf("\rDone testing all possible solutions\n");

		printf("Steps taken %d\n\n", StepsTaken);
	}
};

void DivTestaaToN()
{
    // 108k
	SquareToBrick::DivTest_aaToN( 349, 751 ); // N = 262099 SN = 511
    // 938k
	SquareToBrick::DivTest_aaToN( 6871, 7673 ); // N = 52721183 , SN = 7260
    // 9M tries
	SquareToBrick::DivTest_aaToN( 26729, 31793 ); // N = 849795097 , SN = 29151
	SquareToBrick::DivTest_aaToN(784727, 918839);
	SquareToBrick::DivTest_aaToN(6117633, 7219973);
	SquareToBrick::DivTest_aaToN(26729, 61781);
	SquareToBrick::DivTest_aaToN(11789, 61781);
}
