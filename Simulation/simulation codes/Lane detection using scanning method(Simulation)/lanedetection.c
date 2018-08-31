#include "eyebot.h"
#define SIZE QVGA_SIZE
#define PIX  (SIZE/3)
#define predepth  80
#define f 2
#define l 1
int L;
int F;
int startpoint=0,endpoint=0,distance=0,leftsmall=0, leftbig=0, depth=0,length=0;
QVGAgray image, left, right, mid, imageout;//gray scale
void GetLeft(int L)
{ 
    if(PSDGet(l) - L > predepth)
    {
      ENCODERReset(1);
      LCDSetPrintf(3,1,"I come to the start!;startpoint = %d",PSDGet(f));
      leftbig = PSDGet(l);// the left value now substract the history value
      leftsmall = L;
      depth = leftbig - leftsmall;
      LCDSetPrintf(4,1,"leftsmall=%4d\n", leftsmall);
      LCDSetPrintf(5,1,"leftbig=%4d\n", leftbig);
      LCDSetPrintf(6,1,"depth=%4d\n",depth);
    }
}
void Park2(int length, int leftbig)
{
  VWSetSpeed(-150,0);
  OSWait(length-1267);
  VWSetSpeed(0,-60);
  OSWait(1900);
  VWSetSpeed(-100,0);
  OSWait(leftbig*10-1000);
  VWSetSpeed(0,60);
  OSWait(1900);
  VWSetSpeed(0,0);
}
void MarkLeftLine(QVGAgray image, QVGAgray left)
{
  int row;
  int column;
  int middle = 160;
  // int tempmiddle = 160;
  for(row = 239;row>80;row--) //bottom 2/3
  {
  	for(column = middle;column>0;column--) //from middle to left side
  	{
  		if(image[row*320+column]==0)
  		{
  			left[row*320+column]=1; //find the first point of this row
  			//LCDPixel(column,row,0xFF0000);
  			break;
  		}
  	}
  }
}
void MarkRightLine(QVGAgray image, QVGAgray left)
{
  int row;
  int column;
  int middle = 160;
  // int tempmiddle = 160;
  for(row = 239;row>80;row--)
  {
  	for(column = middle;column<320;column++)
  	{
  		if(image[row*320+column]==0)
  		{
  			right[row*320+column]=1;
  			//LCDPixel(column,row,0xFF0000);
  			break;
  		}
  	}
  }
}
  // while(middle>0)
  //  {
  //   //check
  //   if(image[239*320+middle]==0)//combine two ways
  //  {
  //     //LCDPixel(middle,row,0xFF0000);//yellow
  //     tempmiddle = middle;
  //     left[239*320+middle]=1;
  //     break; //now tempmiddle = first left edge x coordinate
  //  }
  //   middle--;
  //  }
  // //towards the left
  // for(row=238;row>120;row--) //bottom 1/2
  // {
  // 	for(column=tempmiddle+80;column>(tempmiddle-80>0)?tempmiddle-80:0;column--)//window
  // 	{
  // 		if(image[row*320+column]==0)
  // 		{
  // 			tempmiddle = column;
  // 			left[row*320+column]=1;
  // 			break;
  // 		}
  // 	}
  // }

// void MarkRightLine(QVGAgray image, QVGAgray left)
// {
//   int row;
//   int column;
//   int middle = 160;
//   int tempmiddle = 160;
//   while(middle<320)
//    {
//     //check
//     if(image[239*320+middle]==0)//combine two ways
//    {
//       //LCDPixel(middle,row,0xFF0000);//yellow
//       tempmiddle = middle;
//       left[239*320+middle]=1;
//       break; //now tempmiddle = first left edge x coordinate
//    }
//     middle++;//towards the right
//    }
  
//   for(row=238;row>120;row--) //bottom 1/2
//   {
//   	for(column=tempmiddle-80;column<(tempmiddle+80<320)?tempmiddle-80:320;column++)//window
//   	{
//   		if(image[row*320+column]==0)
//   		{
//   			tempmiddle = column;
//   			left[row*320+column]=1;
//   			break;
//   		}
//   	}
//   }
// }

