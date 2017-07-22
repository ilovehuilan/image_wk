/*
ÿ��ժҪ
*/
/*
���feature��ֵ**************************************************
��� lineinf[j].LeftPos  == right ������
��� lineinf[j].LeftPos  == left ������
��� lineinf[j].RightPos == right ������
��� lineinf[j].RghtPos  == left ������

*/
#include "common.h"
#include "image.h"
#include "math.h"
#include "steer_control.h"
#include "OLED.h"
#include "speed_control.h"
#include "gpio.h"


uint8 Matrix[45][120] = {0};

 uint8 picture[60][160];     
 uint8 (*picturess)[160];
LineType lineinf[60];

#define FRONT (1)
#define BEHIND (2)
int STATUSDO = FRONT;
int IsZebarLine = 0;
int HalfFlag=0;

float RowWeight[60] = {0};
int TrueRight[60] = {0};
int TrueLeft[60] = {0};
int ISNOOB = 0;

int SPEED_Stright;
int SPEED_S;
int SPEED_Other;
int SPEED_Circle;
int StraightLine2[60] = 
{34,34,34,34,33,33,33,33,33,32,
32,32,32,31,31,31,30,30,30,29,
29,29,28,28,27,27,26,26,26,25,
24,24,22,22,21,21,21,21,20,20,
18,18,18,18,18,17,17,16,15,14,
13,13,12,12,11,10, 9, 9, 8, 7 };

//int StraightLine[60] = {30,30,30,30,29,29,29,28,27,26,
//                        26,26,26,25,25,25,24,24,24,22,
//                        23,23,22,22,21,21,20,20,20,19,
//                        18,17,16,15,15,15,15,15,14,14,
//                        14,14,13,13,13,13,13,12,12,11,
//                        11,11,11,11,10, 9, 8, 8, 7, 6};


int StraightLine[60] = 
{27,27,27,27,26,26,26,25,24,23,
23,23,23,22,22,22,21,21,21,19,
20,20,19,19,18,18,18,17,16,15,
14,14,13,13,12,12,12,11,11,11,
12,12,11,11,10,10, 9, 9, 8, 8,
7, 7, 7, 6, 6, 5, 4, 3, 3, 3};

int Rdem[60] = 
{84,84,85,85,86,86,87,87,88,88,
 89,90,90,91,91,92,92,93,93,94,
95,95,96,96,96,97,97,98,98,99,
99,100,100,101,101,102,102,102,103,103,
103,104,104,104,105,105,105,106,106,106,
106,107,107,107,108,108,108,108,109,109};

int Ldem[60] = 
{75,75,75,74,74,74,73,73,72,72,
71,70,70,69,69,68,68,67,67,66,
66,65,65,64,64,63,63,62,62,62,
61,61,60,60,59,59,58,58,57,57,
56,56,55,55,55,54,54,53,53,53,
52,52,51,51,51,50,50,49,49,49};


int RoadDir = 0;
int CircleStop = 0;
int CircleStop2 = 0;
int dada = 0;
//int Safe_Mode_Flag = 0;

int DelayZebarCount;

/*
CurveLine���ȫ�ֱ���
*/
int RightCount = 0;      //�Ҳ�߽����
int LeftCount = 0;       //���߽����
int LeftDownLow = 0;     //�����ֵ
int LeftDownHigh = 59;   //�����ֵ
int RightDownLow = 0;    //�Ҳ���ֵ
int RightDownHigh = 59;  //�Ҳ���ֵ


int CircleDir = 1;
int IsCircle = 0;
int CircleUp = 0;
int CircleDown = 0;
int CircleLeft = 159;
int CircleRight = 0;
int BlackCircleLeft[60] = { 80 };
int BlackCircleRight[60] = { 80 };
/*
���߳���
*/
int IsCurveLine = 0;
/*
СS����
*/
int IsSLine = 0;
/*
ʮ�ֳ���
*/
int IsCross = 0;
/*
ֱ������
*/
int IsExceed = 0;    //1Ϊ��2Ϊ�ң�0û��
/*
��·ȫ�ֱ�־λ
*/
int StructCircle = 0;
int StructCircleFlag = 0;

/*========================================================================
*  ��������:  ErrorReturn
*  ����˵���� ����ƫ��
*  ����ʱ�䣺2017
*  �޸�ʱ�䣺2017
*  ����˵��:
========================================================================*/
int ErrorReturn(int effect, uint8(*picture)[160])
{
	if(effect > 25)
		return 0;
	
	int sum = 0;
	for(int i = 35; i > 25; i--)
		sum += (int)fabs(lineinf[i].MiddlePos - 80);
	
	return sum;
}


/*========================================================================
*  ��������:  ReturnBigger
*  ����˵�����Ƚ����С�ĳ���
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
========================================================================*/

