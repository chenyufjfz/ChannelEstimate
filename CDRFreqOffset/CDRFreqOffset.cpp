// CDRFreqOffset.cpp : 定義主控台應用程式的進入點。
//


#include <stdio.h>
#include <math.h>
#include "Header.h"
const short SptPos_m2_m0[12] =
{ 0, 12, 24, 36, 48, 60, 61, 73, 85, 97, 109, 121 };
const short SptPos_m2_m1[10] =
{ 8, 20, 32, 44, 56, 65, 77, 89, 101, 113 };
const short SptPos_m2_m2[10] =
{ 4, 16, 28, 40, 52, 69, 81, 93, 105, 117 };
const short SptPos_m13_m0[22] =
{ 0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 121, 133, 145, 157, 169, 181, 193, 205, 217, 229, 241 };
const short SptPos_m13_m1[20] =
{ 8, 20, 32, 44, 56, 68, 80, 92, 104, 116, 125, 137, 149, 161, 173, 185, 197, 209, 221, 233 };
const short SptPos_m13_m2[20] =
{ 4, 16, 28, 40, 52, 64, 76, 88, 100, 112, 129, 141, 153, 165, 177, 189, 201, 213, 225, 237 };
const short SpecIdx = 1;

#define Max(x,y) ((x)>(y) ? (x) :(y))
#define Abs abs
const short Kms[39] = { 45, 22, 15, 11, 9, 7, 6, 5, 6, 5, 9, 9, 5, 7, 7, 7, 7, 4, 6, 6, 4, 9, 7, 11, 11, 6, 9, 9, 9, 9, 5, 7, 7, 7, 7, 6, 6, 5, 4 };
int SFO_Integration = 0;
short angle_cordic_fix(int rr, int ii, short stage)
{
	short PI = (1 << 15) - 1;
	short HalfPI = (1 << 14);
	short CATAN[15] = { 8192, 4836, 2555, 1297, 651, 325, 162, 81, 40, 20, 10, 5, 2, 1, 0 };
	int tmpr, tmpi, tmp;
	short phase = 0;
	int i;				//stage loop

	char	x_sign = 1;
	char	y_sign = 1;
	char	xyswap = 0;

	tmpr = rr;
	tmpi = ii;

	if (tmpr < 0){
		tmpr = -tmpr;
		x_sign = 0;
	}
	if (tmpi < 0){
		tmpi = -tmpi;
		y_sign = 0;
	}
	if (tmpi > tmpr){
		tmp = tmpr;
		tmpr = tmpi;
		tmpi = tmp;
		xyswap = 1;
	}

	for (i = 0; i<stage; i++){
		if (tmpi < 0){
			tmp = tmpr - (tmpi >> i);
			tmpi = tmpi + (tmpr >> i);
			phase = phase - CATAN[i];
			tmpr = tmp;
		}
		else	{
			tmp = tmpr + (tmpi >> i);
			tmpi = tmpi - (tmpr >> i);
			phase = phase + CATAN[i];
			tmpr = tmp;
		}
	}

	if (xyswap)
		phase = HalfPI - phase;
	if (x_sign == 0)
		phase = PI - phase;
	if (y_sign == 0)
		phase = -phase;
	return phase;
}


