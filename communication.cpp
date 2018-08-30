#include "eyebot++.h"
#include <pthread.h>
#include <string.h>

#define RES  QVGA
#define SIZE QVGA_SIZE
#define SIZEG QVGA_SIZE/3
#define MAX 50
#define CarNum 3 //change this on each car

bool executing = TRUE,display = FALSE,stop = FALSE,connect_state = FALSE;
bool turnleft = FALSE,turnright = FALSE, turnaround = FALSE, park = FALSE, follow = FALSE;
int linespeed = 0, angspeed = 0;
char menu[4][20]={0}; //The words displayed on the 4 keys

int myid, base_id, partnerid, follower_id = -1, leader_id = -1;
char buf[MAX]= {'\0'};

pthread_mutex_t img1,img2,flags;

BYTE image1[SIZEG],image2[SIZEG];

void *ftsd1(void *arg){
    return NULL;
}

void *ftsd2(void *arg){
    return NULL;
}

void *lane_detection(void *arg){
    return NULL;
}

void *communicate(void *arg){
	char buf[MAX]= {'\0'};
	int cmd,hr,min,sec,tick;
	int phr,pmin,psec,ptick;
	RADIOInit();
	myid = RADIOGetID();
	LCDSetPrintf(16,0,"My id is %3d,\n",myid);
	LCDSetPrintf(16,13,"connecting to base station\n");
	do{  //Test
		memset(buf,'\0',MAX);
		RADIOReceive(&base_id, buf, MAX);
		//LCDSetPrintf(3,0,"Received %s                    ",buf);
	}while( executing && strcmp( buf, "Base station" ) != 0 );
	memset(buf,'\0',MAX);
	sprintf(buf, "This is car No.%d,%d,%d,%d,%d,%d,%d",CarNum,stop,turnleft,turnright,turnaround,park,follow);
	RADIOSend(base_id, buf);
	LCDSetPrintf(16,13,"base station online, id %d\n",base_id);
	LCDSetPrintf(15,0,"Flags: %d,%d,%d,%d,%d,%d",stop,turnleft,turnright,turnaround,park,follow);
	connect_state=1;
	memset(buf,'\0',MAX);
	OSGetTime(&phr,&pmin,&psec,&ptick);
	while(executing){
		//LCDSetPrintf(11,0,"RADIOCheck=%d",RADIOCheck() );
		memset(buf,'\0',MAX);
		RADIOReceive(&partnerid, buf, MAX);
		if(executing==0)break;
		if(buf[0]=='B'){
			if( strcmp( buf, "Base station" ) == 0 ){
				base_id = partnerid;
				memset(buf,'\0',MAX);
				sprintf(buf, "This is car No.%d,%d,%d,%d,%d,%d,%d", CarNum, stop, turnleft, turnright, turnaround, park, follow);
				RADIOSend(base_id, buf);
				connect_state = 1;
				LCDSetPrintf( 16, 13, "base station online, id %d     \n", base_id );
				OSGetTime(&phr,&pmin,&psec,&ptick);
				continue;
			}
		}
		if(partnerid == base_id){ // Commands from the base station
			switch(buf[0]){
				case '1': 
					stop = !stop;
					if(stop) sprintf(menu[0], "START%c",'\0');
					else sprintf(menu[0], "STOP%c",'\0');
					LCDMenu(menu[0],menu[1],menu[2],menu[3]);
					break;
				// Turn left, right, around can't be true at the same time	
				case '2': // Turn left
					if( turnright || turnaround ) break;
					turnleft = !turnleft; break;
				case '3': // Turn right
					if( turnleft || turnaround ) break;
					turnright = !turnright; break;
				case '4': // Turn around
					if( turnleft || turnright ) break;
					turnaround = !turnaround; break;
				case '5': // Following mode
					if(follow) break;
					park = !park;
					if(park){
						sprintf(menu[1], "CNCL PRK%c",'\0');
						sprintf(menu[2], "%c",'\0');
					}
					else{
						sprintf(menu[1], "PARK%c",'\0');
						sprintf(menu[2], "FOLLOW%c",'\0');
					}
					LCDMenu(menu[0],menu[1],menu[2],menu[3]);
					break;
				case '6': // Parking mode
					if(park) break;
					follow = !follow;
					if(follow){
						sprintf(menu[2], "CNCL FLLW%c",'\0');
						sprintf(menu[1], "%c",'\0');
						//RADIOSend(base_id, "follower");
					}
					else{
						sprintf(menu[2], "FOLLOW%c",'\0');
						sprintf(menu[1], "PARK%c",'\0');
					}
					LCDMenu(menu[0],menu[1],menu[2],menu[3]);
					break;
				/*
				Inportant!!!  The following 2 case doesn't work for now.
				The orginal idea of these is apply to the computer for the leader's id
					or the follower's id. Then they can communicate with each other. The
					leader is supposed to send its speed continously to the follower so	
					that the follower can copy its movement.
				Also, the follow mode and park mode can't exist at the same time.
				*/
				case '7': 
					if(buf[1]!=','){
						LCDSetPrintf(14,0,"Wrong follower commands");
						break;
					}
					follower_id=buf[2]*100+buf[3]*10+buf[4];
					break;
				case '8': 
					if(buf[1]!=','){
						LCDSetPrintf(14,0,"Wrong leader commands");
						break;
					}
					leader_id=buf[2]*100+buf[3]*10+buf[4];
					memset(buf,'\0',MAX);
					sprintf(buf, "This is follower");
					RADIOSend(leader_id, buf);
					break;
				case '9': // Resending the message but doesn't trigger any alarm
					buf[0] = cmd; //Store the old command
					break;
				default: LCDSetPrintf(14,0,"Receiving wrong commands");
			}
			//LCDSetPrintf(14,0,"cmd=%d,buf[0]=%d",cmd,buf[0]);
			cmd=buf[0]; //Store the command so that if needed, we can resend the previous message
			memset(buf,'\0',MAX);
			sprintf(buf, "%c%d%d%d%d%d%d",cmd,stop,turnleft,turnright,turnaround,park,follow);
			RADIOSend(base_id, buf);  //Send command turnaround to check the result
			memset(buf,'\0',MAX);
		}
		else if(partnerid == leader_id){ // Not working for now
			sscanf(buf,"%d,%d",&linespeed,&angspeed);
		}
		else if(partnerid == follower_id){ // Not working for now
			if( strcmp( buf, "This is follower" ) ){
				LCDSetPrintf(14,0,"This is Leader, successfully connect to the follower");
				memset(buf,'\0',MAX);
				sprintf(buf, "This is leader");
				RADIOSend(leader_id, buf);
			}
			while(follow){
				memset(buf,'\0',11);
				sprintf(buf, "%d,%d",linespeed,angspeed);
				RADIOSend(follower_id, buf);
			}
		}
		memset(buf,'\0',MAX);
		OSGetTime(&hr,&min,&sec,&tick);
		LCDSetPrintf( 13, 0, "Time left to refresh: %d", (hr-phr)*3600 + (min-pmin)*60 + (sec-psec) );
		if( (hr-phr)*3600 + (min-pmin)*60 + (sec-psec) > 70 && connect_state ){
			connect_state=0;
			LCDSetPrintf(16,13,"base station offline            ");
		}
		LCDSetPrintf(15,0,"Flags: %d,%d,%d,%d,%d,%d",stop,turnleft,turnright,turnaround,park,follow);	
	}   // Loop of executing
	RADIORelease();
    return NULL;
}