int ReturnBigger(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

int ReturnSmaller(int a, int b)
{
	if (a > b)
		return b;
	else
		return a;
}


/*========================================================================
*  ��������:  LimitPr
*  ����˵�������Ƶ�ķ�Χ��ʹ֮���ᳬ�����ұ߽�  
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
========================================================================*/
int LimitPr(int a)
{
	if (a > right)
		a = right;
	if (a < left)
		a = left;
	
	return a;
}

/*========================================================================
*  ��������:  TriFind
*  ����˵�������Ǽ��
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵������⵽ ���� 0
����   ���� 1
========================================================================*/
int TriDown = 0;
int TriCount = 0;
int TriFlag = 0;
int TriFlag2 = 0;
int TriPos = 0;
int TriFind(int effect, uint8(*picture)[160], int dir)
{
#define TriFindLimit     (25)
	int i;
	TriDown = 0;
	TriCount = 0;
	TriFlag = 0;
	TriFlag2 = 0;
	TriPos = 0;
	int DDFlag = 0;
	
	switch (dir)
	{
	case 1:
		for (i = down; i > TriFindLimit; i--)
		{
			if(lineinf[i].LeftPos != left && DDFlag == 0)
				DDFlag = 1;
			if (lineinf[i].LeftPos == left && DDFlag == 1)
			{
				TriDown = i;
				break;
			}
		}
		if (TriDown != 0 && TriDown != 59)
		{
			for (i = down; i > TriDown; i--)
			{
				if (lineinf[i].LeftPos >= lineinf[i - 1].LeftPos)
					TriCount++;
			}
			if (TriCount >= ReturnBigger(59 - TriDown , 0))
				TriFlag = 1;
			if ((lineinf[ReturnSmaller(TriDown + 1, 59)].LeftPos - lineinf[TriDown].LeftPos >= 7)
				&& (lineinf[ReturnSmaller(TriDown + 2, 59)].LeftPos - lineinf[ReturnSmaller(TriDown + 1, 59)].LeftPos <= 6))
			{
				TriFlag = 0;
				TriFlag2 = 1;
				TriPos = TriDown + 1;
			}
		}
		break;
		
	case 2:
		for (i = down; i > TriFindLimit; i--)
		{
			if(lineinf[i].RightPos != right && DDFlag == 0)
				DDFlag = 1;
			if (lineinf[i].RightPos == right&& DDFlag == 1)
			{
				TriDown = i;
				break;
			}
		}
		if (TriDown != 0 && TriDown != 59)
		{
			for (i = down; i > TriDown; i--)
			{
				if (lineinf[i].RightPos <= lineinf[i - 1].RightPos)
					TriCount++;
			}
			if (TriCount >= ReturnBigger(59 - TriDown , 0))
				TriFlag = 1;
			if ((lineinf[TriDown].RightPos - lineinf[ReturnSmaller(TriDown + 1, 59)].RightPos >= 7)
				&& (lineinf[ReturnSmaller(TriDown + 1, 59)].RightPos - lineinf[ReturnSmaller(TriDown + 2, 59)].RightPos <= 6))
			{
				TriFlag = 0;
				TriFlag2 = 1;
				TriPos = TriDown + 1;
			}
		}
		break;
	default:
		break;
	}
	if (TriFlag == 1)
		return 0;
	if (TriFlag == 0 && TriFlag2 == 1)
		return 0;
	if (TriFlag == 0 && TriFlag2 == 0)
		return 1;
}


/*========================================================================
*  ��������:  ReturnTendency
*  ����˵�������Ƽ���
*
����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵�������򷵻� 1  
���򷵻� 2
========================================================================*/

float ReturnTendency(uint8(*picture)[160], int Dir)
{
#define TENDLIMITUP   (18) 
#define TENDLIMITDOWN   (27)
	int i = 0;
	
	float AVE_X = 0;
	float AVE_Y = 0;
	float aa = 0;
	float bb = 0;
	float SLOPE = 0;
	
	if(EFFECT >= TENDLIMITUP)
		return 0;
	
	switch(Dir)
	{
	case 1:
		for(i = TENDLIMITDOWN; i > TENDLIMITUP; i--)
			AVE_X += i; 
		AVE_X = AVE_X / (TENDLIMITDOWN - TENDLIMITUP + 1);
		
		for(i = TENDLIMITDOWN; i > TENDLIMITUP; i--)
			AVE_Y += lineinf[i].LeftPos;
		AVE_Y = AVE_Y / (TENDLIMITDOWN - TENDLIMITUP + 1);
		
		for(i = TENDLIMITDOWN; i > TENDLIMITUP; i--)
			aa += (i - AVE_X) * (lineinf[i].LeftPos - AVE_Y);
		
		for(i = TENDLIMITDOWN; i > TENDLIMITUP; i--)
			bb += (i - AVE_X) * (i - AVE_X);
		break;
	case 2:
		for(i = TENDLIMITDOWN; i > TENDLIMITUP; i--)
			AVE_X += i; 
		AVE_X = AVE_X / (TENDLIMITDOWN - TENDLIMITUP + 1);
		
		for(i = TENDLIMITDOWN; i > TENDLIMITUP; i--)
			AVE_Y += lineinf[i].RightPos;
		AVE_Y = AVE_Y / (TENDLIMITDOWN - TENDLIMITUP + 1);
		
		for(i = TENDLIMITDOWN; i > TENDLIMITUP; i--)
			aa += (i - AVE_X) * (lineinf[i].RightPos - AVE_Y);
		
		for(i = TENDLIMITDOWN; i > TENDLIMITUP; i--)
			bb += (i - AVE_X) * (i - AVE_X);
		
		break;
	default:
		break;
		
	}
	
	
	if(bb != 0)
		SLOPE = aa / bb;
	
	return SLOPE;
	//  if (SLOPE > 0)
	//     return 1;
	//  if (SLOPE < 0)
	//     return 2;
	//  if(SLOPE == 0 )
	//     return 0;
}

float ReturnTendency2(uint8(*picture)[160])
{
#define TENDLIMIT   (33)  
	int i = 0;
	
	float AVE_X = 0;
	float AVE_Y = 0;
	float aa = 0;
	float bb = 0;
	float SLOPE = 0;
	
	
	if(EFFECT >= TENDLIMIT)
		return 0;
	for(i = down; i > TENDLIMIT; i--)
		lineinf[i].MiddlePos = (lineinf[i].LeftPos + lineinf[i].RightPos) / 2;
	
	for(i = down; i > TENDLIMIT; i--)
		AVE_X += i; 
	AVE_X = AVE_X / (down - TENDLIMIT + 1);
	
	for(i = down; i > TENDLIMIT; i--)
		AVE_Y += lineinf[i].MiddlePos;
	AVE_Y = AVE_Y / (down - TENDLIMIT + 1);
	
	for(i = down; i > TENDLIMIT; i--)
		aa += (i - AVE_X) * (lineinf[i].MiddlePos - AVE_Y);
	
	for(i = down; i > TENDLIMIT; i--)
		bb += (i - AVE_X) * (i - AVE_X);
	
	if(bb != 0)
		SLOPE = aa / bb;
	
	//return SLOPE;
	if (SLOPE > 0)
		return 1;
	if (SLOPE < 0)
		return 2;
	if(SLOPE == 0 )
		return 2;
}

float ReturnTendency3(uint8(*picture)[160])
{
#define TENDLIMIT3   (35)  
	int i = 0;
	
	float AVE_X = 0;
	float AVE_Y = 0;
	float aa = 0;
	float bb = 0;
	float SLOPE = 0;
	
	
	if(EFFECT >= TENDLIMIT3)
		return 0;
	for(i = down; i > TENDLIMIT3; i--)
		lineinf[i].MiddlePos = (lineinf[i].LeftPos + lineinf[i].RightPos) / 2;
	
	for(i = down; i > TENDLIMIT3; i--)
		AVE_X += i; 
	AVE_X = AVE_X / (down - TENDLIMIT3 + 1);
	
	for(i = down; i > TENDLIMIT3; i--)
		AVE_Y += lineinf[i].MiddlePos;
	AVE_Y = AVE_Y / (down - TENDLIMIT3 + 1);
	
	for(i = down; i > TENDLIMIT3; i--)
		aa += (i - AVE_X) * (lineinf[i].MiddlePos - AVE_Y);
	
	for(i = down; i > TENDLIMIT3; i--)
		bb += (i - AVE_X) * (i - AVE_X);
	
	if(bb != 0)
		SLOPE = aa / bb;
	
	return SLOPE;
	
}

float ReturnTendency4(uint8(*picture)[160])
{
#define TENDLIMIT4   (10)  
	int i = 0;
	
	float AVE_X = 0;
	float AVE_Y = 0;
	float aa = 0;
	float bb = 0;
	float SLOPE = 0;
	
	if(EFFECT >= TENDLIMIT4)
		return 0;
	
	for(i = down; i > TENDLIMIT4; i--)
		lineinf[i].MiddlePos = (lineinf[i].LeftPos + lineinf[i].RightPos) / 2;
	
	for(i = down; i > TENDLIMIT4; i--)
		AVE_X += i; 
	AVE_X = AVE_X / (down - TENDLIMIT4 + 1);
	
	for(i = down; i > TENDLIMIT4; i--)
		AVE_Y += lineinf[i].MiddlePos;
	AVE_Y = AVE_Y / (down - TENDLIMIT4 + 1);
	
	for(i = down; i > TENDLIMIT4; i--)
		aa += (i - AVE_X) * (lineinf[i].MiddlePos - AVE_Y);
	
	for(i = down; i > TENDLIMIT4; i--)
		bb += (i - AVE_X) * (i - AVE_X);
	
	if(bb != 0)
		SLOPE = aa / bb;
	
	return SLOPE;
	
}

int ReturnTendency5(uint8(*picture)[160])
{
	int slope = 0;
	int limit5 = 0;
	limit5 = (lineinf[down].MiddlePos + lineinf[down-1].MiddlePos + lineinf[down-2].MiddlePos)/3;
	for(int i = down; i > 50; i--)
	{
		if(lineinf[i].MiddlePos < limit5)
			slope--;
		else if(lineinf[i].MiddlePos > limit5)
			slope++;
	}
	if(slope > 0)
		return 2;
	else
		return 1;
	
}
float ReturnTendency6(uint8(*picture)[160], int high, int low)
{
	
	int i = 0;
	
	float AVE_X = 0;
	float AVE_Y = 0;
	float aa = 0;
	float bb = 0;
	float SLOPE = 0;
	
	for(i = low; i > high; i--)
		AVE_X += i; 
	AVE_X = AVE_X / (low - high + 1);
	
	for(i = low; i > high; i--)
		AVE_Y += lineinf[i].MiddlePos;
	AVE_Y = AVE_Y / (low - high + 1);
	
	for(i = low; i > high; i--)
		aa += (i - AVE_X) * (lineinf[i].MiddlePos - AVE_Y);
	
	for(i = low; i > high; i--)
		bb += (i - AVE_X) * (i - AVE_X);
	
	if(bb != 0)
		SLOPE = aa / bb;
	
	return SLOPE;
	
}

float ReturnTendency7(uint8(*picture)[160], int high, int low, int dir)
{
	
	int i = 0;
	
	float AVE_X = 0;
	float AVE_Y = 0;
	float aa = 0;
	float bb = 0;
	float SLOPE = 0;
	switch(dir)
	{
	case 1:
		for(i = low; i > high; i--)
			AVE_X += i; 
		AVE_X = AVE_X / (low - high + 1);
		
		for(i = low; i > high; i--)
			AVE_Y += lineinf[i].LeftPos;
		AVE_Y = AVE_Y / (low - high + 1);
		
		for(i = low; i > high; i--)
			aa += (i - AVE_X) * (lineinf[i].LeftPos - AVE_Y);
		
		for(i = low; i > high; i--)
			bb += (i - AVE_X) * (i - AVE_X);
		
		break;
	case 2:
		
		for(i = low; i > high; i--)
			AVE_X += i; 
		AVE_X = AVE_X / (low - high + 1);
		
		for(i = low; i > high; i--)
			AVE_Y += lineinf[i].RightPos;
		AVE_Y = AVE_Y / (low - high + 1);
		
		for(i = low; i > high; i--)
			aa += (i - AVE_X) * (lineinf[i].RightPos - AVE_Y);
		
		for(i = low; i > high; i--)
			bb += (i - AVE_X) * (i - AVE_X);
		break;
	default:
		break;
		
	}
	
	if(bb != 0)
		SLOPE = aa / bb;
	
	return SLOPE;
	
}

float ReturnTendency8(uint8(*picture)[160])
{
#define TENDLIMIT4   (15)  
	int i = 0;
	
	float AVE_X = 0;
	float AVE_Y = 0;
	float aa = 0;
	float bb = 0;
	float SLOPE = 0;
	
	if(EFFECT >= TENDLIMIT4)
		return 0;
	
	for(i = down; i > TENDLIMIT4; i--)
		AVE_X += i; 
	AVE_X = AVE_X / (down - TENDLIMIT4 + 1);
	
	for(i = down; i > TENDLIMIT4; i--)
		AVE_Y += lineinf[i].MiddlePos;
	AVE_Y = AVE_Y / (down - TENDLIMIT4 + 1);
	
	for(i = down; i > TENDLIMIT4; i--)
		aa += (i - AVE_X) * (lineinf[i].MiddlePos - AVE_Y);
	
	for(i = down; i > TENDLIMIT4; i--)
		bb += (i - AVE_X) * (i - AVE_X);
	
	if(bb != 0)
		SLOPE = aa / bb;
	
	return SLOPE;
	
}

/*========================================================================
*  ��������: CountLeftRight
*  ����˵����������������
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵������߶� ���� 1
�ұ߶� ���� 0
========================================================================*/
int CountLeftRight(int effect, uint8(*picture)[160])
{
	int i;
	int j = 10;
	for (i = effect; i < (effect + 10); i++)
	{
		if (lineinf[i].LeftPos == left)
			j--;
		if (lineinf[i].RightPos == right)
			j++;
		
		if (j < 8)
			return 1;
		if (j > 12)
			return 2;
	}
	return 0;
}


/*========================================================================
*  ��������: CountLine
*  ����˵����ѡ�мǺڵ���
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵��������ֵ �ڵ�ĸ���
========================================================================*/
int CountLine(int which, uint8(*picture)[160])
{
	int count = 0;
	
	for(int i = TrueLeft[which]; i <= TrueRight[which]; i++)
	{
		if(picture[which][i] == 0)
			count++;
	}
	return count;
}

int CountLine2(int which, uint8(*picture)[160])
{
	int count = 0;
	
	for(int i = TrueLeft[which]; i <= TrueRight[which]; i++)
	{
		if(picture[which][i] == 255)
			count++;
	}
	return count;
}


/*========================================================================
*  ��������: PatchLineByL/M/R
*  ����˵�����������ߣ�����߽� ���� �ұ߽�
PatchLineByL ---- ���߽����㺯��������linelnf.LeftPosֵ
PatchLineByM ---- ���߽߱����㺯��������linelnf.MiddlePosֵ
PatchLineByR ---- �Ҳ�߽����㺯��������linelnf.RightPosֵ
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
========================================================================*/
int PatchLineByL(int Down, int DownPos, int Up, int UpPos)
{
	int i = 0;
	
	if (up == down) 
		return 0;
	else
	{
		float DValue = 1.0 * (UpPos - DownPos) / (Down - Up);
		
		for (i = Down; i >= Up; i--)
		{
			lineinf[i].LeftPos = (int)(DownPos + (Down - i) * DValue);
			LimitPr(lineinf[i].LeftPos);
		}
	}
	return 0;
	
}


int PatchLineByR(int Down, int DownPos, int Up, int UpPos)
{
	int i = 0;
	if (up == down)
		return 0;
	else
	{
		float DValue = 1.0 * (UpPos - DownPos) / (Down - Up);
		
		for (i = Down; i >= Up; i--)
		{
			lineinf[i].RightPos = (int)(DownPos + (Down - i) * DValue);
			LimitPr(lineinf[i].RightPos);
		}
	}
	return 0;
	
}


int PatchLineByM(int Down, int DownPos, int Up, int UpPos)
{
	int i = 0;
	if (up == down) 
		return 0;
	else
	{
		float DValue = 1.0 * (UpPos - DownPos) / (Down - Up);
		
		for (i = Down; i >= Up; i--)
		{
			lineinf[i].MiddlePos = (int)(DownPos + (Down - i) * DValue);
			LimitPr(lineinf[i].MiddlePos);
		}
		
	}
	return 0;
	
}


/*========================================================================
*  ��������: CurveSlope
*  ����˵��������б��
*  ����:     ���巨��������б��
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵��������ֵ ��Ӧ������б�� 
========================================================================*/
float CurveSlope(int x, int pos[])
{
	float SUM_XY = 0;
	float SUM_X = 0;
	float SUM_Y = 0;
	float SUM_XX = 0;
	float SUM_X_SUM_X = 0;
	float a = 0;
	
	if(x < 2)
		return 0;
	
	SUM_XY = x*pos[x] + (x - 1)*pos[x - 1] + (x - 2)*pos[x - 2];
	SUM_X = x + x - 1 + x - 2;
	SUM_Y = pos[x] + pos[x - 1] + pos[x - 2];
	SUM_XX = x*x + (x - 1)*(x - 1) + (x - 2)*(x - 2);
	SUM_X_SUM_X = SUM_X * SUM_X;
	
	if (3 * SUM_XX - SUM_X_SUM_X == 0)
		return 0;
	a = (float)((3 * SUM_XY - SUM_X * SUM_Y) / (3 * SUM_XX - SUM_X_SUM_X));
	
	return a;
}


/*========================================================================
*  ��������: StructInit
*  ����˵������س�ʼ��
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵���� 
========================================================================*/
void StructInit()
{
	int i = 0;
	
	for (i = 0; i < 60; i++)
	{
		lineinf[i].Leftleft = 0;
		lineinf[i].Rightright = 0;
		BlackCircleLeft[i] = 80;
		BlackCircleRight[i] = 80;
	}
	
	LeftDownLow = 0;
	LeftDownHigh = 59;
	RightDownLow = 0;
	RightDownHigh = 59;
	RightCount = 0;
	LeftCount = 0;
	/*
	��·
	*/
	IsCircle = 0;
	CircleUp = 0;
	CircleDown = 0;
	CircleLeft = 159;
	CircleRight = 0;
	
	IsCurveLine = 0;
	
	IsSLine = 0;
	
	IsCross = 0;
	
	IsExceed = 0;
}


/*========================================================================
*  ��������: FindFirstPos            FindOtherPos
*  ����˵�����ҵ�һ�б߽�           �������б߽�
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵���� 
========================================================================*/
void FindFirstPos(uint8(*picture)[160])
{
	/*
	������ʼ��
	*/
	int  row = 80;    //����������
	
	/*
	����㷨
	*/
	for (row = 80; row >= left; row--)                                  //�ҵ�һ�е���߽��
	{
		if (picture[59][row] != picture[59][row + 1])                     //�ж��������Ƿ���ͬ�ķ�������߽��
		{
			if (picture[59][row + 1] == picture[59][row + 2])               //�˲�
			{
				lineinf[59].LeftPos = row;
				break;
			}
			else if (row == left)                                       //���û�����ж���߽��Ϊ��߽�
				lineinf[59].LeftPos = left;
			else {}
		}
		else if (row == left)                                            //���û�����ж���߽��Ϊ��߽�
			lineinf[59].LeftPos = left;
		else {}
	}
	for (row = 80; row <= right; row++)                                  //�ҵ�һ�е��ұ߽��
	{
		if (picture[59][row] != picture[59][row - 1])                      //�ж��������Ƿ���ͬ�ķ�������߽��
		{
			if (picture[59][row - 1] == picture[59][row - 2])                //�˲�
			{
				lineinf[59].RightPos = row;
				break;
			}
			else if (row == right)                                       //���û�����ж��ұ߽��Ϊ�ұ߽�
				lineinf[59].RightPos = right;
			else {}
		}
		else if (row == right)                                           //���û�����ж��ұ߽��Ϊ�ұ߽�
			lineinf[59].RightPos = right;
		else {}
	}
}


void FindOtherPos(int effect, uint8(*picture)[160])
{
	/*
	��ر���
	*/
	int column = 58;   //����������
	int row = 80;      //����������
	
	/*
	����㷨
	�㷨˵����������ص��Ϸ��ĵ����ɫ���жϱ߽硣���Ϊ��ɫ�����������Ѱ�ң���������Ա߽�Ѱ��
	����һ��������߽���˵���ܳ��ֺܶ���߽綼Ϊleftֵ����������Զ�����߽���˵��δ�ҵ��߽����(row == (left))��������ұ߽�ͬ��
	*/
	for (column = 58; column >= effect; column--)
	{
		if (picture[column][lineinf[column + 1].LeftPos] == 255)                     //�ж��Ϸ������ɫ����ɫ��
		{
			
			if (lineinf[column + 1].LeftPos == right)
				lineinf[column].LeftPos = right;
			else
			{
				for (row = lineinf[column + 1].LeftPos; row < right; row++)              //������߽���˵���Ϸ���Ϊ��ɫ��������Ѱ��
				{
					if (picture[column][row] != picture[column][row + 1])
					{
						lineinf[column].LeftPos = row;
						break;
					}
					else if (row == (right - 1) || (row == (right)))                      //���û���ҵ���ֵΪ�ұ߽�
						lineinf[column].LeftPos = right;
					else {}
				}
			}
			
		}
		else                                                                          //�ж��Ϸ������ɫ����ɫ��
		{
			for (row = lineinf[column + 1].LeftPos; row >= left; row--)               //������߽���˵���Ϸ���Ϊ��ɫ��������Ѱ��
			{
				if (picture[column][row] != picture[column][row + 1])
				{
					lineinf[column].LeftPos = row + 1;
					break;
				}
				else if ((row == (left + 1)) || (row == (left)))                      //���û���ҵ���ֵΪ��߽�
					lineinf[column].LeftPos = left;
				else {}
				
			}
		}
		//�ұ߽�Ҳ��һ���ģ���ע���ˣ�̫���ˣ� ^0^
		if (picture[column][lineinf[column + 1].RightPos] == 255)
		{
			if (lineinf[column + 1].RightPos == left)
				lineinf[column].RightPos = left;
			else
			{
				for (row = lineinf[column + 1].RightPos; row > left; row--)
				{
					if (picture[column][row] != picture[column][row - 1])
					{
						lineinf[column].RightPos = row;
						break;
					}
					else if (row == (left + 1) || (row == left))
						lineinf[column].RightPos = left;
					else {}
				}
			}
			
			
		}
		else
		{
			for (row = lineinf[column + 1].RightPos; row <= right; row++)
			{
				if (picture[column][row] != picture[column][row - 1])
				{
					lineinf[column].RightPos = row - 1;
					break;
				}
				else if ((row == (right - 1)) || (row == (right)))
					lineinf[column].RightPos = right;
				else {}
			}
		}
	}
}


/*========================================================================
*  ��������: EffectiveGet            
*  ����˵������ͼ����Ч��
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵��������ֵ ͼ�����Ч�� ��
*  ��ע   �������������д��
========================================================================*/
int EffectiveGet(int leftpos, int rightpos, uint8(*picture)[160])
{
	int h_count = 0, effective_temp = 59, effective_min = 59;
	int w = 0;
	
	for (w = leftpos; w <= rightpos; w++)
	{
		for (h_count = 59; h_count >= 0; h_count--)
		{
			if (picture[h_count][w] == 255 || h_count == 0)
			{
				effective_temp = h_count;
				break;
			}
		}
		if (effective_min > effective_temp)
		{
			effective_min = effective_temp;
		}
	}
	
	if (effective_min > 59) effective_min = 59;
	if (effective_min < 0) effective_min = 0;
	
	return effective_min;
}
int EffectiveGet2(uint8(*picture)[160])
{
	int i = 0;
	for(i = down; i > up; i--)
	{
		if(lineinf[i].LeftPos >= lineinf[i].RightPos || lineinf[i].LeftPos > 135 || lineinf[i].RightPos < 25)
			return i;
		if(i ==1)
			return 1;
	}
}

/*========================================================================
*  ��������: ʮ�� Сs            
*  ����˵����
*  ���˵����Cross_S_Line      ��   ��СS����д���Ч����һ��ѽ��
Cross_S_Line2     ��   ��СS����д���Ч�����ã�
CrossLineNormal   ��   ��ʮ�ֽ��д�������ֵ�Ƚϴ�С���бȽϵķ���������ô���ã�
CrossLineNormal2  ��   ��ʮ�ֽ��д�������б�ʴ�С���бȽϵķ���
CrossLineFeNormal ��   �Գ�ʮ�ֵ��ο����д��� 
BabyFind          ��   б��ʮ�ֲ���
BabyFind2         ��   б��ʮ�ֲ���
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
*  ��ע��    ����б�ʣ�����任
========================================================================*/
int BabyDownBigger = 0;
int BabyDownPosBigger = 0;
//��ص����Ҳ��������Ʒ�Χ��ֵ�ĺ궨��
#define SLOPERIGHTDOWN   (   0)
#define SLOPERIGHTUP     (   8)//5
#define SLOPELEFTDOWN    (  -8)//-5
#define SLOPELEFTUP      (   0)

#define SLOPERIGHTDOWN3   (   0)
#define SLOPERIGHTUP3     (   8)//5
#define SLOPELEFTDOWN3    (  -8)//-5
#define SLOPELEFTUP3      (   0)
int SLCount;
int SRCount;
int SCount;
int ZeroCount;

int SCrossCount = 0;
int SFlag = 0;
int SPos[60] = {0};int tendS = 0;float aa = 0;
int Cross_S_Find2(int effect, uint8(*picture)[160])
{
	SFlag = 0;
	SCrossCount = 0;
	int i = 0;
	
	for(i = down; i >= up; i--)
		SPos[i] = 0;
	
	tendS = ReturnTendency5(picture);
	
	if(tendS == 1)
		SFlag = 1;
	if(tendS == 2)
		SFlag = 2;
	
	for(i = 50; i > effect + 3; i--)
	{
		if(lineinf[i].MiddlePos - lineinf[i-1].MiddlePos <= 0 && lineinf[i-1].MiddlePos - lineinf[i-2].MiddlePos > 0
		   && lineinf[i-2].MiddlePos - lineinf[i-3].MiddlePos >= 0&&  SFlag== 2)
		{
			if(fabs(ReturnTendency6(picture, i-1, 59)) > 0.4 )
			{
				SPos[SCrossCount] = i-1;
				break;
			}
		}
		if(lineinf[i].MiddlePos - lineinf[i-1].MiddlePos >= 0 && lineinf[i-1].MiddlePos - lineinf[i-2].MiddlePos < 0
		   && lineinf[i-2].MiddlePos - lineinf[i-3].MiddlePos <= 0&& SFlag == 1)
		{
			if(fabs(ReturnTendency6(picture, i-1, 59)) > 0.4)
			{
				SPos[SCrossCount] = i-1;
				break;
			}
		}
		
		
		if(i == effect + 2 ) return 0;
	}
	
	for(i = SPos[0]; i > effect + 2; i--)
	{
		switch(SFlag)
		{
		case 1:
			if(SCrossCount % 2 == 0)
			{
				if((lineinf[i].MiddlePos - lineinf[i-1].MiddlePos) <= 0 && (lineinf[i-1].MiddlePos - lineinf[i-2].MiddlePos) > 0 && lineinf[i-2].MiddlePos - lineinf[i-3].MiddlePos >= 0
				   && SPos[SCrossCount] - i > 5 && fabs(lineinf[i-1].MiddlePos - lineinf[SPos[SCrossCount]].MiddlePos) > 5)
				{
					SCrossCount++;
					SPos[SCrossCount] = i-1;
				}
			}
			if(SCrossCount % 2 == 1)
			{
				if((lineinf[i].MiddlePos - lineinf[i-1].MiddlePos) >= 0 && (lineinf[i-1].MiddlePos - lineinf[i-2].MiddlePos) < 0 && lineinf[i-2].MiddlePos - lineinf[i-3].MiddlePos <= 0
				   && SPos[SCrossCount] - i > 5 && fabs(lineinf[i-1].MiddlePos - lineinf[SPos[SCrossCount]].MiddlePos) > 5)
				{  
					SCrossCount++;
					SPos[SCrossCount] = i-1;
				}
			}
			break;
		case 2:
			if(SCrossCount % 2 == 1)
			{
				if((lineinf[i].MiddlePos - lineinf[i-1].MiddlePos) <= 0 && (lineinf[i-1].MiddlePos - lineinf[i-2].MiddlePos) > 0 && lineinf[i-2].MiddlePos - lineinf[i-3].MiddlePos >= 0
				   && SPos[SCrossCount] - i > 5 && fabs(lineinf[i-1].MiddlePos - lineinf[SPos[SCrossCount]].MiddlePos) > 5)
				{
					SCrossCount++; 
					SPos[SCrossCount] = i-1;
				}
			}
			if(SCrossCount % 2 == 0)
			{
				if((lineinf[i].MiddlePos - lineinf[i-1].MiddlePos) >= 0 && (lineinf[i-1].MiddlePos - lineinf[i-2].MiddlePos) < 0 && lineinf[i-2].MiddlePos - lineinf[i-3].MiddlePos <= 0
				   && SPos[SCrossCount] - i > 5 && fabs(lineinf[i-1].MiddlePos - lineinf[SPos[SCrossCount]].MiddlePos) > 5)
				{ 
					SCrossCount++; 
					SPos[SCrossCount] = i-1;
				}
			}
			break;
		default:
			break;
			
		}
	}
	
	int IsOver = 0;
	int IsOverLR = 0;
//	for(i = down; i > effect+3; i--)
//	{
//		if(lineinf[i].LeftPos > 80 || lineinf[i].RightPos < 80)
//			IsOver++;
//		
//		if(lineinf[i].LeftPos >= Rdem[i] )
//			IsOverLR++;
//		if(lineinf[i].RightPos <= Ldem[i])
//			IsOverLR++;
//	}
//	
	if(effect < 2 && SCrossCount >= 1 && StructCircle == 0 && IsCross == 0)//&& IsOver == 0 && IsOverLR == 0)
		SCrossCount = 5;
//        if(SCrossCount == 5 && IsOver == 0 && IsOverLR == 0)
//              SCrossCount = 6;
	
}


void Cross_S_Line2(int effect, uint8(*picture)[160])
{
	int  CSUP = effect + 8;
	int i;
	
	if (IsSLine == 1)
	{
		if(down != CSUP)
			PatchLineByM(down, 80, CSUP, (lineinf[CSUP].LeftPos + lineinf[CSUP].RightPos) / 2);
		for(i = CSUP; i > effect; i--)
			lineinf[i].MiddlePos = (left + right) / 2;
	}
}

void Cross_S_Line3(int effect, uint8(*picture)[160])
{
	for(int i = down; i > effect; i--)
	{
		lineinf[i].MiddlePos = (lineinf[i].MiddlePos + 80) / 2;
	}
}

void Cross_S_Line4(int effect, uint8(*picture)[160])
{
	float SUMXX = 0;
	float SUMY = 0;
	float SUMX = 0;
	float SUMXY = 0;
	float SUMX_2 = 0;
	float a = 0;
	float b = 0;
	
	int i = 0;
	
	for (i = down; i > effect + 2; i--)  SUMXX += i*i;
	for (i = down; i > effect + 2; i--)  SUMY  += lineinf[i].MiddlePos;
	for (i = down; i > effect + 2; i--)  SUMX  += i;
	for (i = down; i > effect + 2; i--)  SUMXY += i * lineinf[i].MiddlePos;
	SUMX_2 = SUMX * SUMX;
	
	b = (SUMXX * SUMY - SUMX * SUMXY) / ((down - effect - 2) * SUMXX - SUMX_2);
	a = ((down - effect - 2) * SUMXY - SUMX * SUMY) / ((down - effect - 2) * SUMXX - SUMX_2);
	
	for (i = down; i > effect + 2; i--)
		lineinf[i].MiddlePos = (int)((float)(a * i) + b);
}


int RCrossDown = 0;
int RCrossUp = 0;
int LCrossDown = 0;
int LCrossUp = 0;
int IsDoing2 = 0;
int IsDoing = 0;
int RightTend = 0;
int LeftTend = 0;
int IsCross2 = 0;
int LeftCrossFePos = 58;   //����Ҫ������־��
int RightCrossFePos = 58;  //����Ҫ���Ҳ��־��
int CrossLineNormal2(int effect, uint8(*picture)[160])
{
	
#define feature (3)
#define limit   (10)
#define LLLIMIT  (20)
#define RRLIMIT  (140)
	
	int i;
	int j;
	int IsDoingFlag = 0;
	int IsDoingFlag2 = 0;
	IsCross2 = 0;
	int LCrossFlag = 0;
	int RCrossFlag = 0;
	LCrossDown = 0;
	LCrossUp = 0;
	int LCrossCount = 0;
	RCrossDown = 0;
	RCrossUp = 0;
	int RCrossCount = 0;
	int RCount = 0;
	int LCount = 0;
	IsDoing2 = 0;
	IsDoing = 0;
	int LEFT[60] = { left };
	int RIGHT[60] = { right };
	
	RightTend = ReturnTendency(picture, 2);
	LeftTend  = ReturnTendency(picture, 1);
	
	
	IsCross = 0;
	
	if (effect > 12)                        //�ж��Ƿ���Խ���ʮ�ּ�����
		return 0;
	
	for (i = down; i > up; i--)
	{
		LEFT[i] = left;
		RIGHT[i] = right;
		
	}
	for (i = down; i > effect; i--)
	{
		LEFT[i] = lineinf[i].LeftPos;
		RIGHT[i] = lineinf[i].RightPos;
	}
	
	for(i = down; i > 14; i--)
	{
		if(LEFT[i] < 4)
			IsDoingFlag++;
		if(RIGHT[i] > 155)
			IsDoingFlag2++;
	}
	if(IsDoingFlag > 43)
		IsDoingFlag = 100;
	if(IsDoingFlag2 > 43)
		IsDoingFlag2 = 100;
		
	/*
	����ʮ�����߳���
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //���ʮ�����½ǵ�ֵ
	{
		if ((((lineinf[i].LeftPos - lineinf[i - 1].LeftPos) <= 0) && ((lineinf[i - 1].LeftPos - lineinf[i - 2].LeftPos) > 0)))
		{
			LCrossDown = i-1;
			LCrossFlag = 1;
			break;
		}
	}
	
	if (0 == TriFind(effect, picture, 1) && LCrossFlag == 0)
	{
		if (TriFlag2 == 1)
			LCrossDown = TriPos;
		else
			LCrossDown = down;//TriDown; //down
	}
	
	if ( LCrossDown != 0 && lineinf[LCrossDown].LeftPos != left 
		||  LCrossDown != 0 && lineinf[LCrossDown].LeftPos == left && RightTend < 0 )                  //���ʮ�����Ͻǵ�ֵ
	{
		for (i = LCrossDown; i > effect + 3; i--)
		{
			if ((CurveSlope(i, LEFT) > SLOPELEFTDOWN && CurveSlope(i, LEFT) < SLOPELEFTUP) && (lineinf[i].LeftPos >= ReturnBigger(lineinf[LCrossDown].LeftPos - 15, left))
				&&lineinf[i].LeftPos != left && lineinf[i].LeftPos != left + 1)
			{
				LCrossUp = ReturnBigger(effect, i-1);
				break;
			}
			if (i == effect + 8) //6
			{
				LCrossUp = effect + 8; //6
				IsDoing = 1;
				break;
			}
		}
		
		
	}
	/*******************************************************************/
	if(LCrossDown != 0 && LCrossUp != 0 && ((lineinf[LCrossDown].LeftPos - lineinf[LCrossUp].LeftPos > 20) || (LCrossDown - LCrossUp > 48)))
		IsDoing = 1;
	/*******************************************************************/
	if ((LCrossDown != 0) && (LCrossUp != 0)) //���ʮ�ּ�������
	{
		for (i = LCrossDown; i > LCrossUp; i--)
		{
			if (lineinf[i].LeftPos <= left + 2)
				LCrossCount++;
		}
	}
	
	/*
	�Ҳ��ʮ�����߳���
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //���ʮ�����½ǵ�ֵ
	{
		if ((((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0) && ((lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0)))
		{
			if((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0 && (lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0)
			{
				RCrossDown = i-1;
				RCrossFlag = 1;
				break;
			}
		}
		
	}
	if (0 == TriFind(effect, picture, 2) && RCrossFlag == 0)
	{
		if (TriFlag2 == 1)
			RCrossDown = TriPos;
		else
			RCrossDown = down;//TriDown; //down
	}
	if ( RCrossDown != 0  && lineinf[RCrossDown].RightPos != right || 
		RCrossDown != 0  && lineinf[RCrossDown].RightPos == right && LeftTend > 0)                 //���ʮ�����Ͻǵ�ֵ
	{
		for (i = RCrossDown; i > effect + 3; i--)
		{
			if ((CurveSlope(i, RIGHT) > SLOPERIGHTDOWN && CurveSlope(i, RIGHT) < SLOPERIGHTUP) && (lineinf[i].RightPos <= ReturnSmaller(lineinf[RCrossDown].RightPos + 15, right))
				&&lineinf[i].RightPos != right && lineinf[i].RightPos != right - 1)
			{
				RCrossUp = ReturnBigger(effect, i-1);
				break;
			}
			if (i == effect + 8)  //6
			{
				IsDoing2 = 1;
				RCrossUp = effect + 8; //6
				break;
			}
		}
	}
	
	if ((RCrossDown != 0) && (RCrossUp != 0)) //�Ҳ�ʮ�ּ�������
	{
		for (i = RCrossDown; i > RCrossUp; i--)
		{
			if (lineinf[i].RightPos >= right - 2)
				RCrossCount++;
		}
	}
	
	/*********************************************************************/
	if(RCrossDown != 0 && RCrossUp != 0 && ((lineinf[RCrossDown].RightPos - lineinf[RCrossUp].RightPos < -20) || (RCrossDown - RCrossUp > 48)))
		IsDoing2 = 1;
	/*********************************************************************/
	/*
	���߼�����
	*/
	
	
	if (IsDoing == 1 )
		BabyFind(effect, picture, LCrossDown, lineinf[LCrossDown].LeftPos, 1);
	if (IsDoing2 == 1 )
		BabyFind(effect, picture, RCrossDown, lineinf[RCrossDown].RightPos, 2);
	
	return 0;
}



int CrossLineNormal3(int effect, uint8(*picture)[160])
{
	
	
#define feature (3)
#define limit   (15)
#define LLLIMIT  (20)
#define RRLIMIT  (140)
	
	int i;
	int j;
	int IsDoingFlag = 0;
	int IsDoingFlag2 = 0;
	IsCross2 = 0;
	int LCrossFlag = 0;
	int RCrossFlag = 0;
	LCrossDown = 0;
	LCrossUp = 0;
	int LCrossCount = 0;
	RCrossDown = 0;
	RCrossUp = 0;
	int RCrossCount = 0;
	int RCount = 0;
	int LCount = 0;
	IsDoing2 = 0;
	IsDoing = 0;
	int LEFT[60] = { left };
	int RIGHT[60] = { right };
	
	RightTend = ReturnTendency(picture, 2);
	LeftTend  = ReturnTendency(picture, 1);
	
	
	IsCross = 0;
	
	if (effect > 12)                        //�ж��Ƿ���Խ���ʮ�ּ�����
		return 0;
	
	for (i = down; i > up; i--)
	{
		LEFT[i] = left;
		RIGHT[i] = right;
		
	}
	for (i = down; i > effect; i--)
	{
		LEFT[i] = lineinf[i].LeftPos;
		RIGHT[i] = lineinf[i].RightPos;
	}
	
	for(i = down; i > 14; i--)
	{
		if(LEFT[i] < 4)
			IsDoingFlag++;
		if(RIGHT[i] > 155)
			IsDoingFlag2++;
	}
	if(IsDoingFlag > 43)
		IsDoingFlag = 100;
	if(IsDoingFlag2 > 43)
		IsDoingFlag2 = 100;
	
	
	for(i = down; i > effect + 5; i--)
	{
		if(lineinf[i].LeftPos < 3 || lineinf[i].RightPos > 156)
		{
			IsCross2 = 1;
			break;
		}
	}
	if(IsCross2 == 0)
		return 0;
	
	
	/*
	����ʮ�����߳���
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //���ʮ�����½ǵ�ֵ
	{
		if ((((lineinf[i].LeftPos - lineinf[i - 1].LeftPos) <= 0) && ((lineinf[i - 1].LeftPos - lineinf[i - 2].LeftPos) > 0)))
		{
			LCrossDown = i-1;
			LCrossFlag = 1;
			break;
		}
	}
	
	if (0 == TriFind(effect, picture, 1) && (LCrossFlag == 0 || (LCrossFlag == 1 && LCrossDown < 25 && LCrossDown != 0 && lineinf[LCrossDown].LeftPos < 10)))
	{
		if (TriFlag2 == 1)
			LCrossDown = TriPos;
		else
			LCrossDown = down;
	}
	
	if ( LCrossDown != 0 && lineinf[LCrossDown].LeftPos != left 
		||  LCrossDown != 0 && lineinf[LCrossDown].LeftPos == left && (RightTend < 0 || LCrossDown > 50))                  //���ʮ�����Ͻǵ�ֵ
	{
		for (i = LCrossDown; i > effect + 3; i--)
		{
			if ((CurveSlope(i, LEFT) > SLOPELEFTDOWN && CurveSlope(i, LEFT) < SLOPELEFTUP) && (lineinf[i].LeftPos >= ReturnBigger(lineinf[LCrossDown].LeftPos - 15, left))
				&&lineinf[i].LeftPos != left && lineinf[i].LeftPos != left + 1)
			{
				LCrossUp = ReturnBigger(effect, i-1);
				break;
			}
			if (i == effect + 8) //6
			{
				LCrossUp = effect + 8; //6
				IsDoing = 1;
				break;
			}
		}
		
		
	}
	/*******************************************************************/
	if((LCrossDown != 0 && LCrossUp != 0 && ((lineinf[LCrossDown].LeftPos - lineinf[LCrossUp].LeftPos > 20) || (LCrossDown - LCrossUp > 48)))
	   ||((LCrossDown - LCrossUp)> 30 && lineinf[LCrossUp].LeftPos - lineinf[LCrossDown].LeftPos < 18 && ((LCrossDown > 55 && lineinf[LCrossDown].LeftPos < 40) || lineinf[LCrossDown].LeftPos < 30) && (lineinf[LCrossUp].RightPos - lineinf[LCrossUp].LeftPos) > 110))
		IsDoing = 1;
	/*******************************************************************/
	if ((LCrossDown != 0) && (LCrossUp != 0)) //���ʮ�ּ�������
	{
		for (i = LCrossDown; i > LCrossUp; i--)
		{
			if (lineinf[i].LeftPos <= left + 2)
				LCrossCount++;
		}
	}
	
	/*
	�Ҳ��ʮ�����߳���
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //���ʮ�����½ǵ�ֵ
	{
		if ((((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0) && ((lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0)))
		{
			RCrossDown = i-1;
			RCrossFlag = 1;
			break;
			
		}
	}
	if (0 == TriFind(effect, picture, 2) && (RCrossFlag == 0 || (RCrossFlag == 1 && RCrossDown < 25 && RCrossDown != 0 && lineinf[RCrossDown].RightPos > 150)))
	{
		if (TriFlag2 == 1)
			RCrossDown = TriPos;
		else
			RCrossDown = down;
	}
	
	if ( RCrossDown != 0  && lineinf[RCrossDown].RightPos != right || 
		RCrossDown != 0  && lineinf[RCrossDown].RightPos == right && (LeftTend > 0 || RCrossDown > 50))                 //���ʮ�����Ͻǵ�ֵ
	{
		for (i = RCrossDown; i > effect + 3; i--)
		{
			if ((CurveSlope(i, RIGHT) > SLOPERIGHTDOWN && CurveSlope(i, RIGHT) < SLOPERIGHTUP) && (lineinf[i].RightPos <= ReturnSmaller(lineinf[RCrossDown].RightPos + 15, right))
				&&lineinf[i].RightPos != right && lineinf[i].RightPos != right - 1)
			{
				RCrossUp = ReturnBigger(effect, i-1);
				break;
			}
			if (i == effect + 8)  
			{
				IsDoing2 = 1;
				RCrossUp = effect + 8; 
				break;
			}
		}
	}
	
	if ((RCrossDown != 0) && (RCrossUp != 0)) //�Ҳ�ʮ�ּ�������
	{
		for (i = RCrossDown; i > RCrossUp; i--)
		{
			if (lineinf[i].RightPos >= right - 2)
				RCrossCount++;
		}
	}
	
	/*********************************************************************/
	if((RCrossDown != 0 && RCrossUp != 0 && ((lineinf[RCrossDown].RightPos - lineinf[RCrossUp].RightPos < -20) || (RCrossDown - RCrossUp > 48)))
	   ||((RCrossDown - RCrossUp)> 30 && (lineinf[RCrossDown].RightPos - lineinf[RCrossUp].RightPos) < 18 && ( (RCrossDown > 55 && lineinf[RCrossDown].RightPos > 120) || lineinf[RCrossDown].RightPos > 130) && (lineinf[RCrossUp].RightPos - lineinf[RCrossUp].LeftPos) > 110))
		IsDoing2 = 1;
	/*********************************************************************/
	/*
	���߼�����
	*/
	
	if (RCrossCount > 3 && IsDoing2 == 0)
	{
		if(RCrossDown != RCrossUp)
			PatchLineByR(RCrossDown, lineinf[RCrossDown].RightPos, RCrossUp-1, lineinf[RCrossUp-1].RightPos);
		IsCross = 1;
	}
	if (LCrossCount > 3 && IsDoing == 0)
	{
		IsCross = 1;
		if(LCrossDown != LCrossUp)
			PatchLineByL(LCrossDown, lineinf[LCrossDown].LeftPos, LCrossUp-1, lineinf[LCrossUp-1].LeftPos);
	}
	if (IsDoing == 1 && ((lineinf[RCrossDown].RightPos > 90 && RCrossDown != 0) || RCrossDown == 0) &&(RightTend < 1.1 ||  (LCrossDown != 0 && RCrossDown != 0) || IsDoingFlag2 == 100|| RightCrossFePos != 58) )
	{
		IsCross = 1;
		if (1==BabyFind(effect, picture, LCrossDown, lineinf[LCrossDown].LeftPos, 1))
		{
			if(BabyDownBigger > effect + 1 && LCrossDown - BabyDownBigger < 45)
			{
				if((abs(BabyDownPosBigger - lineinf[BabyDownBigger].RightPos) > 3 ) && BabyDownBigger != LCrossDown)
				{
					
					PatchLineByL(LCrossDown, lineinf[LCrossDown].LeftPos, BabyDownBigger, BabyDownPosBigger);
					if (BabyDownPosBigger == right) {}
					else
					{
						lineinf[BabyDownBigger].LeftPos = BabyDownPosBigger;
						for (i = BabyDownBigger; i > effect + 1; i--)
						{
							if (picture[i - 1][lineinf[i].LeftPos] == 255)
							{
								for (j = lineinf[i].LeftPos; j < right; j++)
								{
									if (picture[i][j] == 255 && picture[i][j + 1] == 0)
									{
										lineinf[i - 1].LeftPos = j;
										break;
									}
									if (j == (right - 1))
										lineinf[i - 1].LeftPos = right;
								}
							}
						}
						if(LCrossDown != BabyDownBigger-2)
							PatchLineByL(LCrossDown, lineinf[LCrossDown].LeftPos, BabyDownBigger-2, lineinf[BabyDownBigger-2].LeftPos);
					}
				}
			}
		}
	}
	if (IsDoing2 == 1 && ((lineinf[LCrossDown].LeftPos < 70 && LCrossDown != 0) || LCrossDown == 0) &&(LeftTend > -1.1 || (LCrossDown != 0 && RCrossDown != 0) || IsDoingFlag == 100 || LeftCrossFePos != 58))
	{
		IsCross = 1;
		if (1==BabyFind(effect, picture, RCrossDown, lineinf[RCrossDown].RightPos, 2))
		{
			if(BabyDownBigger > effect + 1 && RCrossDown - BabyDownBigger < 45)
			{
				
				if((abs(BabyDownPosBigger - lineinf[BabyDownBigger].LeftPos) > 3 ) && (BabyDownBigger != RCrossDown))
				{
					PatchLineByR(RCrossDown, lineinf[RCrossDown].RightPos, BabyDownBigger, BabyDownPosBigger);
					if (BabyDownPosBigger == left ) {}
					else
					{
						lineinf[BabyDownBigger].RightPos = BabyDownPosBigger;
						for (i = BabyDownBigger; i > effect + 1; i--)
						{
							if (picture[i - 1][lineinf[i].RightPos] == 255)
							{
								for (j = lineinf[i].RightPos; j > left; j--)
								{
									if (picture[i][j] == 255 && picture[i][j - 1] == 0)
									{
										lineinf[i - 1].RightPos = j;
										break;
									}
									if (j == (left + 1))
										lineinf[i - 1].RightPos = left;
								}
							}
						}
						if(RCrossDown != BabyDownBigger-2)
							PatchLineByR(RCrossDown, lineinf[RCrossDown].RightPos, BabyDownBigger-2, lineinf[BabyDownBigger-2].RightPos);
					}
				}
				
			}
		}
	}
	return 0;
}


int CrossLineNormal4(int effect, uint8(*picture)[160], int dir)
{
	
	int i;
	int j;
	int IsDoingFlag = 0;
	int IsDoingFlag2 = 0;
	IsCross2 = 0;
	int LCrossFlag = 0;
	int RCrossFlag = 0;
	LCrossDown = 0;
	LCrossUp = 0;
	int LCrossCount = 0;
	RCrossDown = 0;
	RCrossUp = 0;
	int RCrossCount = 0;
	int RCount = 0;
	int LCount = 0;
	IsDoing2 = 0;
	IsDoing = 0;
	int LEFT[60] = { left };
	int RIGHT[60] = { right };
	
	RightTend = ReturnTendency(picture, 2);
	LeftTend  = ReturnTendency(picture, 1);
	
	
	IsCross = 0;
	
	if (effect > 12)                        //�ж��Ƿ���Խ���ʮ�ּ�����
		return 0;
	
	for (i = down; i > up; i--)
	{
		LEFT[i] = left;
		RIGHT[i] = right;
		
	}
	for (i = down; i > effect; i--)
	{
		LEFT[i] = lineinf[i].LeftPos;
		RIGHT[i] = lineinf[i].RightPos;
	}
	
	for(i = down; i > 14; i--)
	{
		if(LEFT[i] == left)
			IsDoingFlag++;
		if(RIGHT[i] == right)
			IsDoingFlag2++;
	}
	if(IsDoingFlag > 43)
		IsDoingFlag = 100;
	if(IsDoingFlag2 > 43)
		IsDoingFlag2 = 100;
	
	
	for(i = down; i > effect + 5; i--)
	{
		if(lineinf[i].LeftPos < 3 || lineinf[i].RightPos > 156)
		{
			IsCross2 = 1;
			break;
		}
	}
	if(IsCross2 == 0)
		return 0;
	
	
	/*
	����ʮ�����߳���
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //���ʮ�����½ǵ�ֵ
	{
		if ((((lineinf[i].LeftPos - lineinf[i - 1].LeftPos) <= 0) && ((lineinf[i - 1].LeftPos - lineinf[i - 2].LeftPos) > 0)))
		{
			LCrossDown = i-1;
			LCrossFlag = 1;
			break;
		}
	}
	
	if (0 == TriFind(effect, picture, 1) && LCrossFlag == 0)
	{
		if (TriFlag2 == 1)
			LCrossDown = TriPos;
		else
			LCrossDown = down;
	}
	
	
	
	/*
	�Ҳ��ʮ�����߳���
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //���ʮ�����½ǵ�ֵ
	{
		if ((((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0) && ((lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0)))
		{
			if((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0 && (lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0)
			{
				RCrossDown = i-1;
				RCrossFlag = 1;
				break;
			}
		}
		
	}
	if (0 == TriFind(effect, picture, 2) && RCrossFlag == 0)
	{
		if (TriFlag2 == 1)
			RCrossDown = TriPos;
		else
			RCrossDown = down;
	}
	
	
	//===========================================================================================================================//
	if (( LCrossDown != 0 && lineinf[LCrossDown].LeftPos != left 
		 ||  LCrossDown != 0 && lineinf[LCrossDown].LeftPos == left && RightTend < 0 ) && RCrossDown > 10 && (IsDoingFlag2 != 0 || lineinf[effect+1].RightPos > 80))                  //���ʮ�����Ͻǵ�ֵ
	{
		for (i = LCrossDown; i > effect + 3; i--)
		{
			if ((CurveSlope(i, LEFT) > SLOPELEFTDOWN && CurveSlope(i, LEFT) < SLOPELEFTUP) && (lineinf[i].LeftPos >= ReturnBigger(lineinf[LCrossDown].LeftPos - 15, left))
				&&lineinf[i].LeftPos != left && lineinf[i].LeftPos != left + 1)
			{
				LCrossUp = ReturnBigger(effect, i-1);
				break;
			}
			if (i == effect + 8) //6
			{
				LCrossUp = effect + 8; //6
				IsDoing = 1;
				break;
			}
		}
		
		
	}
	
	if((lineinf[LCrossDown].LeftPos - lineinf[LCrossUp].LeftPos > 20) && LCrossUp != 0)
		IsDoing = 1;
	
	if ((LCrossDown != 0) && (LCrossUp != 0)) //���ʮ�ּ�������
	{
		for (i = LCrossDown; i > LCrossUp; i--)
		{
			if (lineinf[i].LeftPos <= left + 2)
				LCrossCount++;
		}
	}
	
	//============================================================================================================================//
	if (( RCrossDown != 0  && lineinf[RCrossDown].RightPos != right || 
		 RCrossDown != 0  && lineinf[RCrossDown].RightPos == right && LeftTend > 0) && LCrossDown > 10 && (IsDoingFlag != 0 ||lineinf[effect+1].LeftPos < 80))                //���ʮ�����Ͻǵ�ֵ
	{
		for (i = RCrossDown; i > effect + 3; i--)
		{
			if ((CurveSlope(i, RIGHT) > SLOPERIGHTDOWN && CurveSlope(i, RIGHT) < SLOPERIGHTUP) && (lineinf[i].RightPos <= ReturnSmaller(lineinf[RCrossDown].RightPos + 15, right))
				&&lineinf[i].RightPos != right && lineinf[i].RightPos != right - 1)
			{
				RCrossUp = ReturnBigger(effect, i-1);
				break;
			}
			if (i == effect + 8)  //6
			{
				IsDoing2 = 1;
				RCrossUp = effect + 8; //6
				break;
			}
		}
	}
	
	if ((RCrossDown != 0) && (RCrossUp != 0)) //�Ҳ�ʮ�ּ�������
	{
		for (i = RCrossDown; i > RCrossUp; i--)
		{
			if (lineinf[i].RightPos >= right - 2)
				RCrossCount++;
		}
	}
	
	if((lineinf[RCrossDown].RightPos - lineinf[RCrossUp].RightPos < -20) && RCrossUp != 0)
		IsDoing2 = 1;
	
	/*
	���߼�����
	*/
	
	if (RCrossCount > 3 && IsDoing2 == 0)
	{
		switch(dir)
		{
		case 1:
			if(down != RCrossUp)
				PatchLineByR(down, lineinf[down].RightPos, RCrossUp, lineinf[RCrossUp].RightPos);
			IsCross = 1;
			break;
		case 2: 
			//                  if(RCrossDown != RCrossUp)
			//                    PatchLineByR(RCrossDown, lineinf[RCrossDown].RightPos, RCrossUp, lineinf[RCrossUp].RightPos);
			//                    IsCross = 1;
			break;
		default:
			break;
		}
		
	}
	if (LCrossCount > 3 && IsDoing == 0)
	{
		switch(dir)
		{
		case 1:
			//                  if(LCrossDown != LCrossUp)
			//                      PatchLineByL(LCrossDown, lineinf[LCrossDown].LeftPos, LCrossUp, lineinf[LCrossUp].LeftPos);
			//                   IsCross = 1;
			break;
		case 2: 
			if(LCrossUp != down)
				PatchLineByL(down, lineinf[down].LeftPos, LCrossUp, lineinf[LCrossUp].LeftPos);
			IsCross = 1;
			break;
		default:
			break;
		}
		
	}
	if (IsDoing == 1 && (RightTend < 1.1 ||  (LCrossDown != 0 && RCrossDown != 0) || IsDoingFlag2 == 100|| RightCrossFePos != 58))
	{
		
		IsCross = 1;
		if (BabyFind(effect, picture, LCrossDown, lineinf[LCrossDown].LeftPos, 1))
		{
			switch(dir)
			{
			case 1:
				if(LCrossDown != BabyDownBigger)
					PatchLineByR(down, lineinf[down].RightPos, BabyDownBigger, BabyDownPosBigger);
				if (BabyDownPosBigger == left ) {}
				else
				{
					lineinf[BabyDownBigger].RightPos = BabyDownPosBigger;
					for (i = BabyDownBigger; i > effect + 1; i--)
					{
						if (picture[i - 1][lineinf[i].RightPos] == 255)
						{
							for (j = lineinf[i].RightPos; j > left; j--)
							{
								if (picture[i][j] == 255 && picture[i][j - 1] == 0)
								{
									lineinf[i - 1].RightPos = j;
									break;
								}
								if (j == (left + 1))
									lineinf[i - 1].RightPos = left;
							}
						}
					}
				}
				break;
			case 2: 
				if(BabyDownBigger != down)
					PatchLineByL(LCrossDown, lineinf[LCrossDown].LeftPos, BabyDownBigger, BabyDownPosBigger);
				if (BabyDownPosBigger == right) {}
				else
				{
					lineinf[BabyDownBigger].LeftPos = BabyDownPosBigger;
					for (i = BabyDownBigger; i > effect + 1; i--)
					{
						if (picture[i - 1][lineinf[i].LeftPos] == 255)
						{
							for (j = lineinf[i].LeftPos; j < right; j++)
							{
								if (picture[i][j] == 255 && picture[i][j + 1] == 0)
								{
									lineinf[i - 1].LeftPos = j;
									break;
								}
								if (j == (right - 1))
									lineinf[i - 1].LeftPos = right;
							}
						}
					}
				}
				break;
			default:
				break;
			}
			
		}
	}
	
	if (IsDoing2 == 1 && (LeftTend > -1.1 || (LCrossDown != 0 && RCrossDown != 0) || IsDoingFlag == 100 || LeftCrossFePos != 58))
	{
		IsCross = 1;
		if (BabyFind(effect, picture, RCrossDown, lineinf[RCrossDown].RightPos, 2))
		{
			switch(dir)
			{
			case 1:
				if(LCrossDown != BabyDownBigger)
					PatchLineByR(RCrossDown, lineinf[RCrossDown].RightPos, BabyDownBigger, BabyDownPosBigger);
				if (BabyDownPosBigger == left ) {}
				else
				{
					lineinf[BabyDownBigger].RightPos = BabyDownPosBigger;
					for (i = BabyDownBigger; i > effect + 1; i--)
					{
						if (picture[i - 1][lineinf[i].RightPos] == 255)
						{
							for (j = lineinf[i].RightPos; j > left; j--)
							{
								if (picture[i][j] == 255 && picture[i][j - 1] == 0)
								{
									lineinf[i - 1].RightPos = j;
									break;
								}
								if (j == (left + 1))
									lineinf[i - 1].RightPos = left;
							}
						}
					}
				}
				break;
			case 2: 
				if(BabyDownBigger != down)
					PatchLineByL(down, lineinf[down].LeftPos, BabyDownBigger, BabyDownPosBigger);
				if (BabyDownPosBigger == right) {}
				else
				{
					lineinf[BabyDownBigger].LeftPos = BabyDownPosBigger;
					for (i = BabyDownBigger; i > effect + 1; i--)
					{
						if (picture[i - 1][lineinf[i].LeftPos] == 255)
						{
							for (j = lineinf[i].LeftPos; j < right; j++)
							{
								if (picture[i][j] == 255 && picture[i][j + 1] == 0)
								{
									lineinf[i - 1].LeftPos = j;
									break;
								}
								if (j == (right - 1))
									lineinf[i - 1].LeftPos = right;
							}
						}
					}
				}
				break;
			default:
				break;
			}
			
		}
	}
	return 0;
}


int CrossLineFeNormal(int effect, uint8(*picture)[160])
{
	int RCount2 = 0;           //�Ҳ��¼��������
	int LCount2 = 0;           //����¼��������
	LeftCrossFePos = 58;   //����Ҫ������־��
	RightCrossFePos = 58;  //����Ҫ���Ҳ��־��
	int Count = 0;             //�ܵ��¼��������
	int LEFT[60] = { left };   //���
	int RIGHT[60] = { right };//�ұ�
	
	int i;
	
	/*
	������
	�ж��Ƿ��������ʮ�ִ���
	*/
	
	if(effect > 18)
		return 0;
	
	
	if (lineinf[down - 0].LeftPos == left && lineinf[down - 0].RightPos == right)
		Count++;
	if (lineinf[down - 1].LeftPos == left && lineinf[down - 1].RightPos == right)
		Count++;
	if (lineinf[down - 2].LeftPos == left && lineinf[down - 2].RightPos == right)
		Count++;
	if (lineinf[down - 3].LeftPos == left && lineinf[down - 3].RightPos == right)
		Count++;
	if (lineinf[down - 4].LeftPos == left && lineinf[down - 4].RightPos == right)
		Count++;
	
	for (i = down; i > down - 20; i--)
	{
		if (lineinf[i].LeftPos == left && lineinf[down].LeftPos == left)
			LCount2++;
		if (lineinf[i].RightPos == right && lineinf[down].RightPos == right)
			RCount2++;
	}
	
	if (LCount2 > 1 || RCount2 > 1) {}
	else if (Count < 2)
		return 0;
	
	/*
	����ʮ�ֱ�־��Ѱ��
	*/
	for (i = down; i > effect; i--) //���߽�ֵ
	{
		LEFT[i] = lineinf[i].LeftPos;
		RIGHT[i] = lineinf[i].RightPos;
	}
	if (Count > 2 || LCount2 > 1)  //Ѱ������־��
	{
		for (i = down; i > 13; i--)
		{
			if (CurveSlope(i, LEFT) > SLOPELEFTDOWN3 && CurveSlope(i, LEFT) < SLOPELEFTUP3 && lineinf[i].LeftPos != left && lineinf[down].LeftPos == left)
			{
				LeftCrossFePos = i-1;
				break;
			}
		}
	}
	if (Count > 2 || RCount2 > 1)  //Ѱ���Ҳ��־��
	{
		for (i = down; i > 13; i--)
		{
			if (CurveSlope(i, RIGHT) > SLOPERIGHTDOWN3 && CurveSlope(i, RIGHT) < SLOPERIGHTUP3 && lineinf[i].RightPos != right && lineinf[down].RightPos == right)
			{
				RightCrossFePos = i-1;
				break;
			}
		}
	}
	/*
	����ʮ�ֲ���
	*/
	if (picture[down][left] == 0 && picture[down][right] == 0 && RCount2 > 3 && LCount2 > 3) //���߶�����
	{
		if(down != LeftCrossFePos)
			PatchLineByL(down, lineinf[down].LeftPos, LeftCrossFePos-1, lineinf[LeftCrossFePos-1].LeftPos);
		if(down != RightCrossFePos)
			PatchLineByR(down, lineinf[down].RightPos, RightCrossFePos-1, lineinf[RightCrossFePos-1].RightPos);
		IsCross = 1;
	}
	if (LCount2 > 3 && LeftCrossFePos != 0 && LeftCrossFePos != 58)   //��ಹ��
	{
		IsCross = 1;
		if(down != LeftCrossFePos)
			PatchLineByL(down, lineinf[down].LeftPos, LeftCrossFePos, lineinf[LeftCrossFePos].LeftPos);
	}
	if (RCount2 > 3 && RightCrossFePos != 0 && RightCrossFePos != 58)   //�Ҳಹ��
	{
		IsCross = 1;
		if(down != RightCrossFePos)
			PatchLineByR(down, lineinf[down].RightPos, RightCrossFePos, lineinf[RightCrossFePos].RightPos);
	}
	return 0;
}

int RightLimPos = 0;
int RightLim = 0;
int LeftLimPos = 0;
int LeftLim = 0;
int BabyFind(int effect, uint8(*picture)[160], int CrossDown, int CrossDownPos, int turn)
{
	int i;
	int j;
	int RLIM = 0;
	int LLIM = 0;
	LeftLimPos = 0;
	LeftLim = 0;
	RightLimPos = 0;
	RightLim = 0;
	int BabyDown = 0;
	int BabyDownPos = 0;
	BabyDownPosBigger = 0;
	BabyDownBigger = 0;
	
	if (turn == 1)//��
	{
		for (i = CrossDown; i > up; i--) //LeftLim
		{
			LeftLimPos = CrossDownPos;
			if (picture[i][CrossDownPos] == 0 && picture[i - 1][CrossDownPos] == 255)
			{
				LeftLim = i - 1;
				break;
			}
			if (i == up + 8)
			{
				LeftLim = up + 8;
				break;
			}
		}
		if(LeftLim == effect)
		{
			lineinf[LeftLim].LeftPos = left;
			lineinf[LeftLim].RightPos = right;
		}
		for (i = ReturnBigger(LeftLimPos+1,lineinf[LeftLim].LeftPos+1); i < lineinf[LeftLim].RightPos; i++) //RightLim
		{
			if (picture[LeftLim][i] == 255 && picture[LeftLim][i + 1] == 0)
			{
				RightLimPos = i;
				break;
			}
			if (i == lineinf[LeftLim].RightPos - 1)
			{
				RightLimPos = lineinf[LeftLim].RightPos;
				break;
			}
		}
		for (i = LeftLimPos; i <= ReturnSmaller(RightLimPos, 130); i++)
		{
			for (j = LeftLim; j < 45; j++)
			{
				if (picture[j][i] == 255 && picture[j + 1][i] == 0 && picture[LeftLim][i] == 255)
				{
					BabyDownPos = i;
					BabyDown = j;
					break;
				}
			}
			/********************************************/
			//			if(picture[LeftLim][i] != 255)
			//				LLIM++;
			//			if(LLIM > 2)
			//				break;
			/*******************************************/
			if (BabyDown >= BabyDownBigger)
			{
				BabyDownBigger = BabyDown;
				BabyDownPosBigger = BabyDownPos;
			}
		}
		
	}
	
	if (turn == 2)//��
	{
		for (i = CrossDown; i > up; i--) //LeftLim
		{
			RightLimPos = CrossDownPos;
			if (picture[i][CrossDownPos] == 0 && picture[i - 1][CrossDownPos] == 255)
			{
				RightLim = i - 1;
				break;
			}
			if (i == up + 8)
			{
				RightLim = up + 8;
				break;
			}
		}
		if(RightLim == effect)
		{
			lineinf[RightLim].LeftPos = left;
			lineinf[RightLim].RightPos = right;
		}
		for (i = ReturnSmaller(RightLimPos-1,lineinf[RightLim].RightPos-1); i > lineinf[RightLim].LeftPos; i--) //RightLim
		{
			if (picture[RightLim][i] == 255 && picture[RightLim][i - 1] == 0)
			{
				LeftLimPos = i;
				break;
			}
			if (i == lineinf[RightLim].LeftPos + 1)
			{
				LeftLimPos = lineinf[RightLim].LeftPos;
				break;
			}
		}
		for (i = RightLimPos; i >= ReturnBigger(LeftLimPos,30); i--)
		{
			for (j = RightLim; j < 45; j++)
			{
				if (picture[j][i] == 255 && picture[j + 1][i] == 0 && picture[RightLim][i] == 255)
				{
					BabyDownPos = i;
					BabyDown = j;
					break;
				}
			}
			/*******************************************/
			//			if(picture[RightLim][i] != 255)
			//				RLIM++;
			//			if(RLIM > 2)
			//				break;
			/*******************************************/
			if (BabyDown >= BabyDownBigger)
			{
				BabyDownBigger = BabyDown;
				BabyDownPosBigger = BabyDownPos;
			}
		}
		
		
		
	}
	
	if (BabyDownBigger < 30 && BabyDownBigger != 0 && BabyDownPosBigger != 0)
	{
		if(turn == 1)
		{
			if((BabyDownPosBigger - lineinf[LCrossDown].LeftPos <= 70|| (lineinf[LCrossDown].LeftPos-BabyDownPosBigger < 4 && lineinf[LCrossDown].LeftPos-BabyDownPosBigger > 0)) && BabyDownPosBigger != right) //36
				return 1;
			else
				return 0;
		}
		if(turn == 2)
		{
			if((lineinf[RCrossDown].RightPos- BabyDownPosBigger <= 70 || (BabyDownPosBigger - lineinf[RCrossDown].RightPos < 4 && BabyDownPosBigger - lineinf[RCrossDown].RightPos > 0)) && BabyDownPosBigger != left) //36
				return 1;
			else
				return 0;
		}
	}
	else
		return 0;
	
}


/*========================================================================
*  ��������: ��·           
*  ����˵����
*  ���˵����CircleFind() Ϊ�һ�·�Լ���ػ�·���Եĺ���
CircleHandle() Ϊֱ�Ӳ����ߵĳ����Ҳ���ôϲ��
CircleHandle2() Ϊ���߽����
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
*  ��ע��    ����б�ʣ�����任
========================================================================*/
#define SLOPERIGHTDOWN2   (   0)
#define SLOPERIGHTUP2     (   3.1)
#define SLOPELEFTDOWN2    (  -3.1)
#define SLOPELEFTUP2      (   0)

int CircleLimit = 0;
float CircleD = 0;
int IsBlackCircle = 0;
int StructCircle2 = 0;
float CIRCLEC = 0;
float WHITEC = 0;
int IsBlack2 = 0;
int IsBlack3 = 0;
void CircleFind(int effect, uint8(*picture)[160])
{
	
#define UPDOWN   (4)
#define CIRNOBAR (20)
	int i, j, k;
	
	int MiddleCircle = 0;
	int WhiteCount = 0;
	CIRCLEC = 0;
	WHITEC = 0;
	int Rightright = 0;
	static int CircleCentre;
	int CircleFindFlag = 0;
	dada = 0;
	IsBlackCircle = 0;
	int RightHAHA = 0;
	int LeftHAHA = 159;
	CircleLimit = 0;
	CircleD = 0;
	
	/*
	��ʼ��
	*/
	for (i = down; i >= 0; i--)
	{
		BlackCircleLeft[i] = 0;
		BlackCircleRight[i] = 0;
	}
	
	for(i= down; i > effect+2; i--)
	{
		if(picture[i][(left+right)/2 - 3] == 255 || picture[i][(left+right)/2 + 3] == 255)
			MiddleCircle++;
	}
	
	/*
	����ɫԲȦ������
	*/
	for (i = down; i > effect; i--)
	{
		for (j = lineinf[i].LeftPos + 1; j < lineinf[i].RightPos - 2; j++)
		{
			if (picture[i][j] == 0 && picture[i][j + 1] == 255 && picture[i][j + 2] == 255)
			{
				BlackCircleLeft[i] = j + 1;
				for (k = lineinf[i].RightPos - 2; k > BlackCircleLeft[i]; k--)
				{
					if (picture[i][k] == 255 && picture[i][k + 1] == 0 && picture[i][k + 2] == 0)
					{
						BlackCircleRight[i] = k;
						if ((BlackCircleRight[i] - BlackCircleLeft[i] > 5)) 
							IsBlackCircle++;
						break;
					}
				}
				break;
				
			}
		}
	}
	
	/*
	ȷ����ɫԲ�ε����Ԫ��
	*/
	if (IsBlackCircle > 4)
	{
		for (i = down; i > effect; i--)
		{
			if (CircleDown == 0 && (BlackCircleRight[i] != 0 || BlackCircleLeft[i] != 0)) //ȷ����ɫԲ�ε��¶���
			{
				CircleDown = i;
				CircleFindFlag = 1;
			}
			if (BlackCircleRight[i] != 0)                                                  //ȷ����ɫԲ�ε��Ҷ���
			{
				if (BlackCircleRight[i] > RightHAHA)
				{
					RightHAHA = BlackCircleRight[i];
					CircleRight = i;
				}
			}
			if (BlackCircleLeft[i] != 0)                                                  //ȷ����ɫԲ�ε��󶥵�
			{
				if (BlackCircleLeft[i] < LeftHAHA)
				{
					LeftHAHA = BlackCircleLeft[i];
					CircleLeft = i;
				}
				
			}
			if ((BlackCircleRight[i] != 0 || BlackCircleLeft[i] != 0) && CircleDown != 0 && CircleFindFlag == 1)  //ȷ����ɫԲ�����϶���
				CircleUp = i;
			
			if(BlackCircleRight[i] == 0 && BlackCircleLeft[i] == 0 && CircleFindFlag == 1)
				CircleFindFlag = 0;
			
		}
	}
	
	CircleLimit = (BlackCircleRight[CircleDown] + BlackCircleLeft[CircleDown]) / 2;
	
	for(j = CircleDown; j >= CircleUp; j--)
	{
		for(i = BlackCircleLeft[j] + 1; i <  BlackCircleRight[j]; i++)
		{
			
                    if(picture[j][i] == 0)
                    {
				WhiteCount++;
                                break;
                    }
		}
	}
	
	for(i = CircleDown; i >= CircleUp; i--)
	{
		CIRCLEC += BlackCircleRight[i] - BlackCircleLeft[i];
		WHITEC += (lineinf[i].RightPos - lineinf[i].LeftPos) - (BlackCircleRight[i] - BlackCircleLeft[i]);
	}
	CrossLineNormal2(effect, picture);
	
	int CircleFlagR = 0;
	int CircleFlagL = 0;
	if(CircleDown <= RCrossUp && RCrossUp > (effect + 8) && (WHITEC/CIRCLEC) > 0.6)
		CircleFlagR = 1;
	if(CircleDown <= LCrossUp && LCrossUp > (effect + 8) && (WHITEC/CIRCLEC) > 0.6)
		CircleFlagL = 1;    
	
	IsBlack2 = 0;
	if(CircleDown - CircleUp > 15)
	{
		if(picture[(int)((CircleDown+CircleUp)/2+2)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 ||
		   picture[(int)((CircleDown+CircleUp)/2+3)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
			   picture[(int)((CircleDown+CircleUp)/2+4)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
				   picture[(int)((CircleDown+CircleUp)/2+5)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255)
			IsBlack2 = 1;
	}
	else if(CircleDown - CircleUp > 10)
	{
		if(picture[(int)((CircleDown+CircleUp)/2+1) ][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
		   picture[(int)((CircleDown+CircleUp)/2+2)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
			   picture[(int)((CircleDown+CircleUp)/2+3)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255)
			IsBlack2 = 1;
	}
	else if(CircleDown - CircleUp > 7)
	{
		if(picture[(int)((CircleDown+CircleUp)/2)   ][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
		   picture[(int)((CircleDown+CircleUp)/2+1)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
			   picture[(int)((CircleDown+CircleUp)/2+2)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255)
			IsBlack2 = 1;
	}
	else if(CircleDown - CircleUp > 4)
	{
		if(picture[(int)((CircleDown+CircleUp)/2)   ][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
		   picture[(int)((CircleDown+CircleUp)/2-1)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
			   picture[(int)((CircleDown+CircleUp)/2+1)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255)
			IsBlack2 = 1;
	}
	
	IsBlack3 = 0;        
	if(((fabs(sqrt((60-LCrossDown)*(60-LCrossDown)+lineinf[LCrossDown].LeftPos*lineinf[LCrossDown].LeftPos) - 
			 sqrt((60-RCrossDown)*(60-RCrossDown)+(lineinf[RCrossDown].RightPos-160)*(lineinf[RCrossDown].RightPos-160))) > 33)
	   && LCrossDown > 25 && RCrossDown > 25 )|| fabs(LCrossDown - RCrossDown) > 18)
		IsBlack3 = 1;
	
	int IsWhite = 0;
	for(i = down; i > 35; i--)
	{
		if(lineinf[i].LeftPos != left || lineinf[i].RightPos != right)
		{
			IsWhite = 1;
			break;
		}
	}
	
	if (IsBlack2 == 1 && IsBlack3 == 0 && CircleDown - CircleUp >= UPDOWN && CircleDown >= 6 && WhiteCount < 3 && CIRCLEC != 0 && CircleFlagL == 0 && CircleFlagR == 0) 
	{
		dada = 1;
		if(CircleUp != 0 && (BlackCircleRight[CircleRight] > 80) && (BlackCircleLeft[CircleLeft] < 80) && ((IsWhite == 0) ||(LCrossDown != 0 && RCrossDown != 0) || (LCrossDown == 0 && RCrossDown > 37) ||(RCrossDown == 0 && LCrossDown > 37))
		   && MiddleCircle > 1  && ((float)(WHITEC/CIRCLEC) < 2 || StructCircle == 1) )
		{
			IsCircle = 1;
			StructCircle = 1;
			
		}
	}
}

void CircleFindByFront(int effect, uint8(*picture)[160],int Dir,int IsGet)
{
#define UPDOWN   (4)
#define CIRNOBAR (20)
	int i, j, k;
	
	int MiddleCircle = 0;
	int WhiteCount = 0;
	CIRCLEC = 0;
	WHITEC = 0;
	int Rightright = 0;
	static int CircleCentre;
	int CircleFindFlag = 0;
	dada = 0;
	IsBlackCircle = 0;
	int RightHAHA = 0;
	int LeftHAHA = 159;
	CircleLimit = 0;
	CircleD = 0;
	
	/*
	��ʼ��
	*/
	for (i = down; i >= 0; i--)
	{
		BlackCircleLeft[i] = 0;
		BlackCircleRight[i] = 0;
	}
	
	for(i= down; i > effect+2; i--)
	{
		if(picture[i][(left+right)/2] == 255)
			MiddleCircle++;
	}
	
	/*
	����ɫԲȦ������
	*/
	for (i = down; i > effect; i--)
	{
		for (j = lineinf[i].LeftPos + 1; j < lineinf[i].RightPos - 2; j++)
		{
			if (picture[i][j] == 0 && picture[i][j + 1] == 255 && picture[i][j + 2] == 255)
			{
				BlackCircleLeft[i] = j + 1;
				for (k = lineinf[i].RightPos - 2; k > BlackCircleLeft[i]; k--)
				{
					if (picture[i][k] == 255 && picture[i][k + 1] == 0 && picture[i][k + 2] == 0)
					{
						BlackCircleRight[i] = k;
						if ((BlackCircleRight[i] - BlackCircleLeft[i] > 5)) //&& fabs((BlackCircleRight[i] + BlackCircleLeft[i]) / 2 - (left + right) / 2) < CIRNOBAR) // ȷ���ǻ�·�������ϰ�
							IsBlackCircle++;
						break;
					}
				}
				break;
				
			}
		}
	}
	
	/*
	ȷ����ɫԲ�ε����Ԫ��
	*/
	if (IsBlackCircle > 4)
	{
		for (i = down; i > effect; i--)
		{
			if (CircleDown == 0 && (BlackCircleRight[i] != 0 || BlackCircleLeft[i] != 0)) //ȷ����ɫԲ�ε��¶���
			{
				CircleDown = i;
				CircleFindFlag = 1;
			}
			if (BlackCircleRight[i] != 0)                                                  //ȷ����ɫԲ�ε��Ҷ���
			{
				if (BlackCircleRight[i] > RightHAHA)
				{
					RightHAHA = BlackCircleRight[i];
					CircleRight = i;
				}
			}
			if (BlackCircleLeft[i] != 0)                                                  //ȷ����ɫԲ�ε��󶥵�
			{
				if (BlackCircleLeft[i] < LeftHAHA)
				{
					LeftHAHA = BlackCircleLeft[i];
					CircleLeft = i;
				}
				
			}
			if ((BlackCircleRight[i] != 0 || BlackCircleLeft[i] != 0) && CircleDown != 0 && CircleFindFlag == 1)  //ȷ����ɫԲ�����϶���
				CircleUp = i;
			
			if(BlackCircleRight[i] == 0 && BlackCircleLeft[i] == 0 && CircleFindFlag == 1)
				CircleFindFlag = 0;
			
		}
	}
	
	CircleLimit = (BlackCircleRight[CircleDown] + BlackCircleLeft[CircleDown]) / 2;
	
	for(j = CircleDown; j >= CircleUp; j--)
	{
		for(i = BlackCircleLeft[j] + 1; i <  BlackCircleRight[j]; i++)
		{
			
			if(picture[j][i] == 0)
				WhiteCount++;
		}
	}
	
	for(i = CircleDown; i >= CircleUp; i--)
	{
		CIRCLEC += BlackCircleRight[i] - BlackCircleLeft[i];
		WHITEC += (lineinf[i].RightPos - lineinf[i].LeftPos) - (BlackCircleRight[i] - BlackCircleLeft[i]);
	}
	CrossLineNormal2(effect, picture);
	
	int CircleFlagR = 0;
	int CircleFlagL = 0;
	if(CircleDown <= RCrossUp && RCrossUp > effect + 8 && (WHITEC/CIRCLEC) > 0.6)
		CircleFlagR = 1;
	if(CircleDown <= LCrossUp && LCrossUp > effect + 8 && (WHITEC/CIRCLEC) > 0.6)
		CircleFlagL = 1;
	//      
	
	IsBlack2 = 0;
	if(CircleDown - CircleUp > 15)
	{
		if(picture[(int)((CircleDown+CircleUp)/2+4)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
		   picture[(int)((CircleDown+CircleUp)/2+5)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
			   picture[(int)((CircleDown+CircleUp)/2+6)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255)
			IsBlack2 = 1;
	}
	else if(CircleDown - CircleUp > 10)
	{
		if(picture[(int)((CircleDown+CircleUp)/2+2)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
		   picture[(int)((CircleDown+CircleUp)/2+3)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
			   picture[(int)((CircleDown+CircleUp)/2+4)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255)
			IsBlack2 = 1;
	}
	else if(CircleDown - CircleUp > 7)
	{
		if(picture[(int)((CircleDown+CircleUp)/2)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
		   picture[(int)((CircleDown+CircleUp)/2+1)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
			   picture[(int)((CircleDown+CircleUp)/2+2)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255)
			IsBlack2 = 1;
	}
	else if(CircleDown - CircleUp > 4)
	{
		if(picture[(int)((CircleDown+CircleUp)/2)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
		   picture[(int)((CircleDown+CircleUp)/2-1)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255 || 
			   picture[(int)((CircleDown+CircleUp)/2+1)][(lineinf[down].LeftPos + lineinf[down].RightPos ) / 2] == 255)
			IsBlack2 = 1;
	}
	
	
	IsBlack3 = 0;        
	if((fabs(sqrt((60-LCrossDown)*(60-LCrossDown)+lineinf[LCrossDown].LeftPos*lineinf[LCrossDown].LeftPos) - 
			 sqrt((60-RCrossDown)*(60-RCrossDown)+(lineinf[RCrossDown].RightPos-160)*(lineinf[RCrossDown].RightPos-160))) > 32)
	   && LCrossDown != 0 && RCrossDown != 0)
		IsBlack3 = 1;
	
	int IsWhite = 0;
	for(i = down; i > 35; i--)
	{
		if(lineinf[i].LeftPos != left || lineinf[i].RightPos != right)
		{
			IsWhite = 1;
			break;
		}
	}
	
	if (IsBlack2 == 1 && IsBlack3 == 0 && CircleDown - CircleUp >= UPDOWN && CircleDown >= 7 && WhiteCount < 30 && CIRCLEC != 0 && CircleFlagL == 0 && CircleFlagR == 0) 
	{
		dada = 1;
		if(CircleUp != 0 && (BlackCircleRight[CircleRight] > 80) && (BlackCircleLeft[CircleLeft] < 80) &&  ((IsWhite == 0) ||(LCrossDown != 0 && RCrossDown != 0) || (LCrossDown == 0 && RCrossDown > 37) ||(RCrossDown == 0 && LCrossDown > 37))
		   && MiddleCircle > 1  && ((float)(WHITEC/CIRCLEC) < 1.6 || StructCircle == 1) )
		{
			IsCircle = 1;
			StructCircle = 1;
			
		}
	}
	
	if(IsCircle == 1)
	{
		switch(Dir)
		{
		case 1:
			CircleDown = CircleLeft;
			break;
		case 2:
			CircleDown = CircleRight;
			break;
		default:
			break;
			
			
		}
	}
}


int IsJump = 0;
int IsCurve = 0;
int IsCurve2 = 0;
int CircleDealS = 0;

int CircleStopF = 0;
int CircleStatus = 0;
int StructCircleDeal(int effect, uint8(*picture)[160])
{
	if(StructCircle == 0)
		CircleStopF = 0;
	
	int tend = RoadDir;
	if(StructCircle == 1 && HalfFlag == 0)
	{
		switch(tend)
		{
		case 1:
			if(RightCrossFePos > 40 && RightCrossFePos < 58 && CircleDealS == 0)
			{
				HalfFlag = 1;
				CircleDealS = RightCrossFePos;
			}
			break;
		case 2:
			if(LeftCrossFePos > 40 && LeftCrossFePos < 58 && CircleDealS == 0)
			{
				HalfFlag = 1;
				CircleDealS = LeftCrossFePos;
			}
			break;
		default:
			break;
		}
	}
	
	if(HalfFlag == 1 && CircleStopF == 0)
	{
		switch(tend)
		{
		case 1:
			if( (RCrossDown > 15 && RCrossUp > (effect + 8)) || (RCrossUp == (effect + 8) && LCrossDown > 10 && LCrossDown < 55) )
			{
				CircleStop2 = 1;
				CircleStopF = 1;
			}
			break;
		case 2:
			if( (LCrossDown > 15 && LCrossUp > (effect + 8)) || (LCrossUp == (effect + 8) && RCrossDown > 10 && RCrossDown < 55) )
			{
				CircleStop2 = 1;
				CircleStopF = 1;
			}
			break;
		default:
			break;
		}
	}
	if(HalfFlag == 1 && CircleStop2 == 2)
	{
		switch(tend)
		{
		case 1:
			if((CircleDealS > RCrossUp && RCrossUp > (effect + 8))||(CircleDealS > RCrossUp && RCrossUp == (effect + 8) && LCrossDown > 18 && LCrossDown < 45))
			{ 
				if(LCrossDown > 13 && LCrossDown < 55 && lineinf[LCrossDown].LeftPos > (lineinf[down].LeftPos + 3))
					CircleStatus = 1;
			}
			break;
		case 2:
			if((CircleDealS > LCrossUp && LCrossUp > (effect + 8))||(CircleDealS > LCrossUp && LCrossUp == (effect + 8) && RCrossDown > 18 && RCrossDown < 45))
			{ 
				if(RCrossDown > 13 && RCrossDown < 55 && lineinf[RCrossDown].RightPos < (lineinf[down].RightPos - 3)) 
					CircleStatus = 1;
			}
			break;
		default:
			break;
			
		}
	}
	if (CircleStatus == 40 )
	{
		CircleStatus = 0;
		CircleDealS = 0;
		StructCircle = 0;
		HalfFlag = 0;
	}
	
}


int CircleStart = 0;
int StructCircleDealStart(int effect, uint8(*picture)[160])
{
	
	int tend = RoadDir;
	if(StructCircle == 1 && HalfFlag == 0)
	{
		switch(tend)
		{
		case 1:
			if(RightCrossFePos > 40 && RightCrossFePos < 58 && CircleDealS == 0)
			{
				HalfFlag = 1;
				CircleDealS = RightCrossFePos;
			}
			break;
		case 2:
			if(LeftCrossFePos > 40 && LeftCrossFePos < 58 && CircleDealS == 0)
			{
				HalfFlag = 1;
				CircleDealS = LeftCrossFePos;
			}
			break;
		default:
			break;
		}
	}
	
	if(HalfFlag == 1)
	{
		switch(tend)
		{
		case 1:
			if((CircleDealS > RCrossUp && RCrossUp > (effect + 8))||(CircleDealS > RCrossUp && RCrossUp == (effect + 8) && LCrossDown > 18 && LCrossDown < 45))
			{ 
				if(LCrossDown > 13 && LCrossDown < 55 && lineinf[LCrossDown].LeftPos > (lineinf[down].LeftPos + 3))
					CircleStatus = 1;
			}
			break;
		case 2:
			if((CircleDealS > LCrossUp && LCrossUp > (effect + 8))||(CircleDealS > LCrossUp && LCrossUp == (effect + 8) && RCrossDown > 18 && RCrossDown < 45))
			{ 
				if(RCrossDown > 13 && RCrossDown < 55 && lineinf[RCrossDown].RightPos < (lineinf[down].RightPos - 3)) 
					CircleStatus = 1;
				
			}
			
			break;
		default:
			break;
			
		}
	}
	
	if(CircleStatus == 60)
	{
		CircleStatus = 0;
		CircleStart = 1;
		CircleDealS = 0;
		StructCircle = 0;
		HalfFlag = 0;
	}
	
}

int LeftDown = 0;
int LeftDownPos = 0;
int RightDown = 0;
int RightDownPos = 0;
int LeftUp = 0;
int LeftUpPos = 0;
int RightUp = 0;
int RightUpPos = 0;

void CircleHandle2(int effect, uint8(*picture)[160],int dir)
{
#define RightCircle  (1)
#define LeftCircle   (2)
#define CircleLimit  (10)
	
	int i;
	
	int CircleDownPos = 0;
	int CircleRightPos = 0;
	int CircleLeftPos = 0;
	int CircleUpPos = 0;
	
	int LEFT[60] = { left };   //���
	int RIGHT[60] = { right };//�ұ�
	int LeftCircleFePos = 0;
	int RightCircleFePos = 0;
	
	LeftDown = 0;
	LeftDownPos = 0;
	RightDown = 0;
	RightDownPos = 0;
	LeftUp = 0;
	LeftUpPos = 0;
	RightUp = 0;
	RightUpPos = 0;
	
	for (i = down; i > up; i--)
	{
		LEFT[i] = left;
		RIGHT[i] = right;
		
	}
	for (i = down; i > effect; i--)
	{
		LEFT[i] = lineinf[i].LeftPos;
		RIGHT[i] = lineinf[i].RightPos;
		
	}
	
	switch (dir)
	{
		
		
	case 1:
		/*
		������
		*/
		CircleLeftPos = BlackCircleLeft[CircleLeft];
		CircleUpPos = (BlackCircleLeft[CircleUp] + BlackCircleRight[CircleUp]) / 2;
		/********************                 �Ҳ�         *******************/
		for (i = down; i > CircleLimit; i--)      //������½ǵ�ֵ
		{
			if (((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0) && ((lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0))
			{
				RightDown = i;
				RightDownPos = lineinf[RightDown].RightPos;
				break;
			}
			if (i == CircleLimit + 1)
			{
				RightDown = 59;
				RightDownPos = right;
				break;
			}
			
		}
		/****************************************/
		
		/*����*/
		if(RightDown > CircleDown-1)
			PatchLineByR(RightDown, RightDownPos, CircleDown-1, BlackCircleLeft[CircleDown-1]);
		else if(CircleDown != 0 && (CircleDown-1) != down)
			PatchLineByR(down, right, CircleDown-1, BlackCircleLeft[CircleDown-1]);
		
		
		for (i = CircleDown-1; i >= CircleUp; i--)
			lineinf[i].RightPos = BlackCircleLeft[i];
		
		break;
		
		
	case 2:
		/*
		�Ҳ����
		*/
		CircleRightPos = BlackCircleRight[CircleRight];
		CircleUpPos = (BlackCircleLeft[CircleUp] + BlackCircleRight[CircleUp]) / 2;
		/********************                 ���         *******************/
		for (i = down; i > CircleLimit; i--)      //������½ǵ�ֵ
		{
			if (((lineinf[i].LeftPos - lineinf[i - 1].LeftPos) <= 0) && ((lineinf[i - 1].LeftPos - lineinf[i - 2].LeftPos) > 0))
			{
				LeftDown = i;
				LeftDownPos = lineinf[LeftDown].LeftPos;
				break;
			}
			if (i == CircleLimit + 1)
			{
				LeftDown = 59;
				LeftDownPos = left;
				break;
			}
		}
		/****************************************/
		
		
		/*����*/
		if(LeftDown > CircleDown-1)
			PatchLineByL(LeftDown, LeftDownPos, CircleDown-1, BlackCircleRight[CircleDown-1]);
		else if( CircleDown != 0 && (CircleDown-1) != down)
			PatchLineByL(down, left, CircleDown, BlackCircleRight[CircleDown-1]);
		
		for (i = CircleDown-1; i >= CircleUp; i--)
			lineinf[i].LeftPos = BlackCircleRight[i];
		
		break;
	default:
		break;
		
		
	}
}



/*========================================================================
*  ��������: ZebarLine           
*  ����˵����������
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
========================================================================*/
int ZECOUNT = 0;

int ZebarLine(int effect, uint8(*picture)[160])
{
	int i;
	int j;
	int ZebarCount = 0;
	ZECOUNT = 0;
	
	for (i = down; i > effect + 10; i--)
	{
		if (lineinf[i].LeftPos == right || lineinf[i].RightPos == left || lineinf[i].LeftPos >= lineinf[i].RightPos 
			|| lineinf[i].LeftPos == left || lineinf[i].RightPos == right){}
		else {
			for (j = lineinf[i].LeftPos + 1; j < lineinf[i].RightPos - 1; j++)
			{
				if (picture[i][j] != picture[i][j + 1])
					ZebarCount++;
			}
			if (ZECOUNT < ZebarCount / 2)
			{
				ZECOUNT = ZebarCount / 2;
				ZebarCount = 0;
			}
			else
			{
				ZECOUNT = ZECOUNT;
				ZebarCount = 0;
			}
		}
	}
	
	if (ZECOUNT > 3)
		return 1;
	
	return 0;
	
}

int ZebarLineDistance(int effect, uint8(*picture)[160])
{
	if(IsZebarLine == 0)
		return 0;
	
	int distance = 0;
	int ZebarLine[60] = { 0 };
	
	for(int i = down; i > effect + 3; i--)
		ZebarLine[i] = CountLine2(i, picture);
	
	for(int i = down; i > effect + 5; i--)
	{
		if(ZebarLine[i] > 5 && ZebarLine[i-1] > 5)
		{
			distance = down - i;
			break;
		}
	}
	
	if(distance != 0)
		return distance;
	else 
		return 0;
	
}

/*========================================================================
*  ��������: ObstacleFind           
*  ����˵�����ϰ�����
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
========================================================================*/
int IsObstacleLeft = 0;
int IsObstacleRight = 0;
int NotObstacle = 0;
int FirstOb = 0;
int OBBlack = 0;
int ObstacleFind(int effect, uint8(*picture)[160])
{
	int i = 0;
	int j = 0;
	int k = 0;
	
	int wrong = 0;
	FirstOb = 0;
	OBBlack = 0;
	int RIGHTOBS = 0;
	int LEFTOBS = 0;
	int ObstacleLeft[60] = {0};
	int ObstacleRight[60] = {0};
	IsObstacleLeft = 0;
	IsObstacleRight = 0;
	NotObstacle = 0;
	int LeftOb = 0;
	int RightOb = 0;
	
	if(effect > 7)
		return 0;
	
	for(i = down; i > effect; i--)
	{
		if(lineinf[i].LeftPos != left)
		{
			LeftOb = i;
			break;
		}
		
	}
	for(i = down; i > effect; i--)
	{
		if(lineinf[i].RightPos != right)
		{
			RightOb = i;
			break;
		}
		
	}
	for(i = LeftOb; i > effect + 3; i--)
	{
		if(lineinf[i].LeftPos == left)
		{
			NotObstacle = 1;
			break;
		}
	}
	for(i = RightOb; i > effect + 3; i--)
	{
		if(lineinf[i].RightPos == right)
		{
			NotObstacle = 1;
			break;
		}
	}
	
	for(i = down; i > effect + 5; i--)
	{
		if(lineinf[i].LeftPos - lineinf[i-1].LeftPos > 20)
		{
			wrong = 1;
			break;
		}
		if(lineinf[i-1].RightPos - lineinf[i].RightPos > 20)
		{
			wrong = 1;
			break;
		}
	}
	
	
	for (i = down; i > effect + 2; i--)
	{
		for (j = lineinf[i].LeftPos + 1; j < lineinf[i].RightPos - 2; j++)
		{
			if (picture[i][j] == 0 && picture[i][j + 1] == 255 && picture[i][j + 2] == 255)
			{
				ObstacleLeft[i] = j + 1 - lineinf[i].LeftPos;
				LEFTOBS = j;
				for (k = lineinf[i].RightPos - 1; k > lineinf[i].LeftPos + ObstacleLeft[i]; k--)
				{
					if (picture[i][k] == 255 && picture[i][k + 1] == 0 && picture[i][k + 2] == 0)
					{
						ObstacleRight[i] = lineinf[i].RightPos - k;
						RIGHTOBS = k;
						if(ObstacleLeft[i] != 0)
						{
							if ((float)(ObstacleRight[i]/ObstacleLeft[i]) > 3 && ObstacleLeft[i] < 13 && ((ObstacleRight[i] + ObstacleLeft[i]) > (RIGHTOBS - LEFTOBS)))
							{
								IsObstacleLeft++;
								if(picture[i][80] == 255)
									OBBlack++;
								if(picture[i][80] == 0 && FirstOb == 0)
									FirstOb = i;
							}
							if ((float)(ObstacleRight[i]/ObstacleLeft[i]) < 0.3333333 && ObstacleRight[i] < 13 && ((ObstacleRight[i] + ObstacleLeft[i]) > (RIGHTOBS - LEFTOBS)))
							{	
								IsObstacleRight++; 
								if(picture[i][80] == 255)
									OBBlack++;
								if(picture[i][80] == 0 && FirstOb == 0)
									FirstOb = i;
							}
						}
						break;
					}
				}
				break;
				
			}
		}
	}
	
	if(IsObstacleLeft > 2 && NotObstacle == 0  && FirstOb > 5 && wrong == 0) // OBBlack
		return 2;
	if(IsObstacleRight > 2 && NotObstacle == 0 && FirstOb > 5 && wrong == 0)
		return 1;
	
	return 0;
}
int IsMiddleJump(int effect, uint8(*picture)[160])
{
	int Is = 0;
	for(int i = down; i > effect + 20; i--)
	{
		if(fabs(lineinf[i].LeftPos - lineinf[i - 5].LeftPos) > 15 || fabs(lineinf[i].RightPos- lineinf[i - 5].RightPos) > 15)
		{
			Is = 1;
			break;
		}
	}
	if(Is)
		return 1;
	else
		return 0;
}
/*========================================================================
*  ��������: ExceedOnStraightLineFind           
*  ����˵����ֱ����������
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
========================================================================*/
int StructExceed = 0;
int Exceed = 0;      //�������ߵ�
int ExceedPos = 0;   //��������ֵ
int ExceedOn = 0;
int ExceedOnPos = 0;
int CountNum[60] = { 0 };

int ExceedOnStraightLineFind(int effect, uint8(*picture)[160])
{
#define EXCEEDLIMIT   (15)
	
	int i;
	
	Exceed = 0;      //�������ߵ�
	ExceedPos = 0;   //��������ֵ
	
	int RCount2 = 0;
	int LCount2 = 0;
	
	IsExceed = 0;    //1Ϊ��2Ϊ�ң�0û��
	
	for (i = down; i > effect + 5; i--)
	{
		if (lineinf[i - 2].LeftPos - lineinf[i].LeftPos > EXCEEDLIMIT)   // Ѱ���Ƿ��������ڸ����
		{
			Exceed = i - 2;
			ExceedPos = lineinf[i - 2].LeftPos;
			if(Exceed > 15)
			{
				IsExceed = 1;
				StructExceed = 1;
				break;
			}
		}
		if (lineinf[i].RightPos - lineinf[i - 2].RightPos > EXCEEDLIMIT) // Ѱ���Ƿ����Ҳ���ڸ����
		{
			Exceed = i - 2;
			ExceedPos = lineinf[i - 2].RightPos;
			if(Exceed > 15)
			{
				IsExceed = 2;
				StructExceed = 1;
				break;
			}
		}
	}
	
	
	if (IsExceed == 1)  // ��ಹ��
		return 1;
	if (IsExceed == 2 )  // �Ҳಹ��
		return 2;
	
	if (IsExceed == 0)
		return 0;
	
	return 0;
	
}

int ExceedOnStraightLineDo(int WHICH, int effect, uint8(*picture)[160])
{
	int i = 0;
	
	if (IsExceed == 1)  // ��ಹ��
	{
		if(down != Exceed-1)
			PatchLineByL(down, lineinf[down].LeftPos, Exceed-1, lineinf[Exceed-1].RightPos - StraightLine[59-Exceed+1] * 2 );
		for(i = Exceed - 1; i > effect; i--)
			lineinf[i].LeftPos = lineinf[i].RightPos - StraightLine[59-i] * 2; 
	}
	if (IsExceed == 2)  // �Ҳಹ��
	{
		if(down != Exceed-1)
			PatchLineByR(down, lineinf[down].RightPos, Exceed-1, lineinf[Exceed-1].LeftPos + StraightLine[59-Exceed+1] * 2);
		for(i = Exceed - 1; i > effect; i--)
			lineinf[i].RightPos = lineinf[i].LeftPos + StraightLine[59-i] * 2; 
	}
	if (IsExceed == 1 || IsExceed == 2)
		return 1;
	else
		return 0;
}

int CancelStructExceed(int effect, uint8(*picture)[160], int Dir)
{
	int Flag = 0;
	switch(Dir)
	{
	case 1:
		for(int i = effect + 15; i < down; i++)
		{
			if(lineinf[i].LeftPos > lineinf[i-1].LeftPos && lineinf[i-1].LeftPos > lineinf[i-2].LeftPos
			   && lineinf[i-2].LeftPos > lineinf[i-3].LeftPos)
			{
				Flag = i;
				break;
			}
		}
		break;
		
	case 2:
		for(int i = effect + 15; i < down; i++)
		{
			if(lineinf[i].RightPos < lineinf[i-1].RightPos && lineinf[i-1].RightPos < lineinf[i-2].RightPos
			   && lineinf[i-2].RightPos > lineinf[i-3].RightPos )
			{
				Flag = i;
				break;
			}
		}
		break;
		
	default:
		break;
	}
	
	if(StructExceed == 1 && IsExceed == 0 && Flag > 43)
	{
		Flag = 0;
		StructExceed = 0;
		
		return 1;
	}
	else
		return 0;
}

int CancelDistance ;
int CancelStructExceed2(int effect, uint8(*picture)[160])
{
	CancelDistance = 0;
	int CancelExceedNum[60] = {0};
	
	int i = 0;
	for(i = effect + 3; i < down; i++)
		CancelExceedNum[i] = CountLine( i, picture);
	
	for(i = effect + 13; i < down - 1 ;i++)
	{
		if((CancelExceedNum[i-4] - CancelExceedNum[i]) > 4 && (CancelExceedNum[i-4] - CancelExceedNum[i+1]) > 4 && (CancelExceedNum[i-4] - CancelExceedNum[i+2]) > 4)
		{
			CancelDistance = i;
			break;
		}
	}
	
	if(StructExceed == 1 && IsExceed == 0 && CancelDistance > 45)
	{
		CancelDistance = 0;
		StructExceed = 0;
		
		return 1;
	}
	else
		return 0;
}


int DistanceDig(int effect, uint8(*picture)[160])
{
	int count[60] = {0};
	int distance = 59;
	int i = 0;
	
	for(i = down; i > effect; i--)
		count[i] = CountLine(i , picture);
	
	for(i = down; i > effect + 7; i--)
	{
		if((count[i] - count[i-2]) > 15 && (count[i] - count[i-3]) > 15)
		{
			distance = i - 2;
			break;
		}
	}
	
	if(distance != 59)
		return distance;
	else
		return 59;
}

int StraightLineFind(int effect, uint8(*picture)[160])
{
#define STRAIGHTTEMP   32
	int i = 0;
	
	int LStraight = 0;
	int RStraight = 0;
	int StraightZero = 0;
	
	int temp = lineinf[1].RightPos - lineinf[1].LeftPos;
	int temp2 = lineinf[2].RightPos - lineinf[2].LeftPos;
	
	for(i = down; i > 1; i--)
	{
		if(lineinf[i].LeftPos == left || lineinf[i].RightPos == right)
		{
			StraightZero = 1;
			break;
		}
	}
	for(i = down; i > 1; i--)
	{
		if(lineinf[i].LeftPos <= lineinf[i-1].LeftPos && lineinf[i-1].LeftPos - lineinf[i].LeftPos < 4)
			LStraight++;
		if(lineinf[i].RightPos >= lineinf[i-1].RightPos && lineinf[i].RightPos - lineinf[i-1].RightPos < 4)
			RStraight++;
	}
	
	if(LStraight > (down - 2) && RStraight > (down - 2) && effect < 5 
	   && IsCross == 0 && StraightZero == 0 && IsCross == 0 && temp < STRAIGHTTEMP && temp2 < STRAIGHTTEMP)
		return 1;
	else 
		return 0;
	
}

int StraightLineFind2(int effect, uint8(*picture)[160])
{
	int i = 0;
	
	int LStraight = 0;
	int RStraight = 0;
	int StraightZero = 0;
	
	for(i = down; i > 1; i--)
	{
		if(lineinf[i].LeftPos == left || lineinf[i].RightPos == right)
		{
			StraightZero = 1;
			break;
		}
	}
	for(i = down; i > 1; i--)
	{
		if(lineinf[i].LeftPos <= lineinf[i-1].LeftPos && lineinf[i-1].LeftPos - lineinf[i].LeftPos < 10)
			LStraight++;
		if(lineinf[i].RightPos >= lineinf[i-1].RightPos && lineinf[i].RightPos - lineinf[i-1].RightPos < 10)
			RStraight++;
	}
	
	if(LStraight > (down - 2) && RStraight > (down - 2) && effect < 5 
	   && IsCross == 0 && StraightZero == 0)
		return 1;
	else 
		return 0;
	
}

int StraightLineFind3(int effect, uint8(*picture)[160])
{
	int i = 0;
	
	int LStraight = 0;
	int RStraight = 0;
	int StraightZero = 0;
	
	for(i = down; i > 5; i--)
	{
		if(lineinf[i].LeftPos == left || lineinf[i].RightPos == right)
		{
			StraightZero = 1;
			break;
		}
	}
	for(i = down; i > 5; i--)
	{
		if(lineinf[i].LeftPos <= lineinf[i-1].LeftPos && lineinf[i-1].LeftPos - lineinf[i].LeftPos < 10)
			LStraight++;
		if(lineinf[i].RightPos >= lineinf[i-1].RightPos && lineinf[i].RightPos - lineinf[i-1].RightPos < 10)
			RStraight++;
	}
	
	if(LStraight > (down - 6) && RStraight > (down - 6) && effect < 8 && StraightZero == 0)
		return 1;
	else 
		return 0;
	
}
int LStraight4 = 0;
int RStraight4 = 0;
int StraightZero = 0;
int StraightLineFind4(int effect, uint8(*picture)[160])
{
	int i = 0;
	LStraight4 = 0;
	RStraight4  = 0;
	StraightZero = 0;
	
	for(i = down - 5; i > 12; i--)
	{
		if(lineinf[i].LeftPos == left || lineinf[i].RightPos == right)
		{
			StraightZero = 1;
			break;
		}
	}
	for(i = down; i > 12; i--)
	{
		if(lineinf[i].LeftPos <= (lineinf[i-1].LeftPos + 2) && lineinf[i-1].LeftPos - lineinf[i].LeftPos < 10)
			LStraight4++;
		if(lineinf[i].RightPos >= (lineinf[i-1].RightPos - 2) && lineinf[i].RightPos - lineinf[i-1].RightPos < 10)
			RStraight4++;
	}
	
	if(LStraight4 > (down - 13) && RStraight4 > (down - 13) && effect < 15 && StraightZero == 0)
		return 1;
	else 
		return 0;
	
}


int StraightLinePatch(int effect, uint8(*picture)[160], int Dir)
{
	int i = 0;
	
	switch(Dir)
	{
	case 1:
		for(i = down; i > effect; i--)
			lineinf[i].MiddlePos = lineinf[i].LeftPos + StraightLine[down-i];
		break;
	case 2:
		for(i = down; i > effect; i--)
			lineinf[i].MiddlePos = lineinf[i].RightPos - StraightLine[down-i];
		break;
	default:
		break;
	}
	
	return 0;
}

int StraightLinePatch2(int effect, uint8(*picture)[160], int Dir)
{
	int i = 0;
	
	switch(Dir)
	{
	case 1:
		for(i = down; i > effect; i--)
			lineinf[i].MiddlePos = lineinf[i].LeftPos + StraightLine2[down-i] - 3;
		break;
	case 2:
		for(i = down; i > effect; i--)
			lineinf[i].MiddlePos = lineinf[i].RightPos - StraightLine2[down-i] + 3;
		break;
	default:
		break;
	}
	
	return 0;
}

/*========================================================================
*  ��������: ramp          
*  ����˵�����µ�
*  ����ʱ�䣺2017
*  �޸�ʱ�䣺2017
*  ����˵����
========================================================================*/
int IsRamp = 0;
int StructRamp = 0;
int RampFind(int effect, uint8(*picture)[160])
{
	
#define RAMPLIMMIT   35
	int temp1 = 0;
	int temp2 = 0;
	int temp3 = 0;
	if(effect > 2)
		return 0;
	if(StraightLineFind2(effect,picture) == 0 && StructRamp == 0)
		return 0;
	
	temp1 = lineinf[1].RightPos - lineinf[1].LeftPos;
	temp2 = lineinf[2].RightPos - lineinf[2].LeftPos;
	temp3 = lineinf[3].RightPos - lineinf[3].LeftPos;
	
	if( temp1 > RAMPLIMMIT && temp2 > RAMPLIMMIT && temp3 > RAMPLIMMIT)
		return 1;
	else
		return 0;
}

/*========================================================================
*  ��������: GiveMiddle           
*  ����˵�������߸�ֵ����
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
========================================================================*/
void  GiveMiddle(int effect, uint8(*picture)[160])
{
	int i;
	int k = 0;
	
	//if (IsCurveLine == 0)
	//{
	for (i = down; i > effect; i--)
		lineinf[i].MiddlePos = (lineinf[i].LeftPos + lineinf[i].RightPos) / 2;
	
	k = effect+2; 
	
	if(effect>=0 && effect < 55)        //�ж����ߴ������ƣ����߹�һ��
	{
		if(lineinf[down].MiddlePos<lineinf[effect + 2].MiddlePos && k != up)
			PatchLineByM(k,lineinf[k].MiddlePos, up, right);
		else if(lineinf[down].MiddlePos>lineinf[effect + 2].MiddlePos && k != up)
			PatchLineByM(k,lineinf[k].MiddlePos, up, left);
		else if( k != 0)
			PatchLineByM(k,lineinf[k].MiddlePos, 0, 80 );
		else{}
		
		if(StructRamp != 0)
			PatchLineByM(k,lineinf[k].MiddlePos, 0, 80 );
	}
	
	
}


/*========================================================================
*  ��������: show           
*  ����˵����EASY_SHOW����
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
========================================================================*/
void show(int effect, uint8(*picture)[160])
{
	int i;
	
	for (i = 59; i > effect; i--)
	{
		picture[i][lineinf[i].RightPos] = 255;
		picture[i][lineinf[i].LeftPos] = 255;
		picture[i][lineinf[i].MiddlePos] = 255;
		
	}
	
	for (i = effect+1; i > 0; i--)
	{
		picture[i][lineinf[i].MiddlePos] = 0;
	}
	
	for(i = down; i > 0 ; i--)
	{
		picture[i][Ldem[i]] = 255;
		picture[i][Rdem[i]] = 255;
	}
}

/*========================================================================
*  ��������: Safe_Mode           
*  ����˵������ȫģʽ
*  ����ʱ�䣺2017
*  �޸�ʱ�䣺2017
*  ����˵����
========================================================================*/
//void Safe_Mode(uint8(*picture)[160])
//{
//	if(picture[down][80] == 255 && picture[down-1][80] == 255
//	   && picture[down-2][80] == 255 && picture[down-3][80] == 255
//		   && picture[down-4][80] == 255 && IsZebarLine == 0 )
//		Safe_Mode_Flag = 1;
//	else
//		Safe_Mode_Flag = 0;
//}

/*========================================================================
*  ��������: RoadInit           
*  ����˵����init
*  ����ʱ�䣺2016
*  �޸�ʱ�䣺2016
*  ����˵����
========================================================================*/
int EFFECT = 0;
int tend = 0;
int StrFlag = 0;
int IsStraightLine = 0;
int IsExceedOnStraightLineFind = 0;
int IsCancelExceed = 0;
int CanExceed = 0;
int DistanceExceed = 59;
int ExceedNo = 0;
int IsUlt = 0;
int ZebarDistance = 0;
int CanZebarLine = 0;
int SError = 0;
int IsObstacle = 0;
int StrucIsObstacle = 0;
int CanObstacle = 0;
int IsJumpOb = 0;
int StrCircleCount = 0;
int StructCircleLast = 0;
int SpeedUp = 0;
int StructSpeedUp = 0;
float SpeedTend = 0;
int StructZebarline = 0;

int StraightLineLong = 0;
int Control_S=0;
void RoadInit(uint8(*picture)[160])
{
	
	EFFECT = 0;
	
	StructCircleFlag = 0;
	if(StructCircle != StructCircleLast)
	{
		StrCircleCount++;
		StructCircleLast = StructCircle;
	}
	
	/*˫������ѡ��*/
	
	switch(StrCircleCount)
	{
	case 1:
                RoadDir = 1;  //2 right
		break;
	case 2:
                RoadDir = 1;
		break;
	case 3:
                RoadDir = 1;//1 left
		break;
        case 4:
                RoadDir = 1;
                break;
        case 5: 
                RoadDir = 1;
                break;
        case 6:
                RoadDir = 1;
                break;
        case 7:
                RoadDir = 1;
                break;
	case 0:
                RoadDir = 1;
		break;
                
	default:
		break;
	}
	if(StrCircleCount == 8)
		StrCircleCount = 0;
	
	
//	if(DelayZebarCount >= 3)
//		Safe_Mode(picture);
//	
	StructInit();
	FindFirstPos(picture);
	FindOtherPos(0, picture);
	EFFECT = EffectiveGet2(picture);//EffectiveGet(lineinf[down].LeftPos, lineinf[down].RightPos, picture);
	FindFirstPos(picture);
	FindOtherPos(EFFECT, picture);
	tend = ReturnTendency2(picture);
	
	/*************�µ�**************************/
	IsRamp = RampFind(EFFECT,picture);
	if(StructRamp == 0 && IsRamp == 1) //ʶ���µ�
		StructRamp = 1;
	if(StructRamp == 1 && IsRamp == 0) //����
		StructRamp = 2;
	if(StructRamp == 2 && IsRamp == 1) //����
		StructRamp = 3;
	if(StructRamp == 3 && EFFECT > 2)  //���־λ
		StructRamp = 0;
	/*******************************************/
	
	for(int i = down; i > EFFECT; i--)
	{
		TrueRight[i] = lineinf[i].RightPos;
		TrueLeft[i] = lineinf[i].LeftPos;
	}
	
	/**********������******************************/
	if(IsZebarLine != 2 && STATUSDO == FRONT)
		IsZebarLine = ZebarLine(EFFECT, picture);
	if(CanZebarLine == 1 && STATUSDO == BEHIND && IsZebarLine != 2)
		IsZebarLine = ZebarLine(EFFECT, picture);  
	if(IsZebarLine == 1)
		StructZebarline = 1;
	if(IsZebarLine == 0 && StructZebarline == 1)
		IsZebarLine = 2;	
	ZebarDistance = ZebarLineDistance(EFFECT, picture);
	/**********�ϰ�********************************/
	if(STATUSDO == FRONT)
	{
		if(StructCircle == 0)
			IsObstacle = ObstacleFind(EFFECT, picture);
		if(IsObstacle != 0  && StructCircle == 0)
		{
			if(IsObstacle == 1)
				StrucIsObstacle = 1;
			else if(IsObstacle == 2)
				StrucIsObstacle = 2;
		}
		if(StrucIsObstacle != 0)
		{
			IsJumpOb = IsMiddleJump(EFFECT, picture);
			StraightLinePatch2(EFFECT, picture, StrucIsObstacle);
		}
		if(IsObstacle == 0 && StrucIsObstacle != 0  && IsJumpOb == 1)
			CanObstacle = 1;
		
		if(CanObstacle == 1 && IsJumpOb == 0)
		{
			StrucIsObstacle = 0;
			CanObstacle = 0; ///////////////////////////////////////////////////////////////
		}
	}

	/*****************************************************/
	/*˫����·��־λʶ��*/
//	if(IsZebarLine == 0 && StrucIsObstacle == 0 && ISNOOB == 1)
//	{
//		if(STATUSDO == FRONT)
//			CircleFind(EFFECT, picture);
//		else
//			CircleFindByFront(EFFECT, picture,1,StrFlag); //����ʱ�󳵵���
//		
//		/*��·������־λʶ��*/
//		if(STATUSDO == FRONT)
//			StructCircleDeal(EFFECT, picture);
//		if(STATUSDO == BEHIND && B2FFlag == 0)
//			StructCircleDealStart(EFFECT, picture);
//	}
//	else if(IsZebarLine == 0 && StrucIsObstacle == 0 && ISNOOB == 0)
//	{
		CircleFind(EFFECT, picture);
		StructCircleDealStart(EFFECT, picture);
//	}
	/*****************************************************************************/
	
	/*���ಹ��*/
	if(IsZebarLine == 0)
	{
		if (IsCircle == 1)
			CircleHandle2(EFFECT, picture,RoadDir);
		
		if (IsCircle == 0)
			CrossLineFeNormal(EFFECT, picture);
		
		if (StructCircle == 0 && IsCircle == 0)
			CrossLineNormal3(EFFECT, picture);
		
		if (StructCircle == 1 && IsCircle == 0)
		{
			if(RoadDir == 2)
				CrossLineNormal4(EFFECT, picture,2);
			else if(RoadDir == 1)
				CrossLineNormal4(EFFECT, picture,1);
		}
	}
	/*============================================================================*/
	
//	if(IsZebarLine == 0 && IsCross == 0 && StructRamp == 0 && ISNOOB == 1)
//	{
//		if(STATUSDO == BEHIND && CanExceed == 1)
//		{
//			if(IsCross == 0 && IsCircle == 0)
//			{
//				IsExceedOnStraightLineFind = ExceedOnStraightLineFind(EFFECT, picture);
//				if(IsExceedOnStraightLineFind)
//					IsUlt = 0;
//				if(IsCancelExceed == 0)
//				{
//					IsCancelExceed = CancelStructExceed2(EFFECT, picture);
//					if(IsCancelExceed)
//						IsUlt = 1;
//				}
//			}
//			
//			if (IsCross == 0 && IsExceedOnStraightLineFind != 0)
//				ExceedOnStraightLineDo(IsExceedOnStraightLineFind, EFFECT, picture);
//			
//			if(StructExceed == 1 && IsExceed == 0)
//				StraightLinePatch(EFFECT, picture, 2); 
//		}
//		/*ֱ������ǰ��ͣ��*/
//		if(STATUSDO == FRONT && ExceedNo == 0)
//		{
//			if(IsCross == 0 && IsStraightLine == 0)
//				IsStraightLine = StraightLineFind( EFFECT, picture);
//			if(IsStraightLine)
//				StraightLinePatch(EFFECT, picture, 2); 
//		}
//		if(STATUSDO == BEHIND && IsStraightLine == 1)
//		{
//			if(IsStraightLine)
//				StraightLinePatch(EFFECT, picture, 2); 
//			DistanceExceed = DistanceDig(EFFECT, picture);
//		}
//	}
	
	//=========�Ӽ���=================================================//       
	//if(StraightLineLong==0)
        //  StraightLineLong = StraightLineFind( EFFECT, picture);
        SpeedTend = ReturnTendency8(picture);
	if(StructSpeedUp == 0)
		StructSpeedUp = StraightLineFind4( EFFECT, picture);
//        if(StraightLineLong)
//        {
//            	if(SpeedTend != 0 && StructSpeedUp == 1 && 
//	   ((fabs(SpeedTend) > 0.9 && EFFECT < 2) || EFFECT > 4 || fabs(SpeedTend) > 1))
//                {
//                    StraightLineLong=0;
//                }
//               	if(StraightLineLong == 1&& StrucIsObstacle == 0 && StructRamp == 0 &&StructCircle == 0)
//                {
//                      right_set=85;
//                      left_set=85;
//                }
//                else
//                {
//                      right_set=55;
//                      left_set=55;
//                }
//                    
//        }
//	else
//        {
            if(SpeedTend != 0 && StructSpeedUp == 1 && 
	   ((fabs(SpeedTend) > 0.3&& EFFECT < 5) || EFFECT >= 5  ))//|| fabs(SpeedTend) > 0.5)
		StructSpeedUp = 0;
	
	if(StructSpeedUp == 1&& StrucIsObstacle == 0 && StructRamp == 0 &&StructCircle == 0&&SCrossCount<5)
        {
             right_set=SPEED_Stright;
             left_set=SPEED_Stright;
             diff_cancel=1;
             Control_S = 1;        
        }
        else if(SCrossCount == 5)
        {
                right_set=SPEED_Other;
                left_set=SPEED_Other;
                diff_cancel=0;
                Control_S = 0;
              
             
                     
        }
	else if(SCrossCount == 6)
	{
             right_set = SPEED_Other;
             left_set = SPEED_Other;
           

                Control_S = 0;
                diff_cancel=0;  
        }
        else
        {
            right_set=SPEED_Other;
            left_set=SPEED_Other;
            diff_cancel=0;
            Control_S = 0;
        }
        if(StructRamp)
        {
                 diff_cancel = 1;
                 right_set=SPEED_Other - 7;
                 left_set=SPEED_Other - 7;
                 Control_S = 0;
        }
        if(StructCircle)
        {
                 right_set = SPEED_Circle;
                 left_set = SPEED_Circle;
                 diff_cancel = 0;
                 Control_S = 0;
        }
  //      }
	//==============================================================//     
	
	/*============================================================================*/
	
	if(IsStraightLine  == 0 && StrucIsObstacle == 0 && (!(StructExceed == 1 && IsExceed == 0)))
		GiveMiddle(EFFECT, picture); 
	
	//Сs����
	Cross_S_Find2(EFFECT, picture);
	if(SCrossCount == 5 && SPos[0] > 15)
		Cross_S_Line3(EFFECT, picture);
	
	//show(EFFECT, picture);
	SError = ErrorReturn(EFFECT, picture);
	
//	if(StrucIsObstacle)
//        {  GPIO_WriteBit(HW_GPIOC, 10, 1);}
//        else
//        {  GPIO_WriteBit(HW_GPIOC, 10, 0);}
//	
	
//	/*������*/ 
//	//if(SCrossCount == 5)
//	if(SCrossCount==5)//HalfFlag == 1 && CircleStatus == 0)
//		BuzzerOn();
//	else
//		BuzzerOff();
	
	
	
}

/*
��������
*/

/*========================================================================
*  ��������: LedSeeImage           
*  ����˵����OLED��ʾͼ����
*  ����ʱ�䣺2017
*  �޸�ʱ�䣺2017
*  ����˵����
========================================================================*/
void LedSeeImage(uint8(*picture)[160])
{
	int i = 0;
	int j = 0;
	
	int temp = 0;
	
	
	for(i = 0; i < 8; i++ )
	{
		LCD_Set_Pos( 0, i);
		
		if(i < 7)
		{
			for(j = 20; j < 140; j++)  //20
			{
				temp = 0;
				if(!picture[i*8 + 0][j])     temp|=0x01;  //�� 1 ��ͼ��
				if(!picture[i*8 + 1][j])     temp|=0x02;  //�� 2 ��ͼ��
				if(!picture[i*8 + 2][j])     temp|=0x04;  //�� 3 ��ͼ��
				if(!picture[i*8 + 3][j])     temp|=0x08;  //�� 4 ��ͼ��
				if(!picture[i*8 + 4][j])     temp|=0x10;  //�� 5 ��ͼ��
				if(!picture[i*8 + 5][j])     temp|=0x20;  //�� 6 ��ͼ��
				if(!picture[i*8 + 6][j])     temp|=0x40;  //�� 7 ��ͼ��
				if(!picture[i*8 + 7][j])     temp|=0x80;  //�� 8 ��ͼ��
				LCD_WrDat(temp);
			}
		}
		if(i == 7)
			
		{
			for(j = 20; j < 140; j++)
			{
				temp = 0;
				if(!picture[i*8 + 0][j])     temp|=0x01;  //�� 57 ��ͼ��
				if(!picture[i*8 + 1][j])     temp|=0x02;  //�� 58 ��ͼ��
				if(!picture[i*8 + 2][j])     temp|=0x04;  //�� 59 ��ͼ��
				if(!picture[i*8 + 3][j])     temp|=0x08;  //�� 60 ��ͼ��
				LCD_WrDat(temp);
			}
		}
	}
	
}