void setzero(QVGAgray a)
{
  int i,j;
  for(i=81;i<240;i++)
  {
    for(j=0;j<320;j++)
    {
      a[i*320+j]=0;
    }
  }
}
void MarkMiddle(QVGAgray left, QVGAgray right, QVGAgray mid)
{
  int row, column,counter=1;
  int leftpoint[2],rightpoint[2],middlepoint[2];//[0] stands for the x coordinate, [1] stands for the y coordinate
  setzero(mid);
  for(row=240;row>200;row--)
  {
    leftpoint[1] = 0; // to let two rows' initial value not be equal
    rightpoint[1] = 1;
    for(column=1;column<319;column++)
    {
      if(left[row*320+column]==1)
      {
        leftpoint[0] = column;
        leftpoint[1] = row;
      }
      if(right[row*320+column]==1)
      {
        rightpoint[0] = column;
        rightpoint[1] = row;
      }
      if(leftpoint[1] == rightpoint[1])
      {
         middlepoint[1] = leftpoint[1];
         middlepoint[0] = (leftpoint[0]+rightpoint[0])/2; //the root of column
        mid[middlepoint[1]*320+middlepoint[0]] = 1;
        LCDSetPrintf(counter,41,"row=%3d",middlepoint[1]);
        LCDSetPrintf(counter+1,41,"column=%3d",middlepoint[0]);
        if(counter<10)
          counter+=2;
        else
          counter = 1;
        
      }     
    }
  }
  
}

void DriveStraight(QVGAgray mid)
{
  int err,v,w;
  for(int row=239;row>120;row--)
  {
    for(int column=0;column<320;column++) 
    {
      if(mid[row*320+column] == 1)
      {
      	LCDSetPrintf(3,52,"column=%3d",column);
        err = 123 - column; // adjust the parameter is tough (left lane)
        LCDSetPrintf(4,52,"err=%3d",err);
        if (err >= -10 && err <= 10)  //middle
        {
		w=0;
		v=200;
        }
        else if(err < -10 && err > -50)       //left. should turn right
        {
		w=-30; //ONLY P, waiting to add I
		v=160;
        }
        else if (err <= -50)            //right
        {
		w=-30;
		v=160;
        }
        else if(err > 10 && err <50)
        {
        	w=30;
        	v=160;
        }
        else
        {
        	w=30;
        	v=160;
        }
        VWSetSpeed(v,w);
        LCDSetPrintf(1,52,"v=%3d",v);
        LCDSetPrintf(2,52,"w=%3d",w);
        break;
        break;
      }
    }
  }
}

int main ()
{
  CAMInit(QVGA);
  LCDMenu("L", "Z", "X", "LOVE");
  VWSetSpeed(200,0);
  do
  { 
    // L=PSDGet(l);
    // F=PSDGet(f);
    // OSWait(100); //delay 100ms
    setzero(left);
    setzero(right);
    setzero(mid);
    CAMGetGray(image);
    IPSobel(image,imageout); // imageout only edge, still a gray picture
    for (int i=0; i<PIX; i++) imageout[i] = (imageout[i]<65);
    LCDImageBinary(imageout);
    MarkLeftLine(imageout,left);// set left array value
    MarkRightLine(imageout,right);// set right array value
    MarkMiddle(left,right,mid);// set mid array value, show green points
    DriveStraight(mid); //here set velocity and angular speed
    // GetLeft(L);
    // if(PSDGet(l) - L < -predepth)
    // {
    //   LCDSetPrintf(7,1,"I come to the end\n");
    //   length = ENCODERRead(1);
    //   LCDSetPrintf(8,1,"endpoint = %d",PSDGet(f));
    //   LCDSetPrintf(9,1,"length=%4d\n",length*100/648);
    //   //OSWait(150);
    //   Park2(length, leftbig);
    //   break;
    // }
    //未做：切割图像，坏值剔除
  } while (KEYRead() != KEY4);
   VWSetSpeed(0,0);
  if(KEYGet()==KEY3)
  return 0;
}