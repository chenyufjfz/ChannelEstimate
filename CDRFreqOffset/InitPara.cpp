#include "Header.h"

extern short PdpSubFrame[FftLength];

void InitPDP(void)
{
	int i;
	short Tau = 15;
	//Tau=*upsynctau;
	//Tau=(Tau<-1024)?-1024:Tau;
	//Tau=(Tau>1023)?1023:Tau;

	int tmp;

	for (i = 0; i <= Tau; i++)
	{
		PdpSubFrame[i] = 16384 / (Tau + 1);
	}
	for (i = Tau + 1; i<FftLength; i++)
	{
		PdpSubFrame[i] = 0;
	}
}

void InitPara(void)
{
	//PDP init
	
	//NoiseVar init
	NoiseVar=168;
	//DopplerSft init
	if (!OnlyLeft)
	{
		DopplerSft=8;
	}
	else
	{
		DopplerSft=69;
	}
	
	InitPDP();
}

