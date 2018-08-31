/* This parking algorithm uses the PSD and odometry encoder.
It is suitable for the cave-like parking zone.
Using the PSD on the left to detect the difference between two sequent distances.
We can know where is the start point and the end point.
*/

#include "eyebot.h"
#define predepth  120 //This value is based on experience.
#define f 2 //In eyesim, 
#define l 1
int L;
int F;
int startpoint=0,endpoint=0,distance=0,leftsmall=0, leftbig=0, depth=0,length=0;
void ShowInfo(int L, int F)
{
  LCDSetPrintf(1,1,"L=%4d\n",L);
  LCDSetPrintf(2,1,"F=%4d\n",F);
}

void ObstableAvoidance()
{
  while(PSDGet(f)<100) // cannot write F<100
  {
    VWSetSpeed(0,0);
  }
  VWSetSpeed(100,0);
}

void GetDepthInfo(int L)
{ 
    if(PSDGet(l) - L > predepth)
    {
      ENCODERReset(2);
      LCDSetPrintf(3,1,"I come to the start!");
      leftbig = PSDGet(l);// the left value now substract the history value
      leftsmall = L;
      depth = leftbig - leftsmall;
      LCDSetPrintf(4,1,"leftsmall=%4d\n", leftsmall);
      LCDSetPrintf(5,1,"leftbig=%4d\n", leftbig);
      LCDSetPrintf(6,1,"depth=%4d\n",depth);
    }
}

int main ()
{
  LCDMenu("", "", "", "END");
  VWTurn(90,90);
  OSWait(1275); //It can help the eyebot rotate 90 degrees counterclockwise.
  VWSetSpeed(200,0);
  do
  { 
    L=PSDGet(l);
    F=PSDGet(f);
    OSWait(100); //delay 100ms; to see the difference of the eyebot
    ShowInfo(L,F);
    ObstableAvoidance();
    GetDepthInfo(L);
    if(PSDGet(l) - L < -predepth)
    {
      LCDSetPrintf(7,1,"I come to the end\n");
      length = ENCODERRead(2);
      LCDSetPrintf(9,1,"length=%4d\n",length*100/648);
      VWSetSpeed(-150,0);
	  OSWait(length);// by experience. adjust the value according to the actural environment
	  VWSetSpeed(0,-60);
	  OSWait(1900);
	  VWSetSpeed(-100,0);
	  OSWait(leftbig*10); //by experience 
	  VWSetSpeed(0,60);
	  OSWait(1900);
	  VWSetSpeed(0,0);
      break;
    }
  } while (KEYRead() != KEY4);
   VWSetSpeed(0,0);
  return 0;
}