void *key(void *arg){
	if(stop) sprintf(menu[0], "START%c",'\0');  //Init
	else sprintf(menu[0], "STOP%c",'\0');
	sprintf(menu[1], "PARK%c",'\0');
	sprintf(menu[2], "FOLLOW%c",'\0');
	sprintf(menu[3], "EXIT%c",'\0');
	LCDMenu(menu[0],menu[1],menu[2],menu[3]);
	
	while(executing){
		switch(KEYGet()){
			case KEY1:
				stop = !stop;
				if(stop) sprintf(menu[0], "START%c",'\0');
				else sprintf(menu[0], "STOP%c",'\0');
				break;
			case KEY2:
				if(follow) break;
				park = !park;
				if(park){
					memset(buf,'\0',MAX);
					sprintf(buf, "9park");
					RADIOSend(base_id, buf);
					sprintf(menu[1], "CNCL PRK%c",'\0');
					sprintf(menu[2], "%c",'\0');
				}
				else{
					sprintf(menu[1], "PARK%c",'\0');
					sprintf(menu[2], "FOLLOW%c",'\0');
				}
				break;
			case KEY3:
				if(park) break;
				follow = !follow;
				if(follow){
					sprintf(menu[2], "CNCL FLLW%c",'\0');
					sprintf(menu[1], "%c",'\0');
				}
				else{
					sprintf(menu[2], "FOLLOW%c",'\0');
					sprintf(menu[1], "PARK%c",'\0');
				}
				break;
			case KEY4:
				executing=0;
				LCDClear();
				LCDSetPrintf(0, 0, "Exiting...             \n");
				break;
			default: LCDPrintf("Detect an error of input key\n");
		}
		LCDMenu(menu[0],menu[1],menu[2],menu[3]);
	}
	return NULL;
}

int main() {
    pthread_t t1, t2, t3, t4, t5;
	
    XInitThreads();
    pthread_mutex_init(&img1,NULL);
	pthread_mutex_init(&img2,NULL);
	pthread_mutex_init(&flags,NULL);
    CAMInit(RES);
    LCDImageSize(SIZE);
	
    pthread_create(&t1, NULL, ftsd1, (void *) 1);
    pthread_create(&t2, NULL, ftsd2, (void *) 2);
    pthread_create(&t3, NULL, lane_detection, (void *) 3);
	pthread_create(&t4, NULL, communicate, (void *) 4); 
	pthread_create(&t5, NULL, key, (void *) 5);
	
    pthread_exit(0); // will terminate program
	LCDPrintf("Main programme exit\n");
}
