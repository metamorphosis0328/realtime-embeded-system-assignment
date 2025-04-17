#include<iostream>

//who's stone
int empty=0;
int player=1;
int computer=2;
//state of each place
int board[13][13]={0};
int x=0;
int y=0;

//int xComputer=0;
//int yComputer=0;
//int max1=0;
//int max2=0;

int turn=player;

int dx[8]={-1,-1,-1,0,0,1,1,1};
int dy[8]={-1,0,1,-1,1,-1,0,1};
//make each place empty
void clearBoard()
{
    int i,j;
    for(i=0;i<13;i++)
    {
        for(j=0;j<13;j++)
        {board[i][j]=empty;}
    }
}
//check if stone placed on board, check if the position is taken
int checkPosition(int x,int y)
{
    if(x>=0&&x<=13&&y>=0&&y<=13&&board[x][y]==empty)
    {return 1;}
    else
    {return 0;}
}
//human's turn
void placeStone()
{
    int i=0;
    do
    {
        std::cin>>x>>y;
        if (checkPosition(x,y))
        {
            i=1;
            //std::cout<<"good"<<std::endl;
        }
        else 
        {
            i=0;
            //std::cout<<"error"<<std::endl;
        }   
    } while (i==0);
    board[x][y]=player;
}
//check if anyone wins
int judge(int x,int y,int currentPlayer)
{
    int i=0;
    int count=1;
    int nx,ny;
    for(i=0;i<8;i++)
    {
        nx=x+dx[i];
        ny=y+dy[i];
        while(nx>=0&&nx<=13&&ny>=0&&ny<=13&&board[nx][ny]==currentPlayer)
        {
            count++;
            nx=nx+dx[i];
            ny=ny+dy[i];
        }
        if(count>=5)
        {return 1;}
    }
    return 0;
}

/*void computerPlay()
{
    int score=0;
    int count=0;
    int i,j,n,k;
    int nx,ny;
    for(i=0;i<13;i++)
    {
        for(j=0;j<13;j++)
        {
            if(board[i][j]==empty)
            {
                board[i][j]=computer;
                if(judge(i,j,computer))
                {
                    xComputer=i;
                    yComputer=j;
                    break;
                }
                else
                {
                    for(n=0;n<8;n++)
                    {
                        nx=i+dx[n];
                        ny=j+dy[n];
                        for(k=2;k>0;k--)
                        {
                         while(nx>=0&&nx<=13&&ny>=0&&ny<=13&&board[nx][ny]==k)
                         {
                            count++;
                            nx=nx+dx[n];
                            ny=ny+dy[n];
                         }
                         if(count>=5)
                         {score=100;}
                         else if(count==4)
                             {score=50;}
                             else if(count==3)
                                  {score=30;}
                                  else if(count==2)
                                       {score=10;}
                                       else if(count==1)
                                            {score=0;}
                         if(max1<=score)
                         {max1=score;}
                        }
                    }
                }
                if(max1>=max2)
                {
                    max2=max1;
                    xComputer=i;
                    yComputer=j;
                }
                board[i][j]=empty;
            }
        }
    }
    
}*/

int main()
{
    clearBoard();
    while(1)
    {
        if (turn==player)
        {
            placeStone();
            if (judge(x,y,player))
            {
                std::cout<<"player win"<<std::endl;
                break;
            }
            turn=computer;
            
        }
        else
        {
           //放下棋算法
            //computerPlay();
           //board[xComputer][yComputer]=computer;
            turn=player;
        }
        
    }
    return 0;
}