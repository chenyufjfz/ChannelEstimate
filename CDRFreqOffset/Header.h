#ifndef _HEADER_H_
#define _HEADER_H_
//This file is writen by Chen Yu

#define Row_p 242*8		
#define Column_p (111+13)
#define Ngap_f 4
#define Ngap_t 3

#define Ntap_f 5
#define Ntap_t 5
#define Nex_t 4	
#define Nex_f 4

#define Row_h 242*8
#define Column_h (111+1)

#ifndef EXTERN
#define EXTERN extern
extern int TransMode;
extern short OnlyLeft;
extern short SubFrameCnt;
extern short Hmatrix_i[Row_h][Column_h];
extern short Hmatrix_q[Row_h][Column_h];
#else
int TransMode = 1;
short OnlyLeft = 0;
short SubFrameCnt = 0;
short Hmatrix_i[Row_h][Column_h] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
short Hmatrix_q[Row_h][Column_h] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
#endif

EXTERN short Pmatrix_i[Row_p][Column_p];
EXTERN short Pmatrix_q[Row_p][Column_p];
EXTERN short DopplerSft;
EXTERN int NoiseVar;
EXTERN short Winter_t[2 * Ngap_t - 1][Ntap_t];
EXTERN short Wex_t[Nex_t];
EXTERN int Mse_t;
EXTERN int Mse_f;
EXTERN short Winter_f_i[Ntap_f*Ngap_f];
EXTERN short Winter_f_q[Ntap_f*Ngap_f];
EXTERN short Wex_f_i[Nex_f];
EXTERN short Wex_f_q[Nex_f];
EXTERN short SymbolNum;
EXTERN short ScNum;
#define FftLength     256

void SampleOffsetEst(void);
void ChannelEst(void);
void write_result(char * filename, short matrix_i[][Column_h], short matrix_q[][Column_h], int sym, int count);
void write_sn(char * filename, int signal_noise[], int count);
void write_tcoef(char * filename, short coef[], int count);
void write_fcoef(char * filename, short coef_i[], short coef_q[], int count);
void TcoefCalc();
void FcoefCalc();
void InitPara();
void DopplerSftEst();
enum WORKMODE {
	NORMAL,
	SIMULATE,
	MER
};

#include <stdio.h>
#define PRINT_MER_RESULT 1
#endif