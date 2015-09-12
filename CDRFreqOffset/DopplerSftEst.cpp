#include "Header.h"

void DopplerSftEst(void)
{
	int A_real=0;
	int B_real=0;
	int C_real=0;
	int D_real=0;
	int E_real=0;
	
	int CorCoef=0;
	int Ehh1=0;
	short i;
	short SymbSpan;
	short n_symb,n_sc;
	
//	ScNum=ScNumTable[TransMode-1];							
//	SymbolNum=SymbolNumArray[TransMode-1];
//	short DopplerSft;
	int temp1,temp2;
	const short J2Fd[3][71] = 
	{
		{512,511,510,509,506,503,499,495,489,483,477,470,462,453,444,435,424,414,403,391,379,366,353,340,326,312,297,283,268,253,237,222,206,191,175,159,143,128,112,97,81,66,51,36,21,7,-7,-21,-34,-47,-60,-72,-84,-95,-106,-116,-126,-135,-144,-152,-160,-167,-174,-180,-185,-190,-194,-197,-200,-203,-204},
		{512,511,510,509,506,503,499,494,489,482,476,468,460,451,442,432,422,411,399,387,374,361,348,334,320,306,291,276,261,245,229,214,198,182,166,150,134,118,102,86,71,55,40,25,11,-4,-18,-32,-45,-58,-70,-82,-94,-105,-115,-125,-135,-144,-152,-160,-167,-174,-180,-185,-190,-194,-198,-201,-203,-205,-206},
		{512,511,510,509,507,504,501,497,493,488,482,476,469,462,454,446,437,428,419,409,398,387,376,364,352,340,327,314,301,288,274,260,246,232,218,203,189,175,160,146,131,117,102,88,74,60,46,33,19,6,-7,-19,-32,-44,-55,-67,-78,-88,-99,-109,-118,-127,-135,-144,-151,-158,-165,-171,-177,-182,-187}
	};
	
	SymbSpan=((TransMode==2)?12:6);;									//the interval of the two symbol
		
	for (n_symb=0;n_symb<(SymbolNum-SymbSpan);n_symb++)
	{
		for (n_sc=0;n_sc<ScNum/2;n_sc++)//left band
		{
			if (((n_symb % 3 == 0) && (n_sc % 12 == 0)) || ((n_symb % 3 == 1) && (n_sc % 12 == 8))
								|| ((n_symb % 3 == 2) && (n_sc % 12 == 4))) 
			{
				temp1=((Hmatrix_i[n_sc][n_symb])*(Hmatrix_i[n_sc][n_symb]))>>10;
				temp2=((Hmatrix_q[n_sc][n_symb])*(Hmatrix_q[n_sc][n_symb]))>>10;
				B_real=B_real+temp1+temp2;
				
      	
				temp1 =	((Hmatrix_i[n_sc][n_symb+SymbSpan])*(Hmatrix_i[n_sc][n_symb+SymbSpan]))>>10;
				temp2 = ((Hmatrix_q[n_sc][n_symb+SymbSpan])*(Hmatrix_q[n_sc][n_symb+SymbSpan]))>>10;
				A_real = A_real + temp1 + temp2;

      	
				temp1 = ((Hmatrix_i[n_sc][n_symb+SymbSpan])*(Hmatrix_i[n_sc][n_symb]))>>10;
				temp2 = ((Hmatrix_q[n_sc][n_symb+SymbSpan])*(Hmatrix_q[n_sc][n_symb]))>>10;
				C_real = C_real + temp1 + temp2;
				temp1=0;
				temp2=0;
			}
		}
		
		for (n_sc=ScNum/2;n_sc<ScNum;n_sc++)//right band
		{
			if (((n_symb % 3 == 0) && (n_sc % 12 == 1)) || ((n_symb % 3 == 1) && (n_sc % 12 == 5))
								|| ((n_symb % 3 == 2) && (n_sc % 12 == 9))) 
			{
				temp1=Hmatrix_i[n_sc][n_symb];
				temp2=Hmatrix_q[n_sc][n_symb];
				
				temp1=((Hmatrix_i[n_sc][n_symb])*(Hmatrix_i[n_sc][n_symb]))>>10;
				temp2=((Hmatrix_q[n_sc][n_symb])*(Hmatrix_q[n_sc][n_symb]))>>10;
				
				B_real=B_real+temp1+temp2;
      	
				temp1 =	((Hmatrix_i[n_sc][n_symb+SymbSpan])*(Hmatrix_i[n_sc][n_symb+SymbSpan]))>>10;
				temp2 = ((Hmatrix_q[n_sc][n_symb+SymbSpan])*(Hmatrix_q[n_sc][n_symb+SymbSpan]))>>10;
				A_real = A_real + temp1 + temp2;
				
      	
				temp1 = ((Hmatrix_i[n_sc][n_symb+SymbSpan])*(Hmatrix_i[n_sc][n_symb]))>>10;
				temp2 = ((Hmatrix_q[n_sc][n_symb+SymbSpan])*(Hmatrix_q[n_sc][n_symb]))>>10;
				C_real = C_real + temp1 + temp2;
				temp1=0;
				temp2=0;
			}
		}
	}
	A_real = (A_real>>10);//u16
	B_real = (B_real>>10);//u16
	C_real = (C_real>>10);//s16
	
//////////////////////////////////////////////////////////////			
	D_real = 0;
	for (i=15;i>=0;i--)
	{
		D_real = D_real + (1<<i);
		if((D_real*D_real)>(A_real*B_real))
			D_real = D_real - (1<<i);
	} // u16
	
	if (D_real==0)
		D_real = 1;
//////////////////////////////////////////////////////////////
	E_real = 0;
	for (i=23;i>=0;i--)
	{
		E_real = E_real + (1<<i);
		if((E_real*D_real)>(65536*256))
			E_real = E_real - (1<<i);
	}// u23
	E_real = (E_real>>5); // u14

	if(SubFrameCnt<1024)
		SubFrameCnt++;
	if (SubFrameCnt<=4)
	{
		CorCoef = CorCoef + E_real*C_real; //*512*64
		if (SubFrameCnt==4)
		{	
			CorCoef = (CorCoef>>2);
			Ehh1 = CorCoef;
		}
		else
			Ehh1 = E_real*C_real;
	}
	else
	{
		CorCoef = CorCoef*15+E_real*C_real; // 512*64
		CorCoef = (CorCoef>>4);
		Ehh1 = CorCoef;
	}

	
	
	DopplerSft = 69;
	for (i=1;i<71;i++)
	{
		if (Ehh1>1024*J2Fd[TransMode-1][i])
		{
			DopplerSft = (i-1);
			break;
		}
	}
#if 1
	printf("DopplerSft=%d,Ehh1=%d", DopplerSft, Ehh1);
#endif
}
