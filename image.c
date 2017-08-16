/*
每日摘要
*/
/*
检查feature的值**************************************************
检查 lineinf[j].LeftPos  == right 的问题
检查 lineinf[j].LeftPos  == left 的问题
检查 lineinf[j].RightPos == right 的问题
检查 lineinf[j].RghtPos  == left 的问题

*/

#include "common.h"
#include "image.h"
#include "math.h"
#include "steer_control.h"
#include "OLED.h"
#include "speed_control.h"
#include "gpio.h"

typedef struct Cir
{
  	int x;
	int y;
}POS;

uint8 picture[60][160];
uint8(*picturess)[160];
uint8 Matrix[45][120] = { 0 };

LineType lineinf[60];

float RowWeight[60] = { 0 };
int TrueRight[60] = { 0 };
int TrueLeft[60] = { 0 };

int StraightLine2[60] =
{ 30,30,30,30,29,29,29,29,28,28,
28,28,28,27,27,27,26,26,26,25,
25,25,24,24,23,23,22,22,22,21,
22,21,21,21,20,19,18,17,17,16,
16,16,15,15,15,14,14,13,12,12,
12,11,10,10, 9, 8, 6, 5, 4, 3 };

int StraightLine[60] =
{ 30,30,30,30,29,29,29,29,28,28,
28,28,28,27,27,27,26,26,26,25,
25,25,24,24,23,23,22,22,22,21,
22,21,21,21,20,19,18,17,17,16,
16,16,15,15,15,14,14,13,12,12,
12,11,10,10, 9, 8, 6, 5, 4, 3 };



int Rdem[60] =
{ 84,84,85,85,86,86,87,87,88,88,
89,90,90,91,91,92,92,93,93,94,
95,95,96,96,96,97,97,98,98,99,
99,100,100,101,101,102,102,102,103,103,
103,104,104,104,105,105,105,106,106,106,
106,107,107,107,108,108,108,108,109,109 };

int Ldem[60] =
{ 75,75,75,74,74,74,73,73,72,72,
71,70,70,69,69,68,68,67,67,66,
66,65,65,65,64,63,63,62,62,62,
61,61,60,60,59,59,58,58,57,57,
56,56,55,55,55,54,54,53,53,53,
52,52,51,51,51,50,50,49,49,49 };

int RoadDir = 0;
int dada = 0;
int Safe_Mode_Flag = 0;
/*环路全局标志位*/
int CircleDir = 1;
int IsCircle = 0;
int CircleUp = 0;
int CircleDown = 0;
int CircleLeft = 159;
int CircleRight = 0;
int BlackCircleLeft[60] = { 80 };
int BlackCircleRight[60] = { 80 };
int StructCircle = 0;
int StructCircleFlag = 0;
/*小S程序*/
int IsSLine = 0;
/*十字程序*/
int IsCross = 0;
/*========================================================================
*  函数名称:  IsPatchHasBlack
*  功能说明：
*  创建时间：2017
*  修改时间：2017
*  参数说明:
========================================================================*/
int IsPatchHasBlack(int pos1_x, int pos1_y, int pos2_x, int pos2_y)
{
    float k = 0, b = 0;
	int sum = 0;
	
	k = (float)((float)(pos2_y - pos1_y) / (float)(pos2_x - pos1_x));
	b = (float)(pos2_y - (float)(k * pos2_x));
	
	for(int i = (pos1_x+1); i > pos2_x; i--)
	{
		if(picture[i][(int)(k*i+b)] == 255)
			sum++;
	}
	
	if(fabs(pos1_x-pos2_x) > 10 && sum > 3)
		return 1;
	else if(fabs(pos1_x-pos2_x) > 5 && sum > 2)
		return 1;
	else 
		return 0;
}
/*========================================================================
*  函数名称:  RLCount
*  功能说明：
*  创建时间：2017
*  修改时间：2017
*  参数说明:
========================================================================*/
int RLCount(int effect, uint8(*picture)[160], int dir )
{
	int sum = 0;

	switch(dir)
	{
	case 1:
		for (int i = down; i > effect+1; i--)
		{
			if(lineinf[i].LeftPos <= lineinf[i-1].LeftPos && lineinf[i].LeftPos >= lineinf[down].LeftPos)
				sum++;
		}
		break;
	case 2:
		for (int i = down; i > effect+1; i--)
		{
			if(lineinf[i].RightPos >= lineinf[i-1].RightPos && lineinf[i].RightPos <= lineinf[down].RightPos)
				sum++;
		}
		break;
	default:
		break;
	}
	
	if(sum > down - effect - 3)
		return 1;
	else
		return 0;


}
/*========================================================================
*  函数名称:  delax
*  功能说明：
*  创建时间：2017
*  修改时间：2017
*  参数说明:
========================================================================*/
float delax(int effect, uint8(*picture)[160])
{
	float sum = 0;

	for (int i = 40; i > 20; i--)
		sum += fabs(lineinf[i].MiddlePos - 80);

	sum = sum / (down - 20);

	return sum;

}
/*========================================================================
*  函数名称:  IsRightLeft
*  功能说明：
*  创建时间：2017
*  修改时间：2017
*  参数说明:
========================================================================*/
int IsRightLeft(int effect, int dir, int pos)
{
	int i = 0;
	int count = 0;
	switch (dir)
	{
	case 1:
		for (i = pos; i >= ReturnBigger(pos - 23, effect); i--)
		{
			if (lineinf[i].LeftPos >= lineinf[pos].LeftPos && lineinf[i].LeftPos != left)
				count++;
		}
		break;
	case 2:
		for (i = pos; i >= ReturnBigger(pos - 23, effect); i--)
		{
			if (lineinf[i].RightPos <= lineinf[pos].RightPos && lineinf[i].LeftPos != right)
				count++;
		}
		break;
	default:
		break;

	}
	if (count > 21)
		return 0;
	else
		return 1;
}
/*========================================================================
*  函数名称:  ErrorReturn
*  功能说明： 返回偏差
*  创建时间：2017
*  修改时间：2017
*  参数说明:
========================================================================*/
int ErrorReturn(int effect, uint8(*picture)[160])
{
	if (effect > 25)
		return 0;

	int M[60] = { 0 };
	for (int i = 35; i > 25; i--)
		M[i] = (lineinf[i].LeftPos + lineinf[i].RightPos) / 2;
	int sum = 0;
	for (int i = 35; i > 25; i--)
		sum += (int)fabs(M[i] - 80);

	return sum;
}
int ErrorReturn2(int effect, uint8(*picture)[160])
{
	if (effect > 25)
		return 0;

	int M[60] = { 0 };
	for (int i = 35; i > 25; i--)
		M[i] = (lineinf[i].LeftPos + lineinf[i].RightPos) / 2;
	int sum = 0;
	for (int i = 35; i > 25; i--)
		sum += (int)(M[i] - 80);

	return sum;
}
int ErrorReturn3(int effect, uint8(*picture)[160])
{
	if (effect > 25)
		return 0;

	int M[60] = { 0 };
	for (int i = 25; i > 20; i--)
		M[i] = lineinf[i].MiddlePos;
	int sum = 0;
	for (int i = 25; i > 20; i--)
		sum += (int)(M[i] - lineinf[down].MiddlePos);

	return (int)fabs(sum);
}