void SampleOffsetEst(void)
{
	int L_Real = 0;
	int L_Imag = 0;
	int R_Real = 0;
	int R_Imag = 0;
	int Temp1;
	int Temp2;
	int v_max;
	int sfo_tmp, sfoprt;

	short i, j;
	short SymoblInd;
	short SptNum;
	short ScInd;
	short Scounter;
	short v_sign, scale;
	short Phi1, Phi2;
	short Divalue;

	short ScoShift = 10;
	short Bw0 = 30;
	short ki0 = 0;
	short kp0 = 0;
	
	short SFO;
	int Sat = 53687;//floor(50e-6*(1<<Bw0))

	short KP[3] = { 8, 8, 8 };	//{1/256,1/256,1/256};
	short KI[3] = { 2, 3, 5 };	//{1/4,1/8,1/32};

	//spt num for different transmode and different spt distribution mode
	short SptNumTable[3][3] =
	{
		{ 22, 20, 20 },
		{ 12, 10, 10 },
		{ 22, 20, 20 }
	};
	const short *Spt_p = SptPos_m2_m0;
	
	//	short Kms[39] = {45,22,15,11,9,7,6,5,6,5,9,9,5,7,7,7,7,4,6,6,4,9,7,11,11,6,9,9,9,9,5,7,7,7,7,6,6,5,4};

	//	SymbolNum=SymbolNumArray[TransMode-1];
	//	ScNum=ScNumTable[TransMode-1];
	Scounter = SymbolNum - 3;//repeat symbol counter
	//SptNum=SptNumTable[TransMode-1];
	short *SptNum_p = *(SptNumTable + TransMode - 1);

	for (i = 0; i<Scounter; i++)
	{
		SymoblInd = i % 3;
		SptNum = SptNum_p[SymoblInd] / 2;
		if (TransMode == 2)								//transmode 2
		{
			switch (SymoblInd)
			{
			case 0:
				Spt_p = SptPos_m2_m0;
				break;
			case 1:
				Spt_p = SptPos_m2_m1;
				break;
			case 2:
				Spt_p = SptPos_m2_m2;
				break;
			default:
				break;
			}
		}
		else														//transmode 1&3
		{
			switch (SymoblInd)
			{
			case 0:
				Spt_p = SptPos_m13_m0;
				break;
			case 1:
				Spt_p = SptPos_m13_m1;
				break;
			case 2:
				Spt_p = SptPos_m13_m2;
				break;
			default:
				break;
			}
		}

		for (j = 0; j<SptNum; j++)
		{

			//left sub-band
			ScInd = Spt_p[j];
			Temp1 = (Pmatrix_i[ScInd][i + 3] * Pmatrix_i[ScInd][i]) >> ScoShift;
			Temp2 = (Pmatrix_q[ScInd][i + 3] * Pmatrix_q[ScInd][i]) >> ScoShift;
			L_Real = L_Real + Temp1 + Temp2;

			Temp1 = (Pmatrix_q[ScInd][i + 3] * Pmatrix_i[ScInd][i]) >> ScoShift;
			Temp2 = (Pmatrix_i[ScInd][i + 3] * Pmatrix_q[ScInd][i]) >> ScoShift;
			L_Imag = L_Imag + Temp1 - Temp2;

			//right sub-band
			ScInd = Spt_p[j + SptNum];


			Temp1 = (Pmatrix_i[ScInd][i + 3] * Pmatrix_i[ScInd][i]) >> ScoShift;
			Temp2 = (Pmatrix_q[ScInd][i + 3] * Pmatrix_q[ScInd][i]) >> ScoShift;
			R_Real = R_Real + Temp1 + Temp2;

			Temp1 = (Pmatrix_q[ScInd][i + 3] * Pmatrix_i[ScInd][i]) >> ScoShift;
			Temp2 = (Pmatrix_i[ScInd][i + 3] * Pmatrix_q[ScInd][i]) >> ScoShift;
			R_Imag = R_Imag + Temp1 - Temp2;
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////
	//	Temp1=Abs(R_Real);
	v_max = Max(Abs(R_Real), Abs(R_Imag));
	if (Abs(R_Real) >= Abs(R_Imag))
		v_sign = 0;
	else
		v_sign = 1;
	scale = 0;
	if (v_sign == 0)
	{
		while (v_max >= (1 << 15))	// positive v_max<(1<<15)
		{
			scale = scale + 1;
			v_max = (v_max >> 1);
		}
		while (v_max<(1 << 14))	// positive v_max>=(1<<14)
		{
			scale = scale - 1;
			v_max = (v_max << 1);
			if (v_max == 0)
				break;
		}
	}
	else
	{
		while (v_max>(1 << 15))	// negative v_max<=-(1<<15)
		{
			scale = scale + 1;
			v_max = (v_max >> 1);
		}
		while (v_max <= (1 << 14))	// negative v_max>-(1<<14)
		{
			scale = scale - 1;
			v_max = (v_max << 1);
			if (v_max == 0)
				break;
		}
	}
	if (scale>0)
	{
		R_Real = (R_Real >> scale);
		R_Imag = (R_Imag >> scale);
	}
	else
	{
		scale = -scale;
		R_Real = (R_Real << scale);
		R_Imag = (R_Imag << scale);
	}

	v_max = Max(Abs(L_Real), Abs(L_Imag));
	if (Abs(L_Real) >= Abs(L_Imag))
		v_sign = 0;
	else
		v_sign = 1;
	scale = 0;
	if (v_sign == 0)
	{
		while (v_max >= (1 << 15))	// positive v_max<(1<<15)
		{
			scale = scale + 1;
			v_max = (v_max >> 1);
		}
		while (v_max<(1 << 14))	// positive v_max>=(1<<14)
		{
			scale = scale - 1;
			v_max = (v_max << 1);
			if (v_max == 0)
				break;
		}
	}
	else
	{
		while (v_max>(1 << 15))	// negative v_max<=-(1<<15)
		{
			scale = scale + 1;
			v_max = (v_max >> 1);
		}
		while (v_max <= (1 << 14))	// negative v_max>-(1<<14)
		{
			scale = scale - 1;
			v_max = (v_max << 1);
			if (v_max == 0)
				break;
		}
	}
	if (scale>0)
	{
		L_Real = (L_Real >> scale);
		L_Imag = (L_Imag >> scale);
	}
	else
	{
		scale = -scale;
		L_Real = (L_Real << scale);
		L_Imag = (L_Imag << scale);
	}
	//////////////////////////////////////////////////////////////////////////////////////

	// S15
	Phi1 = angle_cordic_fix(R_Real, R_Imag, 14);	// pi/2-1<<14
	Phi2 = angle_cordic_fix(L_Real, L_Imag, 14);	// pi/2-1<<14
	/*
	if (SpecIdx == 9 || SpecIdx == 11 || SpecIdx == 16)
		Divalue = 6;
	else if (SpecIdx == 22 || SpecIdx == 25 || SpecIdx == 30)
		Divalue = 9;
	else
		Divalue = 45;*/
	Divalue = Kms[SpecIdx - 1];

	if (TransMode == 2)
		Divalue = Divalue * 2;

	sfo_tmp = (Phi2 - Phi1)*Divalue; // S23
	//sfo_tmp = 1.0*1e6*(Phi2 - Phi1)/(1<<5)/M_1b/(2048+CPLen)/3;

	if (sfo_tmp>Sat)
		sfo_tmp = Sat;
	else if (sfo_tmp<-Sat)
		sfo_tmp = -Sat;	// S16


	//read subframe counter from FPGA,SubFrameCnt
	SubFrameCnt++;

	if (SubFrameCnt <= 3 * (1 << KI[0]))
	{
		ki0 = KI[0];
		kp0 = KP[0];
	}
	else if (SubFrameCnt <= 6 * (1 << KI[0]))
	{
		ki0 = KI[1];
		kp0 = KP[1];
	}
	else
	{
		ki0 = KI[2];
		kp0 = KP[2];
	}
	SFO_Integration = (sfo_tmp >> ki0) + SFO_Integration;		// close loop S13+(Bw0-23)=S20
	sfoprt = (((sfo_tmp >> kp0) + SFO_Integration) >> (Bw0 - 23));// close loop S13
	SFO = sfoprt;

	/*
	printf("R_Real=%d, R_Imag=%d, L_Real=%d, L_Imag=%d, Pi1=%d, Pi2=%d, sfo_tmp=%d, SFO_inte=%d, SFO=%d\n", 
		R_Real, R_Imag, L_Real, L_Imag, Phi1, Phi2, sfo_tmp, SFO_Integration, SFO);*/
}

static FILE *fi = NULL;
static FILE *fq = NULL;
void read_file(char * filename_i, char * filename_q)
{	
	if (fi == NULL)
		fopen_s(&fi, filename_i, "r");
	if (fq == NULL)
		fopen_s(&fq, filename_q, "r");
	for (int x = 0; x < 56; x++)
		for (int y = 0; y < 242; y++) {
			fscanf_s(fi, "%d", &Pmatrix_i[y][x+6]);
			fscanf_s(fq, "%d", &Pmatrix_q[y][x+6]);
		}
}

void test_sample_offset()
{
	char filename_i[] = "F:/chenyu/CDR/cdr_sfodebug/pmatrix_i.am";
	char filename_q[] = "F:/chenyu/CDR/cdr_sfodebug/pmatrix_q.am";
	for (int i = 0; i < 16; i++) {
		read_file(filename_i, filename_q);
		SampleOffsetEst();
	}
}
extern enum WORKMODE work_mode;
void renew_file(char *filename)
{
	FILE *fp;
	fopen_s(&fp, filename, "w");
	fclose(fp);
}

void write_result(char * filename, short matrix_i[][Column_h], short matrix_q[][Column_h], int sym, int count)
{
	FILE *fp;
	fopen_s(&fp, filename, "at");
	for (int x = sym; x < sym + count; x++)
		for (int y = 0; y < 242; y++)
			fprintf(fp, " (%d,%d)\n", matrix_i[y][x], matrix_q[y][x]);

	fclose(fp);
}

void write_sn(char * filename, int signal_noise[], int count)
{
	FILE *fp;
	fopen_s(&fp, filename, "at");
	for (int i = 0; i < count; i++)
		fprintf(fp, " %d\n", signal_noise[i]);
	fclose(fp);
}

void write_tcoef(char * filename, short coef[], int count)
{
	FILE *fp;
	fopen_s(&fp, filename, "at");
	for (int i = 0; i < count; i++)
		fprintf(fp, " %d\n", coef[i]);
	fclose(fp);
}

void write_fcoef(char * filename, short coef_i[], short coef_q[], int count)
{
	FILE *fp;
	fopen_s(&fp, filename, "at");
	for (int i = 0; i < count; i++)
		fprintf(fp, " (%d,%d)\n", coef_i[i], coef_q[i]);
	fclose(fp);
}

void test_mer()
{
	char filename_i[] = "F:/chenyu/CDR/Mer&bug/cdr_mer/pmatrix_i_cy.am";
	char filename_q[] = "F:/chenyu/CDR/Mer&bug/cdr_mer/pmatrix_q_cy.am";
	work_mode = MER;
	renew_file("HestTd.am");
	renew_file("HestFd.am");
	renew_file("Npwr.am");
	renew_file("Spwr.am");
	for (int i = 0; i < 4; i++) {
		read_file(filename_i, filename_q);
		ChannelEst();
	}
}

void simulate()
{
	char filename_i[] = "F:/chenyu/CDR/Mer&bug/CEinput/Pmat_r_dop.txt";
	char filename_q[] = "F:/chenyu/CDR/Mer&bug/CEinput/Pmat_i_dop.txt";
	work_mode = SIMULATE;
	renew_file("HestTd.am");
	renew_file("HestFd.am");
	renew_file("Npwr.am");
	renew_file("Spwr.am");
	renew_file("Weight_F.txt");
	renew_file("Weight_exF.txt");
	renew_file("Weight_T.txt");
	renew_file("Weight_exT.txt");
	InitPara();
	for (int i = 0; i < 8; i++) {
		read_file(filename_i, filename_q);
		if (i == 0) 
		{
			DopplerSft = 15;
			NoiseVar = 50;
		}
		
		TcoefCalc();//102
		if (i==0)
			Mse_t = 80;
		FcoefCalc();//46		
		ChannelEst();
		DopplerSftEst();
	}
}

int main()
{
	//test_mer();
	simulate();
	printf("finished\n");
	getchar();
	return 0;
}

