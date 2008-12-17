#include "TetrisAIAPI.h"
#include "TetrisAI.h"
#include "Block.h"

#define REACHABLE 45
//�㷨���أ����ǵ���&���Ƕಽ
 #define _CONSIDER_DOUBLE


// �����Ҫ��ʼ�������ڴ����
void Init()
{

}

void CopyMatrix(int srcMatrix[18][10], int dstMatrix[18][10])
{
    for (int i=0; i<18; i++)
        for( int j=0; j<10; j++)
        {
            dstMatrix[i][j] = srcMatrix[i][j];
        }
}

int GetMatrixHeight(int matrix_arr[18][10])
{
    //matrix_arr�У�y���������Ļ�Ϸ����·����εݼ�
    for (int i=0; i<18; i++)
        for (int j=0; j<10; j++)
        {
            //�ҵ���һ����Ϊ��Ĳ��֣���Ϊ����߶�
            if ( matrix_arr[i][j] )
            {
                return i;
            }
        }
    return 0;
}

int GetACost(int matrix_arr[18][10])
{

    const int HEIGHTCOST = 7;   //�߶ȼ�Ȩ
    const int HOLECOST = 2;      //�ն���Ȩ
    int countHole = 0;
    int cost = 0;
    int heigth = GetMatrixHeight(matrix_arr);
    cost += (18-heigth) * HEIGHTCOST;
    for (int i=heigth; i<18; i++)
    {
        countHole=0;
        for (int j=0; j<10; j++)
        {
            if ( 0 == matrix_arr[i][j] )
            {
                cost += HOLECOST*(i-heigth+1)*(i-heigth+1)*(i-heigth+1);
                countHole++;
                if ( i-1>=0 && 0 != matrix_arr[i-1][j])
                {
                    cost += HOLECOST*200;
                }
            }
        }
        if ( 0 == countHole )
        {
            cost -= 100000;
        }
    }

    int matrix_old[18][10];
    CopyMatrix(matrix_arr,matrix_old);

    //�ն����
    for (int i=0; i<10; i++)
    {
        matrix_old[0][i] = REACHABLE;
    }
    for (int i=1; i<18; i++)
        for (int j=0; j<10; j++)
        {
            if (REACHABLE == matrix_old[i-1][j] && 0 == matrix_old[i][j])
            {
                matrix_old[i][j] = REACHABLE;                
            }
        }
    for (int i=1; i<18; i++)
        for (int j=0; j<10; j++)
        {
            if ( 0 == matrix_old[i][j] )
            {
                cost += HOLECOST*10;
            }
        }
    return cost;
}
int FindBestMatrix(int matrix_arr[18][10],bool old_block[4][4],int &cost)
{
    int BeginX = GetBlockLeftX(old_block);
    int EndX = GetBlockRightX(old_block);
    int BeginY = GetBlockTopY(old_block);
    int EndY = GetBlockBottomY(old_block);
    int BlockWidth = EndX - BeginX;
    int BlockHeight = EndY - BeginY;
    int MatrixHeight = GetMatrixHeight(matrix_arr)-1;
    bool FlagArrangeOk = true;   //�Ƿ���Խ�����ڷŵ���ǰλ�õı��
    int min_cost = 1000000; //���������ֵ
    int bestMove = 0;
    int matrix_old[18][10];
    int matrix_best[18][10];
    //��������ÿһ�εı��������������ѡ��õĲ���
    for (int sY=MatrixHeight-3+BeginY; sY<=17-EndY; sY++)
    {
        if (sY < 0)
        {
            break;
        }
        for(int sX=0;sX<=9-BlockWidth;sX++)
        {
            CopyMatrix(matrix_arr,matrix_old);
            FlagArrangeOk = true;
            for (int iY=0; iY<=EndY; iY++)
                for (int iX=0; iX<=BlockWidth; iX++)
                {
                    //���block�İڷ�λ���г�����matrix�غϵĵ㣬���ж�Ϊ�ڷ�λ�ò��ϸ�
                    if (matrix_old[sY+iY][sX+iX]&&old_block[iY][iX+BeginX])
                    {
                        FlagArrangeOk = false;
                    }
                    
                    //�ж�block�з�����Ϸ��Ƿ�ᱻmatrix�з���㸲��
                    if (
                        ( 0== iY && old_block[iY][iX+BeginX])||
                        (old_block[iY][iX+BeginX] && (!old_block[iY-1][iX+BeginX]))
                       )
                    {
                        for (int tempH=0; tempH<=sY+iY-1; tempH++)
                        {
                            if (matrix_old[tempH][sX+iX])
                            {
                                FlagArrangeOk = false;
                            }
                        }
                    }


                    matrix_old[sY+iY][sX+iX] = matrix_old[sY+iY][sX+iX]||old_block[iY][iX+BeginX];
                }


                if (FlagArrangeOk)
                {
                    cost = GetACost(matrix_old);
                //��¼Ŀǰ���ž���
                if (cost < min_cost)
                {
                    min_cost = cost;
                    bestMove = 3+BeginX - sX;
                    CopyMatrix(matrix_old,matrix_best);
                }
                }
                else
                {
                    cost = 1000000;
                }

        }
    }
        cost = min_cost;
        CopyMatrix(matrix_best,matrix_arr);
        return bestMove;
}



extern "C" TETRISAI_API MoveState MoveStrategy(bool cur_block[4][4], bool next_block[4][4],int matrix_arr[18][10])
{
    MoveState movement;
    movement.move = 0;
    movement.rotate = 0;
    int Move;
    int Move2;
    int minmumCost = 10000000;
    int matrix_old[18][10];
    int cost = 0;
    int cost2 = 0;
    bool old_block[4][4];
    bool old2_block[4][4];
    //���Ƶ�ǰ4*4����
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
        {
            old_block[i][j] = cur_block[i][j];
            old2_block[i][j] = next_block[i][j];
        }

        //ֻ���ǵ�ǰ����
#ifndef _CONSIDER_DOUBLE 
        for (int i=0; i<4; i++)
        {
            //���Ƶ�ǰ18*10�ľ���
            CopyMatrix(matrix_arr,matrix_old); 

            //ÿ����ת��ʮ�Ⱥ��������ȷ�����Ž�
            Move = FindBestMatrix(matrix_old,old_block,cost);    
            if (cost < minmumCost)
            {
                movement.move = Move;
                movement.rotate = i;
                minmumCost = cost;
            }
            //������һ���任����ĳ���
            RotateBlock(old_block,1);
        }

#endif
        //���ǵ�ǰ�Լ���һ������
#ifdef _CONSIDER_DOUBLE
        for (int i=0; i<4; i++)
        {
            for (int j=0; j<4; j++)
            {
                //���Ƶ�ǰ18*10�ľ���
                CopyMatrix(matrix_arr,matrix_old); 

                //ÿ����ת��ʮ�Ⱥ��������ȷ�����Ž�
                Move = FindBestMatrix(matrix_old,old_block,cost);  
                Move2 = FindBestMatrix(matrix_old,old2_block,cost2);
                if (cost+cost2 < minmumCost)
                {
                    movement.move = Move;
                    movement.rotate = i;
                    minmumCost = cost+cost2;
                }
                //������һ���任����ĳ���
                RotateBlock(old2_block,1);
            }
            RotateBlock(old_block,1);
        }
#endif

        return movement;

}