/*========================================================================
*  函数名称:  ReturnBigger
*  功能说明：比较最大小的程序
*  创建时间：2016
*  修改时间：2016
*  参数说明：
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
*  函数名称:  LimitPr
*  功能说明：限制点的范围，使之不会超过左右边界
*  创建时间：2016
*  修改时间：2016
*  参数说明：
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
*  函数名称:  TriFind
*  功能说明：三角检测
*  创建时间：2016
*  修改时间：2016
*  参数说明：检测到 返回 0
否则   返回 1
========================================================================*/
int TriDown = 0;
int TriCount = 0;
int TriFlag = 0;
int TriFlag2 = 0;
int TriPos = 0;
int TriFind(int effect, uint8(*picture)[160], int dir)
{
#define TriFindLimit     (10)
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
			if (lineinf[i].LeftPos != left && DDFlag == 0)
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
			if (TriCount >= ReturnBigger(59 - TriDown, 0))
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
			if (lineinf[i].RightPos != right && DDFlag == 0)
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
			if (TriCount >= ReturnBigger(59 - TriDown, 0))
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
*  函数名称:  ReturnTendency
*  功能说明：趋势检索
*
创建时间：2016
*  修改时间：2016
*  参数说明：左则返回 1
右则返回 2
========================================================================*/
#define LeftDir    1
#define RightDir    2
#define MiddleDir   3
float ReturnTendency(uint8(*picture)[160], int effect, int Dir, int High, int Low)
{
	int i = 0;

	float AVE_X = 0;
	float AVE_Y = 0;
	float aa = 0;
	float bb = 0;
	float SLOPE = 0;

	if (EFFECT > High)
		return 0;

	for (i = Low; i > High; i--)
		AVE_X += i;
	AVE_X = AVE_X / (Low - High + 1);

	switch (Dir)
	{
	case LeftDir:

		for (i = Low; i > High; i--)
			AVE_Y += lineinf[i].LeftPos;
		AVE_Y = AVE_Y / (Low - High + 1);

		for (i = Low; i > High; i--)
			aa += (i - AVE_X) * (lineinf[i].LeftPos - AVE_Y);

		for (i = Low; i > High; i--)
			bb += (i - AVE_X) * (i - AVE_X);

		break;
	case RightDir:

		for (i = Low; i > High; i--)
			AVE_Y += lineinf[i].RightPos;
		AVE_Y = AVE_Y / (Low - High + 1);

		for (i = Low; i > High; i--)
			aa += (i - AVE_X) * (lineinf[i].RightPos - AVE_Y);

		for (i = Low; i > High; i--)
			bb += (i - AVE_X) * (i - AVE_X);

		break;
	case MiddleDir:

		for (i = Low; i > High; i--)
			AVE_Y += lineinf[i].MiddlePos;
		AVE_Y = AVE_Y / (Low - High + 1);

		for (i = Low; i > High; i--)
			aa += (i - AVE_X) * (lineinf[i].MiddlePos - AVE_Y);

		for (i = Low; i > High; i--)
			bb += (i - AVE_X) * (i - AVE_X);

		break;
	default:
		return 0;
		break;
	}

	if (bb != 0)
		SLOPE = aa / bb;

	return SLOPE;
}


int ReturnTendency5(uint8(*picture)[160])
{
	int slope = 0;
	int limit5 = 0;
	limit5 = (lineinf[down].MiddlePos + lineinf[down - 1].MiddlePos + lineinf[down - 2].MiddlePos) / 3;
	for (int i = down; i > 50; i--)
	{
		if (lineinf[i].MiddlePos < limit5)
			slope--;
		else if (lineinf[i].MiddlePos > limit5)
			slope++;
	}
	if (slope > 0)
		return 2;
	else
		return 1;

}

/*========================================================================
*  函数名称: CountLeftRight
*  功能说明：左右两边数点
*  创建时间：2016
*  修改时间：2016
*  参数说明：左边多 返回 1
右边多 返回 0
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
*  函数名称: CountLine
*  功能说明：选行记黑点数
*  创建时间：2016
*  修改时间：2016
*  参数说明：返回值 黑点的个数
========================================================================*/
int CountLine(int which, uint8(*picture)[160])
{
	int count = 0;

	for (int i = TrueLeft[which]; i <= TrueRight[which]; i++)
	{
		if (picture[which][i] == 0)
			count++;
	}
	return count;
}

int CountLine2(int which, uint8(*picture)[160])
{
	int count = 0;

	for (int i = TrueLeft[which]; i <= TrueRight[which]; i++)
	{
		if (picture[which][i] == 255)
			count++;
	}
	return count;
}


/*========================================================================
*  函数名称: PatchLineByL/M/R
*  功能说明：两点连线，连左边界 中线 右边界
PatchLineByL ---- 左侧边界连点函数，给定linelnf.LeftPos值
PatchLineByM ---- 中线边界连点函数，给定linelnf.MiddlePos值
PatchLineByR ---- 右侧边界连点函数，给定linelnf.RightPos值
*  创建时间：2016
*  修改时间：2016
*  参数说明：
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
*  函数名称: CurveSlope
*  功能说明：曲线斜率
*  方法:     二插法计算曲线斜率
*  创建时间：2016
*  修改时间：2016
*  参数说明：返回值 相应的曲线斜率
========================================================================*/
float CurveSlope(int x, int pos[])
{
	float SUM_XY = 0;
	float SUM_X = 0;
	float SUM_Y = 0;
	float SUM_XX = 0;
	float SUM_X_SUM_X = 0;
	float a = 0;

	if (x < 2)
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
*  函数名称: StructInit
*  功能说明：相关初始化
*  创建时间：2016
*  修改时间：2016
*  参数说明：
========================================================================*/
void StructInit()
{
	int i = 0;

	for (i = 0; i < 60; i++)
	{
		lineinf[i].Leftleft = 0;
		lineinf[i].Rightright = 159;
		lineinf[i].MiddlePos = 80;
		BlackCircleLeft[i] = 80;
		BlackCircleRight[i] = 80;
	}

	/*
	环路
	*/
	IsCircle = 0;
	CircleUp = 0;
	CircleDown = 0;
	CircleLeft = 159;
	CircleRight = 0;

	IsCross = 0;
}


/*========================================================================
*  函数名称: FindFirstPos            FindOtherPos
*  功能说明：找第一行边界           找其他行边界
*  创建时间：2016
*  修改时间：2016
*  参数说明：
========================================================================*/
void FindFirstPos(uint8(*picture)[160])
{
	/*
	变量初始化
	*/
	int  row = 80;    //定义列向量

					  /*
					  相关算法
					  */
	for (row = 80; row >= left; row--)                                  //找第一行的左边界点
	{
		if (picture[59][row] != picture[59][row + 1])                     //判断两个点是否相同的方法找左边界点
		{
			if (picture[59][row + 1] == picture[59][row + 2])               //滤波
			{
				lineinf[59].LeftPos = row;
				break;
			}
			else if (row == left)                                       //如果没有则判断左边界点为左边界
				lineinf[59].LeftPos = left;
			else {}
		}
		else if (row == left)                                            //如果没有则判断左边界点为左边界
			lineinf[59].LeftPos = left;
		else {}
	}
	for (row = 80; row <= right; row++)                                  //找第一行的右边界点
	{
		if (picture[59][row] != picture[59][row - 1])                      //判断两个点是否相同的方法找左边界点
		{
			if (picture[59][row - 1] == picture[59][row - 2])                //滤波
			{
				lineinf[59].RightPos = row;
				break;
			}
			else if (row == right)                                       //如果没有则判断右边界点为右边界
				lineinf[59].RightPos = right;
			else {}
		}
		else if (row == right)                                           //如果没有则判断右边界点为右边界
			lineinf[59].RightPos = right;
		else {}
	}
}


void FindOtherPos(int effect, uint8(*picture)[160])
{
	/*
	相关变量
	*/
	int column = 58;   //定义行向量
	int row = 80;      //定义列向量

					   /*
					   相关算法
					   算法说明：根据相关点上方的点的颜色来判断边界。如果为黑色则向相对中心寻找，否则向相对边界寻找
					   解释一：对于左边界来说可能出现很多左边界都为left值的情况，所以对于左边界来说在未找到边界出现(row == (left))的情况，右边界同理
					   */
	for (column = 58; column >= effect; column--)
	{
		if (picture[column][lineinf[column + 1].LeftPos] == 255)                     //判断上方点的颜色（黑色）
		{

			if (lineinf[column + 1].LeftPos == right)
				lineinf[column].LeftPos = right;
			else
			{
				for (row = lineinf[column + 1].LeftPos; row < right; row++)              //对于左边界来说若上方点为黑色，则向右寻找
				{
					if (picture[column][row] != picture[column][row + 1])
					{
						lineinf[column].LeftPos = row;
						break;
					}
					else if (row == (right - 1) || (row == (right)))                      //如果没有找到则赋值为右边界
						lineinf[column].LeftPos = right;
					else {}
				}
			}

		}
		else                                                                          //判断上方点的颜色（白色）
		{
			for (row = lineinf[column + 1].LeftPos; row >= left; row--)               //对于左边界来说若上方点为白色，则向右寻找
			{
				if (picture[column][row] != picture[column][row + 1])
				{
					lineinf[column].LeftPos = row;
					break;
				}
				else if ((row == (left + 1)) || (row == (left)))                      //如果没有找到则赋值为左边界
					lineinf[column].LeftPos = left;
				else {}

			}
		}
		//右边界也是一样的，不注释了，太懒了， ^0^
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
					lineinf[column].RightPos = row;
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
*  函数名称: EffectiveGet
*  功能说明：找图像有效行
*  创建时间：2016
*  修改时间：2016
*  参数说明：返回值 图像的有效行 、
*  备注   ：这个程序不是我写的
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
	for (i = down; i > up; i--)
	{
		if (lineinf[i].RightPos - lineinf[i].LeftPos <= 5 || lineinf[i].LeftPos > 155 || lineinf[i].RightPos < 5)
			return i;
		if (i == 1)
			return 1;
	}

	return 0;
}

/*========================================================================
*  函数名称: 十字 小s
*  功能说明：
*  相关说明：Cross_S_Line      ：   对小S弯进行处理（效果很一般呀）
Cross_S_Line2     ：   对小S弯进行处理（效果还好）
CrossLineNormal   ：   对十字进行处理，根据值比较大小进行比较的方法（不怎么好用）
CrossLineNormal2  ：   对十字进行处理，根据斜率大小进行比较的方法
CrossLineFeNormal ：   对出十字的形况进行处理
BabyFind          ：   斜切十字补线
BabyFind2         ：   斜切十字补线
*  创建时间：2016
*  修改时间：2016
*  参数说明：
*  备注：    曲线斜率，霍夫变换
========================================================================*/
int BabyDownBigger = 0;
int BabyDownPosBigger = 0;
//相关的左右侧曲率限制范围的值的宏定义
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
int SPos[60] = { 0 }; int tendS = 0; float aa = 0;
int Cross_S_Find2(int effect, uint8(*picture)[160])
{
	IsSLine = 0;
	SFlag = 0;
	SCrossCount = 0;
	int i = 0;

	for (i = down; i >= up; i--)
		SPos[i] = 0;

	tendS = ReturnTendency5(picture);

	if (tendS == 1)
		SFlag = 1;
	if (tendS == 2)
		SFlag = 2;

	for (i = 55; i > effect + 3; i--)
	{
		if (lineinf[i].MiddlePos - lineinf[i - 1].MiddlePos <= 0 && lineinf[i - 1].MiddlePos - lineinf[i - 2].MiddlePos > 0
			&& lineinf[i - 2].MiddlePos - lineinf[i - 3].MiddlePos >= 0 && SFlag == 2)
		{
			SPos[SCrossCount] = i - 1;
			break;
		}
		if (lineinf[i].MiddlePos - lineinf[i - 1].MiddlePos >= 0 && lineinf[i - 1].MiddlePos - lineinf[i - 2].MiddlePos < 0
			&& lineinf[i - 2].MiddlePos - lineinf[i - 3].MiddlePos <= 0 && SFlag == 1)
		{
			SPos[SCrossCount] = i - 1;
			break;
		}


		if (i == effect + 2) return 0;
	}

	for (i = SPos[0]; i > effect + 2; i--)
	{
		switch (SFlag)
		{
		case 1:
			if (SCrossCount % 2 == 0)
			{
				if ((lineinf[i].MiddlePos - lineinf[i - 1].MiddlePos) <= 0 && (lineinf[i - 1].MiddlePos - lineinf[i - 2].MiddlePos) > 0 && lineinf[i - 2].MiddlePos - lineinf[i - 3].MiddlePos >= 0
					&& SPos[SCrossCount] - i > 5 && fabs(lineinf[i - 1].MiddlePos - lineinf[SPos[SCrossCount]].MiddlePos) > 5)
				{
					SCrossCount++;
					SPos[SCrossCount] = i - 1;
				}
			}
			if (SCrossCount % 2 == 1)
			{
				if ((lineinf[i].MiddlePos - lineinf[i - 1].MiddlePos) >= 0 && (lineinf[i - 1].MiddlePos - lineinf[i - 2].MiddlePos) < 0 && lineinf[i - 2].MiddlePos - lineinf[i - 3].MiddlePos <= 0
					&& SPos[SCrossCount] - i > 5 && fabs(lineinf[i - 1].MiddlePos - lineinf[SPos[SCrossCount]].MiddlePos) > 5)
				{
					SCrossCount++;
					SPos[SCrossCount] = i - 1;
				}
			}
			break;
		case 2:
			if (SCrossCount % 2 == 1)
			{
				if ((lineinf[i].MiddlePos - lineinf[i - 1].MiddlePos) <= 0 && (lineinf[i - 1].MiddlePos - lineinf[i - 2].MiddlePos) > 0 && lineinf[i - 2].MiddlePos - lineinf[i - 3].MiddlePos >= 0
					&& SPos[SCrossCount] - i > 5 && fabs(lineinf[i - 1].MiddlePos - lineinf[SPos[SCrossCount]].MiddlePos) > 5)
				{
					SCrossCount++;
					SPos[SCrossCount] = i - 1;
				}
			}
			if (SCrossCount % 2 == 0)
			{
				if ((lineinf[i].MiddlePos - lineinf[i - 1].MiddlePos) >= 0 && (lineinf[i - 1].MiddlePos - lineinf[i - 2].MiddlePos) < 0 && lineinf[i - 2].MiddlePos - lineinf[i - 3].MiddlePos <= 0
					&& SPos[SCrossCount] - i > 5 && fabs(lineinf[i - 1].MiddlePos - lineinf[SPos[SCrossCount]].MiddlePos) > 5)
				{
					SCrossCount++;
					SPos[SCrossCount] = i - 1;
				}
			}
			break;
		default:
			break;

		}
	}
	int IsOverLR = 0;
	for (i = down; i > 3; i--)
	{
		if (lineinf[i].LeftPos > 80 || lineinf[i].RightPos < 80)
			IsOverLR++;
	}

	int topCount = 0;
	for(i = 40; i < 120; i++)
	{
		if(picture[2][i] == 0 && picture[2][i+1] == 0)
			topCount++;
	}
	if(topCount > 10)
		topCount = 100;
	
	if ( effect < 3 &&topCount == 100 && SCrossCount >= 1 && StructCircle == 0)
		IsSLine = 2;
	if (IsSLine == 2 && IsOverLR < 5)
		IsSLine = 1;
	if (IsSLine == 2)
		IsSLine = 0;

	return 0;
}


void Cross_S_Line2(int effect, uint8(*picture)[160])
{
	int  CSUP = 4;

	if (down != CSUP)
		PatchLineByM(down, 80, CSUP, (lineinf[CSUP].LeftPos + lineinf[CSUP].RightPos) / 2);
}

void Cross_S_Line3(int effect, uint8(*picture)[160])
{
	for (int i = down; i > effect; i--)
		lineinf[i].MiddlePos = (lineinf[i].MiddlePos + 80 * 2) / 3;
}

void Cross_S_Line4(int effect, uint8(*picture)[160])
{
	int temp1 = 0;
	int temp2 = 0;
	int Middle = 0;

	for (int i = 0; i < 5; i++)
	{
		if (SPos[i] < 30 && SPos[i] > 0 && temp1 == 0)
		{
			temp1 = SPos[i];
			continue;
		}
		if (SPos[i] < 30 && SPos[i] > 0 && temp1 != 0 && temp2 == 0)
		{
			temp2 = SPos[i];
			break;
		}
	}
	if (temp1 > 10 && temp2 > effect)
		Middle = (lineinf[temp1].MiddlePos + lineinf[temp2].MiddlePos) / 2;

	if (Middle < 75)
		Middle = 77;
	else if (Middle > 85)
		Middle = 83;
	else
		Middle = 80;

	for (int i = down; i > effect; i--)
		lineinf[i].MiddlePos = (lineinf[i].MiddlePos * 3 + Middle * 4) / 7;
}

void Cross_S_Line5(int effect, uint8(*picture)[160])
{

	int i;
	POS Pos0, Pos1;
	float b = 0,k = 0;
	
	Pos0.y = (lineinf[down].MiddlePos + lineinf[SPos[0]].MiddlePos) / 2;
	for(i = down; i > SPos[0]; i--)
	{
		if(fabs(lineinf[i].MiddlePos - Pos0.y) <= 2)
		{
			Pos0.x = i;
			break;
		}
	}
	
	Pos1.y = (lineinf[SPos[1]].MiddlePos + lineinf[SPos[0]].MiddlePos) / 2;
	for(i = SPos[0]; i > SPos[1]; i--)
	{
		if(fabs(lineinf[i].MiddlePos - Pos1.y) <= 2)
		{
			Pos1.x = i;
			break;
		}
	}
	if(Pos1.x != 0 && Pos0.x != 0)
	{
		k = (float)( (float)( Pos1.y - Pos0.y ) / ((float)( Pos1.x - Pos0.x)) );
		b = (float)( (float)Pos1.y - (float)(k * Pos1.x) );
		for(i = down; i > effect; i--)
			lineinf[i].MiddlePos = (int)(k * i + b);
	}

}



int RCrossDown = 0;
int RCrossUp = 0;
int LCrossDown = 0;
int LCrossUp = 0;
int IsDoing2 = 0;
int IsDoing = 0;
float RightTend = 0;
float LeftTend = 0;
int IsCross2 = 0;
int LeftCrossFePos = 58;   //我想要的左侧标志点
int RightCrossFePos = 58;  //我想要的右侧标志点
int tip1 = 0, tip2 = 0;
int CrossLineNormal3(int effect, uint8(*picture)[160])
{


#define feature (3)
#define limit   (15)
#define LLLIMIT  (20)
#define RRLIMIT  (140)

	int i;
	int j;
	int IsFlag = 0;
	int IsFlag2 = 0;
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
	IsDoing2 = 0;
	IsDoing = 0;
	int LEFT[60] = { left };
	int RIGHT[60] = { right };

	RightTend = ReturnTendency(picture, effect, 2, 18, 27);
	LeftTend = ReturnTendency(picture, effect, 1, 18, 27);

	for (i = down; i > effect + 1; i--)
	{
		if (lineinf[i].LeftPos > 135 || lineinf[i].RightPos < 25)
			return 0;
	}

	IsCross = 0;

	if (effect > 10)                        //判断是否可以进入十字检测程序
		return 0;
	if( RLCount(effect, picture, 1) == 1 || RLCount(effect, picture, 2) == 1)
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

	for (i = down; i > 14; i--)
	{
		if (LEFT[i] < 4)
			IsDoingFlag++;
		if (RIGHT[i] > 155)
			IsDoingFlag2++;
	}
	if (IsDoingFlag > 43)
		IsDoingFlag = 100;
	if (IsDoingFlag2 > 43)
		IsDoingFlag2 = 100;


	for (i = down; i > effect + 5; i--)
	{
		if (lineinf[i].LeftPos < 3 || lineinf[i].RightPos > 156)
		{
			IsCross2 = 1;
			break;
		}
	}
	if (IsCross2 == 0)
		return 0;


	/*
	左侧的十字找线程序
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //检测十字左下角的值
	{
		if ((((lineinf[i].LeftPos - lineinf[i - 1].LeftPos) <= 0) && ((lineinf[i - 1].LeftPos - lineinf[i - 2].LeftPos) > 0)))
		{
			LCrossDown = i - 1;
			LCrossFlag = 1;
			break;
		}
	}

	if (0 == TriFind(effect, picture, 1) && (LCrossFlag == 0 || (LCrossFlag == 1 && LCrossDown > 30 && LCrossDown != 0 && lineinf[LCrossDown].LeftPos < 60)))
	{
		if (TriFlag2 == 1)
			LCrossDown = TriPos;
		else
		{
			LCrossDown = down;
			IsFlag = 1;
		}
	}

	if ((LCrossDown != 0 && lineinf[LCrossDown].LeftPos != left)
		|| (LCrossDown != 0 && lineinf[LCrossDown].LeftPos == left && (RightTend < 0 || LCrossDown > 50)))                 //检测十字左上角的值
	{
		for (i = LCrossDown - 3; i > effect + 3; i--)
		{
			if ((CurveSlope(i, LEFT) > SLOPELEFTDOWN && CurveSlope(i, LEFT) < SLOPELEFTUP) && 
				((lineinf[i].LeftPos >= ReturnBigger(lineinf[LCrossDown].LeftPos - 25, left) && TriFind(effect, picture, 1) == 0) || (lineinf[i].LeftPos >= ReturnBigger(lineinf[LCrossDown].LeftPos - 12, left) && TriFind(effect, picture, 1) == 1)))
				//&& lineinf[i].LeftPos > left && lineinf[i].LeftPos != left + 1)
			{
				LCrossUp = ReturnBigger(effect, i - 3);
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
	if (((LCrossDown != 0 && LCrossUp != 0 && ((lineinf[LCrossDown].LeftPos - lineinf[LCrossUp].LeftPos > 20) || (LCrossDown - LCrossUp > 45)))
		|| ((LCrossDown - LCrossUp)> 30 && LCrossUp < 14 && lineinf[LCrossUp].LeftPos - lineinf[LCrossDown].LeftPos < 18 && ((LCrossDown > 55 && lineinf[LCrossDown].LeftPos < 40) || lineinf[LCrossDown].LeftPos < 30) && (lineinf[LCrossUp].RightPos - lineinf[LCrossUp].LeftPos) > 110))
		|| (IsPatchHasBlack(LCrossDown, lineinf[LCrossDown].LeftPos, LCrossUp, lineinf[LCrossUp].LeftPos) == 1 && LCrossUp != 0 && LCrossDown != 0))
		IsDoing = 1;
	/*******************************************************************/
	if ((LCrossDown != 0) && (LCrossUp != 0)) //左侧十字计数程序
	{
		for (i = LCrossDown; i > LCrossUp; i--)
		{
			if (lineinf[i].LeftPos <= left + 2)
				LCrossCount++;
		}
	}

	/*
	右侧的十字找线程序
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //检测十字右下角的值
	{
		if ((((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0) && ((lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0)))
		{
			RCrossDown = i - 1;
			RCrossFlag = 1;
			break;

		}
	}
	if (0 == TriFind(effect, picture, 2) && (RCrossFlag == 0 || (RCrossFlag == 1 && RCrossDown > 30 && RCrossDown != 0 && lineinf[RCrossDown].RightPos > 100)))
	{
		if (TriFlag2 == 1)
			RCrossDown = TriPos;
		else
		{
			RCrossDown = down;
			IsFlag2 = 1;
		}
	}

	if ((RCrossDown != 0 && lineinf[RCrossDown].RightPos != right) ||
		(RCrossDown != 0 && lineinf[RCrossDown].RightPos == right && (LeftTend > 0 || RCrossDown > 50)))                 //检测十字右上角的值
	{
		for (i = RCrossDown - 3; i > effect + 3; i--)
		{
			if ((CurveSlope(i, RIGHT) > SLOPERIGHTDOWN && CurveSlope(i, RIGHT) < SLOPERIGHTUP) && 
				((lineinf[i].RightPos <= ReturnSmaller(lineinf[RCrossDown].RightPos + 25, right) && 0 == TriFind(effect, picture, 2)) || (lineinf[i].RightPos <= ReturnSmaller(lineinf[RCrossDown].RightPos + 12, right) && 1 == TriFind(effect, picture, 2))))
				//&& lineinf[i].RightPos < right && lineinf[i].RightPos != right - 1)
			{
				RCrossUp = ReturnBigger(effect, i - 3);
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

	if ((RCrossDown != 0) && (RCrossUp != 0)) //右侧十字计数程序
	{
		for (i = RCrossDown; i > RCrossUp; i--)
		{
			if (lineinf[i].RightPos >= right - 2)
				RCrossCount++;
		}
	}

	/*********************************************************************/
	if (((RCrossDown != 0 && RCrossUp != 0 && ((lineinf[RCrossDown].RightPos - lineinf[RCrossUp].RightPos < -20) || (RCrossDown - RCrossUp > 45)))
		|| ((RCrossDown - RCrossUp)> 30 && RCrossUp < 14 && (lineinf[RCrossDown].RightPos - lineinf[RCrossUp].RightPos) < 18 && ((RCrossDown > 55 && lineinf[RCrossDown].RightPos > 120) || lineinf[RCrossDown].RightPos > 130) && (lineinf[RCrossUp].RightPos - lineinf[RCrossUp].LeftPos) > 110))
		|| (IsPatchHasBlack(RCrossDown, lineinf[RCrossDown].RightPos, RCrossUp, lineinf[RCrossUp].RightPos) == 1 && RCrossDown != 0 && RCrossUp != 0))
		IsDoing2 = 1;
	/*********************************************************************/
	/*
	补线检测程序
	*/
	/**********************************/
	if(IsDoing == 1)
	{
		if(IsFlag == 1)
			IsDoing = 0;
	}
	if(IsDoing2 == 1)
	{
		if(IsFlag2 == 1)
			IsDoing2 = 0;
	}
	/********************************/

	if (RCrossCount > 3 && IsDoing2 == 0)
	{
		if (RCrossDown != RCrossUp)
			PatchLineByR(RCrossDown, lineinf[RCrossDown].RightPos, RCrossUp - 1, lineinf[RCrossUp - 1].RightPos);
		IsCross = 1;
	}
	if (LCrossCount > 3 && IsDoing == 0)
	{
		IsCross = 1;
		if (LCrossDown != LCrossUp)
			PatchLineByL(LCrossDown, lineinf[LCrossDown].LeftPos, LCrossUp - 1, lineinf[LCrossUp - 1].LeftPos);
	}
	if (IsDoing == 1 && ((lineinf[RCrossDown].RightPos > 90 && RCrossDown != 0) || RCrossDown == 0) && (RightTend < 1.1 || (LCrossDown != 0 && RCrossDown != 0) || IsDoingFlag2 == 100 || RightCrossFePos != 58))
	{
		IsCross = 1;
		if (1 == BabyFind(effect, picture, LCrossDown, lineinf[LCrossDown].LeftPos, 1))
		{
			if (BabyDownBigger > effect + 1 && LCrossDown - BabyDownBigger < 45)
			{
				if ((fabs(BabyDownPosBigger - lineinf[BabyDownBigger].RightPos) > 3) && BabyDownBigger != LCrossDown)
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
						if (LCrossDown != BabyDownBigger - 2)
							PatchLineByL(LCrossDown, lineinf[LCrossDown].LeftPos, BabyDownBigger - 2, lineinf[BabyDownBigger - 2].LeftPos);
					}
				}
			}
		}
	}
	if (IsDoing2 == 1 && ((lineinf[LCrossDown].LeftPos < 70 && LCrossDown != 0) || LCrossDown == 0) && (LeftTend > -1.1 || (LCrossDown != 0 && RCrossDown != 0) || IsDoingFlag == 100 || LeftCrossFePos != 58))
	{
		IsCross = 1;
		if (1 == BabyFind(effect, picture, RCrossDown, lineinf[RCrossDown].RightPos, 2))
		{
			if (BabyDownBigger > effect + 1 && RCrossDown - BabyDownBigger < 45)
			{

				if ((fabs(BabyDownPosBigger - lineinf[BabyDownBigger].LeftPos) > 3) && (BabyDownBigger != RCrossDown))
				{
					PatchLineByR(RCrossDown, lineinf[RCrossDown].RightPos, BabyDownBigger, BabyDownPosBigger);
					if (BabyDownPosBigger == left) {}
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
						if (RCrossDown != BabyDownBigger - 2)
							PatchLineByR(RCrossDown, lineinf[RCrossDown].RightPos, BabyDownBigger - 2, lineinf[BabyDownBigger - 2].RightPos);
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

	IsDoing2 = 0;
	IsDoing = 0;
	int LEFT[60] = { left };
	int RIGHT[60] = { right };

	RightTend = ReturnTendency(picture, effect, 2, 18, 27);
	LeftTend = ReturnTendency(picture, effect, 1, 18, 27);


	IsCross = 0;

	if (effect > 15)                        //判断是否可以进入十字检测程序
		return 0;
	if( RLCount(effect, picture, 1) == 1 || RLCount(effect, picture, 2) == 1)
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

	for (i = down; i > 14; i--)
	{
		if (LEFT[i] == left)
			IsDoingFlag++;
		if (RIGHT[i] == right)
			IsDoingFlag2++;
	}
	if (IsDoingFlag > 43)
		IsDoingFlag = 100;
	if (IsDoingFlag2 > 43)
		IsDoingFlag2 = 100;


	for (i = down; i > effect + 5; i--)
	{
		if (lineinf[i].LeftPos < 3 || lineinf[i].RightPos > 156)
		{
			IsCross2 = 1;
			break;
		}
	}
	if (IsCross2 == 0)
		return 0;


	/*
	左侧的十字找线程序
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //检测十字左下角的值
	{
		if ((((lineinf[i].LeftPos - lineinf[i - 1].LeftPos) <= 0) && ((lineinf[i - 1].LeftPos - lineinf[i - 2].LeftPos) > 0)))
		{
			LCrossDown = i - 1;
			LCrossFlag = 1;
			break;
		}
	}

	if (0 == TriFind(effect, picture, 1) && LCrossFlag == 0)
	{
		if (TriFlag2 == 1)
			LCrossDown = TriPos;
		else
			LCrossDown = TriDown;  // down
	}



	/*
	右侧的十字找线程序
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //检测十字右下角的值
	{
		if ((((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0) && ((lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0)))
		{
			if ((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0 && (lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0)
			{
				RCrossDown = i - 1;
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
			RCrossDown = TriDown; // down
	}


	//===========================================================================================================================//
	if ((LCrossDown != 0 && lineinf[LCrossDown].LeftPos != left)
		|| (LCrossDown != 0 && lineinf[LCrossDown].LeftPos == left && (RightTend < 0 || LCrossDown > 50)))
	{
		for (i = LCrossDown - 3; i > effect + 3; i--)
		{
			if ((CurveSlope(i, LEFT) > SLOPELEFTDOWN && CurveSlope(i, LEFT) < SLOPELEFTUP) && (lineinf[i].LeftPos >= ReturnBigger(lineinf[LCrossDown].LeftPos - 15, left))
				&& lineinf[i].LeftPos != left && lineinf[i].LeftPos != left + 1)
			{
				LCrossUp = ReturnBigger(effect, i - 1);
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

	if ((lineinf[LCrossDown].LeftPos - lineinf[LCrossUp].LeftPos > 20) && LCrossUp != 0)
		IsDoing = 1;

	if ((LCrossDown != 0) && (LCrossUp != 0)) //左侧十字计数程序
	{
		for (i = LCrossDown; i > LCrossUp; i--)
		{
			if (lineinf[i].LeftPos <= left + 2)
				LCrossCount++;
		}
	}

	//============================================================================================================================//
	if ((RCrossDown != 0 && lineinf[RCrossDown].RightPos != right) ||
		(RCrossDown != 0 && lineinf[RCrossDown].RightPos == right && (LeftTend > 0 || RCrossDown > 50)))
	{
		for (i = RCrossDown - 3; i > effect + 3; i--)
		{
			if ((CurveSlope(i, RIGHT) > SLOPERIGHTDOWN && CurveSlope(i, RIGHT) < SLOPERIGHTUP) && (lineinf[i].RightPos <= ReturnSmaller(lineinf[RCrossDown].RightPos + 15, right))
				&& lineinf[i].RightPos != right && lineinf[i].RightPos != right - 1)
			{
				RCrossUp = ReturnBigger(effect, i - 1);
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

	if ((RCrossDown != 0) && (RCrossUp != 0)) //右侧十字计数程序
	{
		for (i = RCrossDown; i > RCrossUp; i--)
		{
			if (lineinf[i].RightPos >= right - 2)
				RCrossCount++;
		}
	}

	if ((lineinf[RCrossDown].RightPos - lineinf[RCrossUp].RightPos < -20) && RCrossUp != 0)
		IsDoing2 = 1;

	/*
	补线检测程序
	*/

	if (RCrossCount > 3 && IsDoing2 == 0)
	{
		switch (dir)
		{
		case 1:
			if (down != RCrossUp)
				PatchLineByR(down, lineinf[down].RightPos, RCrossUp, lineinf[RCrossUp].RightPos);
			IsCross = 1;
			break;
		case 2:
			break;
		default:
			break;
		}

	}
	if (LCrossCount > 3 && IsDoing == 0)
	{
		switch (dir)
		{
		case 1:
			break;
		case 2:
			if (LCrossUp != down)
				PatchLineByL(down, lineinf[down].LeftPos, LCrossUp, lineinf[LCrossUp].LeftPos);
			IsCross = 1;
			break;
		default:
			break;
		}

	}
	if (IsDoing == 1 && (RightTend < 1.1 || (LCrossDown != 0 && RCrossDown != 0) || IsDoingFlag2 == 100 || RightCrossFePos != 58))
	{

		IsCross = 1;
		if (BabyFind(effect, picture, LCrossDown, lineinf[LCrossDown].LeftPos, 1))
		{
			switch (dir)
			{
			case 1:
				if (LCrossDown != BabyDownBigger)
					PatchLineByR(down, lineinf[down].RightPos, BabyDownBigger, BabyDownPosBigger);
				if (BabyDownPosBigger == left) {}
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
				if (BabyDownBigger != down)
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
			switch (dir)
			{
			case 1:
				if (LCrossDown != BabyDownBigger)
					PatchLineByR(RCrossDown, lineinf[RCrossDown].RightPos, BabyDownBigger, BabyDownPosBigger);
				if (BabyDownPosBigger == left) {}
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
				if (BabyDownBigger != down)
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


int CrossLineNormal5(int effect, uint8(*picture)[160])
{


#define feature (3)
#define limit   (8)
#define LLLIMIT  (20)
#define RRLIMIT  (140)

	int i;

	IsCross2 = 0;
	int LCrossFlag = 0;
	int RCrossFlag = 0;
	LCrossDown = 0;
	LCrossUp = 0;
	RCrossDown = 0;
	RCrossUp = 0;
	IsDoing2 = 0;
	IsDoing = 0;
	int LEFT[60] = { left };
	int RIGHT[60] = { right };
	LeftCrossFePos = 58;   //我想要的左侧标志点
	RightCrossFePos = 58;  //我想要的右侧标志点
	int RCount2 = 0;           //右侧下几个点计数
	int LCount2 = 0;           //左侧下几个点计数
	int Count = 0;             //总的下几个点计数


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



	/*
	左侧的十字找线程序
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //检测十字左下角的值
	{
		if ((((lineinf[i].LeftPos - lineinf[i - 1].LeftPos) <= 0) && ((lineinf[i - 1].LeftPos - lineinf[i - 2].LeftPos) > 0)))
		{
			LCrossDown = i - 1;
			LCrossFlag = 1;
			break;
		}
	}

	if (0 == TriFind(effect, picture, 1) && (LCrossFlag == 0 || (LCrossFlag == 1 && LCrossDown > 30 && LCrossDown != 0 && lineinf[LCrossDown].LeftPos < 40)))
	{
		if (TriFlag2 == 1)
			LCrossDown = TriPos;
		else
			LCrossDown = down;
	}

	if ((LCrossDown != 0 && lineinf[LCrossDown].LeftPos != left)
		|| (LCrossDown != 0 && lineinf[LCrossDown].LeftPos == left && (RightTend < 0 || LCrossDown > 50)))                  //检测十字左上角的值
	{
		for (i = LCrossDown - 3; i > effect + 3; i--)
		{
			if ((CurveSlope(i, LEFT) > SLOPELEFTDOWN && CurveSlope(i, LEFT) < SLOPELEFTUP) && (lineinf[i].LeftPos >= ReturnBigger(lineinf[LCrossDown].LeftPos - 15, left))
				&& lineinf[i].LeftPos != left && lineinf[i].LeftPos != left + 1)
			{
				LCrossUp = ReturnBigger(effect, i - 1);
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

	/*
	右侧的十字找线程序
	*/
	for (i = down; i > ReturnBigger(limit, effect + 3); i--)    //检测十字右下角的值
	{
		if ((((lineinf[i].RightPos - lineinf[i - 1].RightPos) >= 0) && ((lineinf[i - 1].RightPos - lineinf[i - 2].RightPos) < 0)))
		{
			RCrossDown = i - 1;
			RCrossFlag = 1;
			break;

		}
	}
	if (0 == TriFind(effect, picture, 2) && (RCrossFlag == 0 || (RCrossFlag == 1 && RCrossDown > 30 && RCrossDown != 0 && lineinf[RCrossDown].RightPos > 120)))
	{
		if (TriFlag2 == 1)
			RCrossDown = TriPos;
		else
			RCrossDown = down;
	}

	if ((RCrossDown != 0 && lineinf[RCrossDown].RightPos != right) ||
		(RCrossDown != 0 && lineinf[RCrossDown].RightPos == right && (LeftTend > 0 || RCrossDown > 50)))                 //检测十字右上角的值
	{
		for (i = RCrossDown - 3; i > effect + 3; i--)
		{
			if ((CurveSlope(i, RIGHT) > SLOPERIGHTDOWN && CurveSlope(i, RIGHT) < SLOPERIGHTUP) && (lineinf[i].RightPos <= ReturnSmaller(lineinf[RCrossDown].RightPos + 15, right))
				&& lineinf[i].RightPos != right && lineinf[i].RightPos != right - 1)
			{
				RCrossUp = ReturnBigger(effect, i - 1);
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
		if (lineinf[i].LeftPos == left && (lineinf[down].LeftPos == left || lineinf[down - 4].LeftPos == left))
			LCount2++;
		if (lineinf[i].RightPos == right && (lineinf[down].RightPos == right || lineinf[down - 4].RightPos == right))
			RCount2++;
	}

	if (LCount2 > 1 || RCount2 > 1) {}
	else if (Count < 2)
		return 0;

	/*
	这种十字标志点寻找
	*/
	for (i = down; i > effect; i--) //赋边界值
	{
		LEFT[i] = lineinf[i].LeftPos;
		RIGHT[i] = lineinf[i].RightPos;
	}
	if (Count > 2 || LCount2 > 1)  //寻找左侧标志点
	{
		for (i = down; i > 13; i--)
		{
			if (CurveSlope(i, LEFT) > SLOPELEFTDOWN3 && CurveSlope(i, LEFT) < SLOPELEFTUP3 && lineinf[i].LeftPos != left )
			{
				LeftCrossFePos = i - 1;
				break;
			}
		}
	}
	if (Count > 2 || RCount2 > 1)  //寻找右侧标志点
	{
		for (i = down; i > 13; i--)
		{
			if (CurveSlope(i, RIGHT) > SLOPERIGHTDOWN3 && CurveSlope(i, RIGHT) < SLOPERIGHTUP3 && lineinf[i].RightPos != right )
			{
				RightCrossFePos = i - 1;
				break;
			}
		}
	}

	return 0;
}


int CrossLineFeNormal(int effect, uint8(*picture)[160])
{
	int RCount2 = 0;           //右侧下几个点计数
	int LCount2 = 0;           //左侧下几个点计数
	LeftCrossFePos = 58;   //我想要的左侧标志点
	RightCrossFePos = 58;  //我想要的右侧标志点
	int Count = 0;             //总的下几个点计数
	int LEFT[60] = { left };   //左边
	int RIGHT[60] = { right };//右边

	int i;

	/*
	检测程序
	判断是否进行这种十字处理
	*/

	if (effect > 18)
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
		if (lineinf[i].LeftPos == left && (lineinf[down].LeftPos == left || lineinf[down - 4].LeftPos == left))
			LCount2++;
		if (lineinf[i].RightPos == right && (lineinf[down].RightPos == right || lineinf[down - 4].RightPos == right))
			RCount2++;
	}

	if (LCount2 > 1 || RCount2 > 1) {}
	else if (Count < 2)
		return 0;

	/*
	这种十字标志点寻找
	*/
	for (i = down; i > effect; i--) //赋边界值
	{
		LEFT[i] = lineinf[i].LeftPos;
		RIGHT[i] = lineinf[i].RightPos;
	}
	if (Count > 2 || LCount2 > 1)  //寻找左侧标志点
	{
		for (i = down; i > 13; i--)
		{
			if (CurveSlope(i, LEFT) > SLOPELEFTDOWN3 && CurveSlope(i, LEFT) < SLOPELEFTUP3 && lineinf[i].LeftPos != left )
			{
				LeftCrossFePos = i - 1;
				break;
			}
		}
	}
	if (Count > 2 || RCount2 > 1)  //寻找右侧标志点
	{
		for (i = down; i > 13; i--)
		{
			if (CurveSlope(i, RIGHT) > SLOPERIGHTDOWN3 && CurveSlope(i, RIGHT) < SLOPERIGHTUP3 && lineinf[i].RightPos != right )
			{
				RightCrossFePos = i - 1;
				break;
			}
		}
	}
	/*
	这种十字补线
	*/
	if (picture[down][left] == 0 && picture[down][right] == 0 && RCount2 > 3 && LCount2 > 3) //两边都补线
	{
		if (down != LeftCrossFePos)
			PatchLineByL(down, lineinf[down].LeftPos, LeftCrossFePos - 1, lineinf[LeftCrossFePos - 1].LeftPos);
		if (down != RightCrossFePos)
			PatchLineByR(down, lineinf[down].RightPos, RightCrossFePos - 1, lineinf[RightCrossFePos - 1].RightPos);
		IsCross = 1;
	}
	if (LCount2 > 3 && LeftCrossFePos != 0 && LeftCrossFePos != 58)   //左侧补线
	{
		IsCross = 1;
		if (down != LeftCrossFePos)
			PatchLineByL(down, lineinf[down].LeftPos, LeftCrossFePos, lineinf[LeftCrossFePos].LeftPos);
	}
	if (RCount2 > 3 && RightCrossFePos != 0 && RightCrossFePos != 58)   //右侧补线
	{
		IsCross = 1;
		if (down != RightCrossFePos)
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

	LeftLimPos = 0;
	LeftLim = 0;
	RightLimPos = 0;
	RightLim = 0;
	int BabyDown = 0;
	int BabyDownPos = 0;
	BabyDownPosBigger = 0;
	BabyDownBigger = 0;

	if (turn == 1)//左
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
		if (LeftLim == effect)
		{
			lineinf[LeftLim].LeftPos = left;
			lineinf[LeftLim].RightPos = right;
		}
		for (i = ReturnBigger(LeftLimPos + 1, lineinf[LeftLim].LeftPos + 1); i < lineinf[LeftLim].RightPos; i++) //RightLim
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
			if (BabyDown >= BabyDownBigger)
			{
				BabyDownBigger = BabyDown;
				BabyDownPosBigger = BabyDownPos;
			}
		}

	}

	if (turn == 2)//右
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
		if (RightLim == effect)
		{
			lineinf[RightLim].LeftPos = left;
			lineinf[RightLim].RightPos = right;
		}
		for (i = ReturnSmaller(RightLimPos - 1, lineinf[RightLim].RightPos - 1); i > lineinf[RightLim].LeftPos; i--) //RightLim
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
		for (i = RightLimPos; i >= ReturnBigger(LeftLimPos, 30); i--)
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
			if (BabyDown >= BabyDownBigger)
			{
				BabyDownBigger = BabyDown;
				BabyDownPosBigger = BabyDownPos;
			}
		}



	}

	if (BabyDownBigger < 30 && BabyDownBigger != 0 && BabyDownPosBigger != 0)
	{
		if (turn == 1)
		{
			if ((BabyDownPosBigger - lineinf[LCrossDown].LeftPos <= 70 || (lineinf[LCrossDown].LeftPos - BabyDownPosBigger < 4 && lineinf[LCrossDown].LeftPos - BabyDownPosBigger > 0)) && BabyDownPosBigger != right) //36
				return 1;
			else
				return 0;
		}
		if (turn == 2)
		{
			if ((lineinf[RCrossDown].RightPos - BabyDownPosBigger <= 70 || (BabyDownPosBigger - lineinf[RCrossDown].RightPos < 4 && BabyDownPosBigger - lineinf[RCrossDown].RightPos > 0)) && BabyDownPosBigger != left) //36
				return 1;
			else
				return 0;
		}
	}
	else
		return 0;

}


/*========================================================================
*  函数名称: 环路
*  功能说明：
*  相关说明：CircleFind() 为找环路以及相关环路属性的函数
CircleHandle() 为直接补中线的程序。我不怎么喜欢
CircleHandle2() 为补边界程序
*  创建时间：2016
*  修改时间：2016
*  参数说明：
*  备注：    曲线斜率，霍夫变换
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
	int RCircleCount  = 0;
	int LCircleCount  = 0;
	int RCircleCount2 = 0;
	int LCircleCount2 = 0;
	int LCirclePos    = 0;
	int RCirclePos    = 0;
	int RLCircleFlag  = 0;
	int RCircleFlag   = 0;
	int LCircleFlag   = 0;
void CircleFind(int effect, uint8(*picture)[160])
{

#define UPDOWN          (  2)
#define CIRNOBAR        ( 20)
#define LCIRCLELIMIT    ( 40)
#define RCIRCLELIMIT    (120)
#define RLCIRCLELIMIT   ( 30)
	
	int i, j, k;

	int MiddleCircle = 0;
	int WhiteCount = 0;
	CIRCLEC = 0;
	WHITEC = 0;
	int CircleFindFlag = 0;
	dada = 0;
	IsBlackCircle = 0;
	int RightHAHA = 0;
	int LeftHAHA = 159;
	CircleLimit = 0;
	CircleD = 0;
	CircleUp = 0;
	CircleDown = 0;
	CircleLeft = 0;
	CircleRight = 0;

	/*
	初始化
	*/
	for (i = down; i >= 0; i--)
	{
		BlackCircleLeft[i] = 0;
		BlackCircleRight[i] = 0;
	}

	for (i = down; i > effect + 2; i--)
	{
		if (picture[i][(left + right) / 2 - 3] == 255 || picture[i][(left + right) / 2 + 3] == 255)
			MiddleCircle++;
	}

	/*
	检测黑色圆圈的两边
	*/
	RCircleCount  = 0;
	LCircleCount  = 0;
	RCircleCount2 = 0;
	LCircleCount2 = 0;
	LCirclePos    = 0;
	RCirclePos    = 0;
	RLCircleFlag  = 0;
	
	
	if(LCrossDown > 10 && LCrossDown < 50 && RCrossDown > 10 && RCrossDown < 50)
		RLCircleFlag = 1;
	if(RLCircleFlag == 1)
	{
		for(i = LCrossDown; i > effect+1; i--)
		{
			if(lineinf[i].LeftPos == left)
				LCircleCount++;
			if(LCircleCount != 0 && lineinf[i].LeftPos != left)
			{
				LCirclePos = ReturnBigger(i-1,effect+1);
				break;
			}
		}
		for(i = RCrossDown; i > effect+1; i--)
		{
			if(lineinf[i].RightPos == right)
				RCircleCount++;
			if(RCircleCount != 0 && lineinf[i].RightPos != right)
			{
				RCirclePos = ReturnBigger(i-1,effect+1);
				break;
			}
	
		}
		if(LCircleCount > 5)       // 右侧
			RCircleFlag = 1;
		if(RCircleCount > 5) // 左侧
			LCircleFlag = 1;
	}
	if(LCircleFlag == 1)
	{
		for(i = RCirclePos; i > effect + 1; i--)
		{
			if(lineinf[i].RightPos < LCIRCLELIMIT)
				RCircleCount2++;
		}
		if(RCircleCount2 > 2)
			LCircleFlag = 2;
	}
	if(RCircleFlag == 1)
	{
		for(i = LCirclePos; i > effect + 1; i--)
		{
			if(lineinf[i].LeftPos > RCIRCLELIMIT)
				LCircleCount2++;
		}
		if(LCircleCount2 > 2)
			RCircleFlag = 2;
	}

	
	if(LCircleFlag == 2)
	{
		for(i = RCirclePos - 1; i > effect + 1; i--)
		{
			BlackCircleLeft[i]  = lineinf[i].RightPos;
			BlackCircleRight[i] = right - 2; 
			IsBlackCircle = RCirclePos - 1 - effect;
		}
	}
	if(RCircleFlag == 2)
	{
		for(i = LCirclePos - 1; i > effect + 1; i--)
		{
			BlackCircleRight[i]  = lineinf[i].LeftPos;
			BlackCircleLeft[i] = left + 2; 
			IsBlackCircle = LCirclePos - 1 - effect;
		}
	}
	if(LCircleFlag != 2 && RCircleFlag != 2)
	{
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
	}

	/*
	确定黑色圆形的相关元素
	*/
	if (IsBlackCircle >= 3 || (StructCircle == 1 && IsBlackCircle > 1))
	{
		for (i = down; i > effect; i--)
		{
			if (CircleDown == 0 && (BlackCircleRight[i] != 0 || BlackCircleLeft[i] != 0)) //确定黑色圆形的下顶点
			{
				CircleDown = i;
				CircleFindFlag = 1;
			}
			if (BlackCircleRight[i] != 0)                                                  //确定黑色圆形的右顶点
			{
				if (BlackCircleRight[i] > RightHAHA)
				{
					RightHAHA = BlackCircleRight[i];
					CircleRight = i;
				}
			}
			if (BlackCircleLeft[i] != 0)                                                  //确定黑色圆形的左顶点
			{
				if (BlackCircleLeft[i] < LeftHAHA)
				{
					LeftHAHA = BlackCircleLeft[i];
					CircleLeft = i;
				}

			}
			if ((BlackCircleRight[i] != 0 || BlackCircleLeft[i] != 0) && CircleDown != 0 && CircleFindFlag == 1)  //确定黑色圆环的上顶点
				CircleUp = i;

			if (BlackCircleRight[i] == 0 && BlackCircleLeft[i] == 0 && CircleFindFlag == 1)
				CircleFindFlag = 0;

		}
	}

	CircleLimit = (BlackCircleRight[CircleDown] + BlackCircleLeft[CircleDown]) / 2;

	for (j = CircleDown; j >= CircleUp; j--)
	{
		for (i = BlackCircleLeft[j] + 1; i < BlackCircleRight[j]; i++)
		{

			if (picture[j][i] == 0)
			{
				WhiteCount++;
				break;
			}
		}
	}

	for (i = CircleDown; i >= CircleUp; i--)
	{
		CIRCLEC += BlackCircleRight[i] - BlackCircleLeft[i];
		WHITEC += (lineinf[i].RightPos - lineinf[i].LeftPos) - (BlackCircleRight[i] - BlackCircleLeft[i]);
	}

	int CircleFlagR = 0;
	int CircleFlagL = 0;
	if (CircleDown <= RCrossUp && RCrossUp > (effect + 8) && (WHITEC / CIRCLEC) > 0.6)
		CircleFlagR = 1;
	if (CircleDown <= LCrossUp && LCrossUp > (effect + 8) && (WHITEC / CIRCLEC) > 0.6)
		CircleFlagL = 1;

	IsBlack2 = 0;
	if (CircleDown - CircleUp > 15)
	{
		if (picture[(int)((CircleDown + CircleUp) / 2 + 2)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255 ||
			picture[(int)((CircleDown + CircleUp) / 2 + 3)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255 ||
			picture[(int)((CircleDown + CircleUp) / 2 + 4)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255 ||
			picture[(int)((CircleDown + CircleUp) / 2 + 5)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255)
			IsBlack2 = 1;
	}
	else if (CircleDown - CircleUp > 10)
	{
		if (picture[(int)((CircleDown + CircleUp) / 2 + 1)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255 ||
			picture[(int)((CircleDown + CircleUp) / 2 + 2)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255 ||
			picture[(int)((CircleDown + CircleUp) / 2 + 3)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255)
			IsBlack2 = 1;
	}
	else if (CircleDown - CircleUp > 7)
	{
		if (picture[(int)((CircleDown + CircleUp) / 2)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255 ||
			picture[(int)((CircleDown + CircleUp) / 2 + 1)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255 ||
			picture[(int)((CircleDown + CircleUp) / 2 + 2)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255)
			IsBlack2 = 1;
	}
	else if (CircleDown - CircleUp >= 2)
	{
		if (picture[(int)((CircleDown + CircleUp) / 2)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255 ||
			picture[(int)((CircleDown + CircleUp) / 2 - 1)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255 ||
			picture[(int)((CircleDown + CircleUp) / 2 + 1)][(lineinf[down].LeftPos + lineinf[down].RightPos) / 2] == 255)
			IsBlack2 = 1;
	}
	else if ((CircleDown - CircleUp > 1) && StructCircle == 1)
		IsBlack2 = 1;

	IsBlack3 = 0;
	if (((fabs(sqrt((60 - LCrossDown)*(60 - LCrossDown) + lineinf[LCrossDown].LeftPos*lineinf[LCrossDown].LeftPos) -
		sqrt((60 - RCrossDown)*(60 - RCrossDown) + (lineinf[RCrossDown].RightPos - 160)*(lineinf[RCrossDown].RightPos - 160))) > 40)
		&& LCrossDown > 25 && RCrossDown > 25) || (fabs(RCrossDown - LCrossDown) > 18 && RCrossDown != 0 && LCrossDown != 0))
		IsBlack3 = 1;

	int IsWhite = 0;
	for (i = down; i > 35; i--)
	{
		if (lineinf[i].LeftPos != left || lineinf[i].RightPos != right)
		{
			IsWhite = 1;
			break;
		}
	}
	int IsBlack4 = 0;
	if(LCrossDown != 0)
	{
		if(lineinf[LCrossDown].LeftPos > BlackCircleRight[CircleRight])
			IsBlack4 = 1;
	}
	if(RCrossDown != 0)
	{
		if(lineinf[RCrossDown].RightPos < BlackCircleLeft[CircleLeft])
			IsBlack4 = 1;
	}
	for(i = CircleDown; i > CircleUp; i--)
	{
		if((BlackCircleRight[i]+BlackCircleLeft[i])/2 < BlackCircleLeft[i-1] || (BlackCircleRight[i]+BlackCircleLeft[i])/2 > BlackCircleRight[i-1])
			IsBlack4 = 1;
	}

	if (IsBlack2 == 1 && IsBlack3 == 0 && IsBlack4 == 0 && ((CircleDown - CircleUp >= UPDOWN) || ((CircleDown - CircleUp >= 1) && StructCircle == 1)) && CircleDown >= 3 && WhiteCount < 3 && CIRCLEC != 0 && CircleFlagL == 0 && CircleFlagR == 0)
	{
		if (CircleUp != 0 && (BlackCircleRight[CircleRight] > 80) && (BlackCircleLeft[CircleLeft] < 80) && ((IsWhite == 0) || (LCrossDown != 0 && RCrossDown != 0) || (LCrossDown == 0 && RCrossDown > 33) || (RCrossDown == 0 && LCrossDown > 33))
			&& MiddleCircle > 1 && ((float)(WHITEC / CIRCLEC) < 1.8 || StructCircle == 1) )
		{
			IsCircle = 1;
			StructCircle = 1;
		}
	}
}

int CircleDealS = 0;
int HalfFlag = 0;
int CircleStatus = 0;
int StructCircleDealStart(int effect, uint8(*picture)[160])
{

	int tend = RoadDir;

	if (StructCircle == 1 && HalfFlag == 0)
	{
		switch (tend)
		{
		case 1:
			if (RightCrossFePos > 40 && RightCrossFePos < 58 && CircleDealS == 0 && (LCrossDown == 0 || LCrossDown > 30) && IsRightLeft(effect, 2, RightCrossFePos) == 0)
			{
				HalfFlag = 1;
				CircleDealS = RightCrossFePos;
			}
			break;
		case 2:
			if (LeftCrossFePos > 40 && LeftCrossFePos < 58 && CircleDealS == 0 && (RCrossDown == 0 || RCrossDown > 30) && IsRightLeft(effect, 1, LeftCrossFePos) == 0)
			{
				HalfFlag = 1;
				CircleDealS = LeftCrossFePos;
			}
			break;
		default:
			break;
		}
	}

	if (HalfFlag == 1)
	{
		switch (tend)
		{
		case 1:

			if ((CircleDealS > RCrossUp && RCrossUp > (effect + 8)) || (((CircleDealS > RCrossUp && RCrossUp == (effect + 8)) || RightCrossFePos < 35) && LCrossDown > 18 && LCrossDown < 45 && (RCrossDown > 25 || RightCrossFePos < 35)))
			{
				if ((LCrossDown > 13 && LCrossDown < 55 && lineinf[LCrossDown].LeftPos >lineinf[down].LeftPos) ||  RightCrossFePos < 35)
					CircleStatus = 1;
			}

			break;
		case 2:

			if ((CircleDealS > LCrossUp && LCrossUp > (effect + 8)) || (((CircleDealS > LCrossUp && LCrossUp == (effect + 8)) || LeftCrossFePos < 35) && RCrossDown > 18 && RCrossDown < 45 && (LCrossDown > 25 || LeftCrossFePos < 35)))
			{
				if ((RCrossDown > 13 && RCrossDown < 55 && lineinf[RCrossDown].RightPos < lineinf[down].RightPos) ||  LeftCrossFePos < 35)
					CircleStatus = 1;
			}


			break;
		default:
			break;

		}
	}

	if (CircleStatus == 100)
	{
		CircleStatus = 0;
		CircleDealS = 0;
		StructCircle = 0;
		HalfFlag = 0;
	}

	return 0;
}

int LeftDown = 0;
int LeftDownPos = 0;
int RightDown = 0;
int RightDownPos = 0;
int LeftUp = 0;
int LeftUpPos = 0;
int RightUp = 0;
int RightUpPos = 0;

void CircleHandle2(int effect, uint8(*picture)[160], int dir)
{
#define RightCircle  (1)
#define LeftCircle   (2)
#define CircleLimit  (10)

	int i;

	LeftDown = 0;
	LeftDownPos = 0;
	RightDown = 0;
	RightDownPos = 0;
	LeftUp = 0;
	LeftUpPos = 0;
	RightUp = 0;
	RightUpPos = 0;

	switch (dir)
	{

	case 1:
		/*
		左侧程序
		*/

		/********************                 右侧         *******************/
		for (i = down; i > CircleLimit; i--)      //检测右下角的值
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

		/*补线*/
		if (RightDown > CircleDown - 1)
			PatchLineByR(RightDown, RightDownPos, CircleDown - 1, BlackCircleLeft[CircleDown - 1]);
		else if (CircleDown != 0 && (CircleDown - 1) != down)
			PatchLineByR(down, right, CircleDown - 1, BlackCircleLeft[CircleDown - 1]);


		for (i = CircleDown - 1; i >= CircleUp; i--)
			lineinf[i].RightPos = BlackCircleLeft[i];

		break;


	case 2:
		/*
		右侧程序
		*/
		/********************                 左侧         *******************/
		for (i = down; i > CircleLimit; i--)      //检测左下角的值
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


		/*补线*/
		if (LeftDown > CircleDown - 1)
			PatchLineByL(LeftDown, LeftDownPos, CircleDown - 1, BlackCircleRight[CircleDown - 1]);
		else if (CircleDown != 0 && (CircleDown - 1) != down)
			PatchLineByL(down, left, CircleDown, BlackCircleRight[CircleDown - 1]);

		for (i = CircleDown - 1; i >= CircleUp; i--)
			lineinf[i].LeftPos = BlackCircleRight[i];

		break;
	default:
		break;


	}
}



/*========================================================================
*  函数名称: ZebarLine
*  功能说明：斑马线
*  创建时间：2016
*  修改时间：2016
*  参数说明：
========================================================================*/
int ZECOUNT = 0;

int ZebarLine(int effect, uint8(*picture)[160])
{
	int i;
	int j;
	int ZebarCount = 0;
	ZECOUNT = 0;

	for (i = down; i > effect + 10; i--) // effect + 10
	{
		if (lineinf[i].LeftPos == right || lineinf[i].RightPos == left || lineinf[i].LeftPos >= lineinf[i].RightPos
			|| lineinf[i].LeftPos == left || lineinf[i].RightPos == right) {
		}
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
	if (IsZebarLine == 0)
		return 0;

	int distance = 0;
	int ZebarLine[60] = { 0 };

	for (int i = down; i > effect + 3; i--)
		ZebarLine[i] = CountLine2(i, picture);

	for (int i = down; i > effect + 5; i--)
	{
		if (ZebarLine[i] > 5 && ZebarLine[i - 1] > 5)
		{
			distance = down - i;
			break;
		}
	}

	if (distance != 0)
		return distance;
	else
		return 0;

}

int ZebarLineDistance2(int effect, uint8(*picture)[160])
{

	int distance = 0;
	int ZebarCount2 = 0;

	for (int i = down; i > effect + 7; i--) // effect + 10
	{
		ZebarCount2 = 0;
		for (int j = lineinf[i].LeftPos + 1; j < lineinf[i].RightPos - 1; j++)
		{
			if (picture[i][j] != picture[i][j + 1])
				ZebarCount2++;
		}
		if ((ZebarCount2 / 2) > 4)
		{
			distance = down - i;
			break;
		}
	}

	if (distance != 0)
		return distance;
	else
		return 0;

}

/*========================================================================
*  函数名称: ObstacleFind
*  功能说明：障碍程序
*  创建时间：2016
*  修改时间：2016
*  参数说明：
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
	int ObstacleLeft[60] = { 0 };
	int ObstacleRight[60] = { 0 };
	IsObstacleLeft = 0;
	IsObstacleRight = 0;
	NotObstacle = 0;
	int LeftOb = 0;
	int RightOb = 0;

	if (effect > 7)
		return 0;

	for (i = down; i > effect; i--)
	{
		if (lineinf[i].LeftPos != left)
		{
			LeftOb = i;
			break;
		}

	}
	for (i = down; i > effect; i--)
	{
		if (lineinf[i].RightPos != right)
		{
			RightOb = i;
			break;
		}

	}
	for (i = LeftOb; i > effect + 3; i--)
	{
		if (lineinf[i].LeftPos == left)
		{
			NotObstacle = 1;
			break;
		}
	}
	for (i = RightOb; i > effect + 3; i--)
	{
		if (lineinf[i].RightPos == right)
		{
			NotObstacle = 1;
			break;
		}
	}

	for (i = down; i > effect + 10; i--)
	{
		if (lineinf[i].LeftPos - lineinf[i - 1].LeftPos > 20)
		{
			wrong = 1;
			break;
		}
		if (lineinf[i - 1].RightPos - lineinf[i].RightPos > 20)
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
						if (ObstacleLeft[i] != 0)
						{
							if ((float)(ObstacleRight[i] / ObstacleLeft[i]) > 3 && ObstacleLeft[i] < 13 && ((ObstacleRight[i] + ObstacleLeft[i]) > (RIGHTOBS - LEFTOBS)))
							{
								IsObstacleLeft++;
								if (picture[i][80] == 255)
									OBBlack++;
								if (picture[i][80] == 0 && FirstOb == 0)
									FirstOb = i;
							}
							if ((float)(ObstacleRight[i] / ObstacleLeft[i]) < 0.3333333 && ObstacleRight[i] < 13 && ((ObstacleRight[i] + ObstacleLeft[i]) > (RIGHTOBS - LEFTOBS)))
							{
								IsObstacleRight++;
								if (picture[i][80] == 255)
									OBBlack++;
								if (picture[i][80] == 0 && FirstOb == 0)
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

	if (IsObstacleLeft > 2 && NotObstacle == 0 && FirstOb > 8 && wrong == 0) // OBBlack
		return 2;
	if (IsObstacleRight > 2 && NotObstacle == 0 && FirstOb > 8 && wrong == 0)
		return 1;

	return 0;
}
int IsMiddleJump(int effect, uint8(*picture)[160])
{
	int Is = 0;
	for (int i = down; i > effect + 20; i--)
	{
		if (fabs(lineinf[i].LeftPos - lineinf[i - 5].LeftPos) > 15 || fabs(lineinf[i].RightPos - lineinf[i - 5].RightPos) > 15)
		{
			Is = 1;
			break;
		}
	}
	if (Is)
		return 1;
	else
		return 0;
}
/*========================================================================
*  函数名称: ExceedOnStraightLineFind
*  功能说明：直到超车程序
*  创建时间：2016
*  修改时间：2016
*  参数说明：
========================================================================*/
int StructExceed = 0;
int Exceed = 0;      //超车补线点
int ExceedPos = 0;   //超车补线值
int ExceedOn = 0;
int ExceedOnPos = 0;
int CountNum[60] = { 0 };

int DistanceDig(int effect, uint8(*picture)[160])
{
	int count[60] = { 0 };
	int distance = 59;
	int i = 0;

	for (i = down; i > effect; i--)
		count[i] = CountLine(i, picture);

	for (i = down; i > effect + 7; i--)
	{
		if ((count[i] - count[i - 2]) > 15 && (count[i] - count[i - 3]) > 15)
		{
			distance = i - 2;
			break;
		}
	}

	if (distance != 59)
		return distance;
	else
		return 59;
}

int StraightLineFind(int effect, uint8(*picture)[160], int distance)//2
{

	int i = 0;

	int LStraight = 0;
	int RStraight = 0;
	int StraightZero = 0;

	for (i = down; i > distance; i--)
	{
		if (lineinf[i].LeftPos == left || lineinf[i].RightPos == right)
		{
			StraightZero = 1;
			break;
		}
	}
	for (i = down; i > distance; i--)
	{
		if (lineinf[i].LeftPos <= lineinf[i - 1].LeftPos && lineinf[i - 1].LeftPos - lineinf[i].LeftPos < 4)
			LStraight++;
		if (lineinf[i].RightPos >= lineinf[i - 1].RightPos && lineinf[i].RightPos - lineinf[i - 1].RightPos < 4)
			RStraight++;
	}

	if (LStraight >(down - distance - 1) && RStraight >(down - distance - 1) && effect < (distance + 3)
		&& StraightZero == 0)
		return 1;
	else
		return 0;

}


int StraightLinePatch(int effect, uint8(*picture)[160], int Dir)
{
	int i = 0;

	switch (Dir)
	{
	case 1:
		for (i = down; i > effect; i--)
			lineinf[i].MiddlePos = lineinf[i].LeftPos + StraightLine[down - i];
		break;
	case 2:
		for (i = down; i > effect; i--)
			lineinf[i].MiddlePos = lineinf[i].RightPos - StraightLine[down - i];
		break;
	default:
		break;
	}

	return 0;
}

int StraightLinePatch2(int effect, uint8(*picture)[160], int Dir)
{
	int i = 0;

	switch (Dir)
	{
	case 1:
		for (i = down; i > effect; i--)
			lineinf[i].MiddlePos = lineinf[i].LeftPos + StraightLine2[down - i];
		break;
	case 2:
		for (i = down; i > effect; i--)
			lineinf[i].MiddlePos = lineinf[i].RightPos - StraightLine2[down - i];
		break;
	default:
		break;
	}

	return 0;
}

/*========================================================================
*  函数名称: ramp
*  功能说明：坡道
*  创建时间：2017
*  修改时间：2017
*  参数说明：
========================================================================*/
int IsRamp = 0;
int StructRamp = 0;
int RampFind(int effect, uint8(*picture)[160])
{

#define RAMPLIMMIT   35
	int temp1 = 0;
	int temp2 = 0;
	int temp3 = 0;
	if (effect > 2)
		return 0;
	if (StraightLineFind(effect, picture, 1) == 0 && StructRamp == 0)
		return 0;

	temp1 = lineinf[1].RightPos - lineinf[1].LeftPos;
	temp2 = lineinf[2].RightPos - lineinf[2].LeftPos;
	temp3 = lineinf[3].RightPos - lineinf[3].LeftPos;

	if (temp1 > RAMPLIMMIT && temp2 > RAMPLIMMIT && temp3 > RAMPLIMMIT)
		return 1;
	else
		return 0;
}

/*========================================================================
*  函数名称: GiveMiddle
*  功能说明：中线赋值程序
*  创建时间：2016
*  修改时间：2016
*  参数说明：
========================================================================*/
void  GiveMiddle(int effect, uint8(*picture)[160])
{
	int i;
	int k = 0;


	for (i = down; i > effect; i--)
		lineinf[i].MiddlePos = (lineinf[i].LeftPos + lineinf[i].RightPos) / 2;

	k = effect + 2;

	if (effect >= 0 && effect < 55)        //判断中线大致趋势，中线归一化
	{
		if (lineinf[down].MiddlePos<lineinf[effect + 2].MiddlePos && k != up)
			PatchLineByM(k, lineinf[k].MiddlePos, up, right);
		else if (lineinf[down].MiddlePos>lineinf[effect + 2].MiddlePos && k != up)
			PatchLineByM(k, lineinf[k].MiddlePos, up, left);
		else if (k != 0)
			PatchLineByM(k, lineinf[k].MiddlePos, 0, 80);
		else {}

		if (StructRamp != 0)
			PatchLineByM(k, lineinf[k].MiddlePos, 0, 80);
	}
}

/*========================================================================
*  函数名称: show
*  功能说明：EASY_SHOW程序
*  创建时间：2016
*  修改时间：2016
*  参数说明：
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

	for (i = effect + 1; i > 0; i--)
	{
		picture[i][lineinf[i].MiddlePos] = 0;
	}
	//	
	//	for(i = down; i > 0 ; i--)
	//	{
	//		picture[i][Ldem[i]] = 255;
	//		picture[i][Rdem[i]] = 255;
	//	}
}


/*========================================================================
*  函数名称: RoadInit
*  功能说明：init
*  创建时间：2016
*  修改时间：2016
*  参数说明：
========================================================================*/
#define STRAIGHTSPEED          (1)  //长直道速度标志位
#define SHORTSTRAIGHTSPEED     (2)  //短直道速度标志位
#define SLINESPEED             (3)  //小s速度标志位
#define NORMALSPEED            (4)  //弯道标志位
#define RAMPSPEED              (5)  //坡道标志位

int EFFECT = 0;                //有效行
int IsStraightLine = 0;        //长直道检测标志位
int IsZebarLine = 0;           //斑马线检测标志位
int StructZebarline = 0;       //斑马线检测全局标志位
int ZebarDistance = 0;		   //斑马线距离
int IsObstacle = 0;			   //障碍检测标志位
int CanObstacle = 0;           //障碍中间状态判定
int StrucIsObstacle = 0;	   //障碍检测全局标志位
int IsJumpOb = 0;              //障碍跳变检测
int StrCircleCount = 0;		   //环路计数
int StructCircleLast = 0;      //上次环路状态
int SpeedUp = 0;               //变速标志位
int StructSpeedUp = 0;         //变速全局标志位
float SpeedTend = 0;           //变速中线趋势
int RampLimit = 0;             //坡道限制
int SPEEDLEVEL = 0;            //速度档
void RoadInit(uint8(*picture)[160])
{
	/*有效行初始化*/
	EFFECT = 0;

	/*环路方向选定*/
	StructCircleFlag = 0;
	if (StructCircle != StructCircleLast)
	{
		StrCircleCount++;
		StructCircleLast = StructCircle;
	}
	switch (StrCircleCount)
	{
		case 1:
			RoadDir = 1;
			break;
		case 2:
			RoadDir = 1;
			break;
		case 3:
			RoadDir = 2;
			break;
		case 4:
			RoadDir = 2;
			break;
		case 5:
			RoadDir = 2;
			break;
		case 0:
			RoadDir = 2;
			break;
		default:
			break;
	}
	if (StrCircleCount == 6)
		StrCircleCount = 0;

	/**********停车程序**********************/
	if (IsZebarLine == 0)
		Safe_Mode(picture);
	/*********初始化*************************/
	StructInit();
	FindFirstPos(picture);
	FindOtherPos(0, picture);
	EFFECT = EffectiveGet2(picture);
	FindFirstPos(picture);
	FindOtherPos(EFFECT, picture);
	for (int i = down; i > EFFECT; i--)
	{
		TrueRight[i] = lineinf[i].RightPos;
		TrueLeft[i] = lineinf[i].LeftPos;
	}
	/**********起跑线******************************/
	if (IsZebarLine == 0)
		IsZebarLine = ZebarLine(EFFECT, picture);
	if (IsZebarLine != 0)
		ZebarDistance = ZebarLineDistance2(EFFECT, picture);
	/*************坡道**************************/
	IsRamp = RampFind(EFFECT, picture);
	if (StructRamp == 0 && IsRamp == 1 && RampLimit == 0) //识别坡道
		StructRamp = 1;
	if (StructRamp == 1 && IsRamp == 0) 				//上坡
		StructRamp = 2;
	if (StructRamp == 2 && IsRamp == 1) 				//下坡
	{
		RampLimit = 1;
		StructRamp = 3;
	}
	if (StructRamp == 3)  								//清标志位
		StructRamp = 0;
	if (fabs((lineinf[EFFECT + 5].LeftPos + lineinf[EFFECT + 5].RightPos) / 2 - 80) > 15 &&  //以防标志位没清，强制清标志位 
		fabs((lineinf[EFFECT + 10].LeftPos + lineinf[EFFECT + 10].RightPos) / 2 - 80) > 15)
		StructRamp = 0;
	/**********障碍********************************/
	if (StructCircle == 0)
		IsObstacle = ObstacleFind(EFFECT, picture);
	if (IsObstacle != 0 && StructCircle == 0)
	{
		if (IsObstacle == 1)
			StrucIsObstacle = 1;
		else if (IsObstacle == 2)
			StrucIsObstacle = 2;
	}
	if (StrucIsObstacle != 0)
	{
		IsJumpOb = IsMiddleJump(EFFECT, picture);
		StraightLinePatch2(EFFECT, picture, StrucIsObstacle);
	}
	if (IsObstacle == 0 && StrucIsObstacle != 0 && IsJumpOb == 1)
		CanObstacle = 1;

	if (CanObstacle == 1 && IsJumpOb == 0)
	{
		CanObstacle = 0;
		StrucIsObstacle = 0;
	}

	/*****环路标志位识别**************************/
	if (IsZebarLine == 0 && StrucIsObstacle == 0)
	{
		CrossLineNormal5(EFFECT, picture);
		//if(HalfFlag == 0)	
			CircleFind(EFFECT, picture);
		StructCircleDealStart(EFFECT, picture);
	}

	/****其余补线*******************************/
	if (IsZebarLine == 0)
	{
		if (IsCircle == 1)
			CircleHandle2(EFFECT, picture, RoadDir);

		if (IsCircle == 0)
			CrossLineFeNormal(EFFECT, picture);

		if (StructCircle == 0 && IsCircle == 0)
			CrossLineNormal3(EFFECT, picture);

		if (StructCircle == 1 && IsCircle == 0)
		{
			if (RoadDir == 2)
				CrossLineNormal4(EFFECT, picture, 2);
			else if (RoadDir == 1)
				CrossLineNormal4(EFFECT, picture, 1);
		}

	}
	/*****补中线************************************/
	if (StrucIsObstacle == 0)
		GiveMiddle(EFFECT, picture);
	/****小s处理**********************************/
		Cross_S_Find2(EFFECT, picture);
		if (IsSLine)
		{
			Cross_S_Line5(EFFECT, picture);
			diff_cancel = 1;
		}
		else
			diff_cancel = 0;

	//=========加减速=================================================// 
			/*长直道提速*/
			if (StructRamp == 0)
				IsStraightLine = StraightLineFind(EFFECT, picture, 4);

			/*基础小直道提速*/
			SpeedTend = ReturnTendency(picture, EFFECT, 3, 15, down);
			if (StructSpeedUp == 0)
				StructSpeedUp = StraightLineFind(EFFECT, picture, 12);
			if (SpeedTend != 0 && StructSpeedUp == 1 &&
				((fabs(SpeedTend) > 0.3 && EFFECT < 4) || (EFFECT >= 4 && fabs(SpeedTend) > 0.5) || EFFECT >= 8))
				StructSpeedUp = 0;

	/*=======速度分级=====================================*/
			if (IsStraightLine)
				SPEEDLEVEL = STRAIGHTSPEED;
			else if (StructCircle == 0 && StructSpeedUp == 1 && StrucIsObstacle == 0 && StructRamp == 0 && IsSLine == 0)
				SPEEDLEVEL = SHORTSTRAIGHTSPEED;
			else if (IsSLine)
				SPEEDLEVEL = SLINESPEED;
			else
				SPEEDLEVEL = NORMALSPEED;
			if (StructRamp != 0)
				SPEEDLEVEL = RAMPSPEED;

	/*=======速度调节=====================================*/
			switch(SPEEDLEVEL)
			{
				case STRAIGHTSPEED:
					right_set = 85;
					left_set = 85;
					break;
				case SHORTSTRAIGHTSPEED:
					right_set = 78;
					left_set = 78;
					break;
				case SLINESPEED:
					right_set = 71;
					left_set = 71;
					break;
				case NORMALSPEED:
					right_set = 60;
					left_set = 60;
					break;
				case RAMPSPEED:
					right_set = 45;
					left_set = 45;
					break;
				default:
					right_set = 60;
					left_set = 60;
					break;
					
			}
	
	/*===================================================*/
	//=========加减速=================================================// 	

	/*蜂鸣器*/
	if (IsSLine)
		BuzzerOn();
	else
		BuzzerOff();
}

/*
纯属娱乐
*/

/*========================================================================
*  函数名称: LedSeeImage
*  功能说明：OLED显示图像函数
*  创建时间：2017
*  修改时间：2017
*  参数说明：
========================================================================*/
void LedSeeImage(uint8(*picture)[160])
{
	int i = 0;
	int j = 0;

	int temp = 0;


	for (i = 0; i < 8; i++)
	{
		LCD_Set_Pos(0, i);

		if (i < 7)
		{
			for (j = 20; j < 140; j++)  //20
			{
				temp = 0;
				if (!picture[i * 8 + 0][j])     temp |= 0x01;  //第 1 行图像
				if (!picture[i * 8 + 1][j])     temp |= 0x02;  //第 2 行图像
				if (!picture[i * 8 + 2][j])     temp |= 0x04;  //第 3 行图像
				if (!picture[i * 8 + 3][j])     temp |= 0x08;  //第 4 行图像
				if (!picture[i * 8 + 4][j])     temp |= 0x10;  //第 5 行图像
				if (!picture[i * 8 + 5][j])     temp |= 0x20;  //第 6 行图像
				if (!picture[i * 8 + 6][j])     temp |= 0x40;  //第 7 行图像
				if (!picture[i * 8 + 7][j])     temp |= 0x80;  //第 8 行图像
				LCD_WrDat(temp);
			}
		}
		if (i == 7)

		{
			for (j = 20; j < 140; j++)
			{
				temp = 0;
				if (!picture[i * 8 + 0][j])     temp |= 0x01;  //第 57 行图像
				if (!picture[i * 8 + 1][j])     temp |= 0x02;  //第 58 行图像
				if (!picture[i * 8 + 2][j])     temp |= 0x04;  //第 59 行图像
				if (!picture[i * 8 + 3][j])     temp |= 0x08;  //第 60 行图像
				LCD_WrDat(temp);
			}
		}
	}

}