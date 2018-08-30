#undef UNICODE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX 1024
#define BYTE char
int PORTA=4950;       // the port users will be connecting to
#define PORTS "4950"       // the port as a string users will be connecting to
#define MAXBUFLEN  100
#define INFO_BUFFER_SIZE 32

// globals

WSADATA wsaData;
struct addrinfo *result = NULL;
struct addrinfo *ptr;
struct addrinfo hints;

int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;  // IP address (4 bytes)

int ip2num[256]={0},row2ip[10]={0};
int num2ip[10]={0,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int num2row[10]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
bool info[10][10]={0},temp[10]={0};

SOCKET RADIOInit(SOCKET sockfdListening)
{
	DWORD dwRetval;
	int iResult;
	char *argv[4];
	argv[0] = NULL; // will be used to save system IP
	argv[2] = PORTS;
	CHAR  infoBuf[INFO_BUFFER_SIZE];
	DWORD  bufCharCount = INFO_BUFFER_SIZE;
	struct sockaddr_in  sockaddr_ipv4;
	struct in_addr addr;
	
	ptr = (struct addrinfo*)malloc(sizeof(struct addrinfo));
	memset(ptr, 0, sizeof(struct addrinfo));
	
	// Get and display the name of the computer.
	if (!GetComputerName(infoBuf, &bufCharCount))
		printf("Getting Computer Name failed \n");
	
	//printf("Computer name:      %s\n", infoBuf);
	argv[1] = infoBuf; //host name
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	memset(&hints, 0, sizeof hints);
	
	hints.ai_family = AF_INET; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	printf("Calling getaddrinfo with following parameters:\n");
	printf("\tnodename = %s\n", argv[1]);
	printf("\tservname (or port) = %s\n\n", argv[2]);

	dwRetval = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (dwRetval != 0) {
		printf("getaddrinfo failed with error: %d\n", dwRetval);
		WSACleanup();
		return 1;
	}

	sockfdListening = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfdListening == INVALID_SOCKET)
	{
		printf("RADIOInit: socket error\n");
		//continue;
	}
	addr.S_un = ((struct sockaddr_in *)(result->ai_addr))->sin_addr.S_un;

	argv[0] = inet_ntoa(addr);
	sscanf(argv[0], "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
	printf("Local IP Address is %d.%d.%d.%d\n\n", ip1, ip2, ip3, ip4);
	
	freeaddrinfo(result);
	return sockfdListening;
}


// id of destination
// buffer  is a string (NULL terminated)
//SOCKET created in init
int RADIOSend(int id,  char* bufS, SOCKET sockfdListening)
{
	
	int size = strlen(bufS);
	int status;
	int numbytes;
	char ipstr[INET_ADDRSTRLEN];
	char ip[16];
	int iResult;
	char buffer_id[10];
	sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, id);

	struct addrinfo *try1;
	char *argv;
	argv = PORTS;
	getaddrinfo(ip, argv, &hints, &try1);

	if ((numbytes = sendto(sockfdListening, bufS, size, 0,  try1->ai_addr, try1->ai_addrlen)) == -1) {
		printf("Error Sending Data");
	}
	
	return 0;


}


// sender id
// string buffer
int RADIOReceive(int id, char* bufR) {
	SOCKET s;
    struct sockaddr_in server, si_other;
    int slen, recv_len;
    int count;

    slen = sizeof(si_other);

    //Create a socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
    }
    //printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORTA);

    //Bind
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    //printf("Bind done\n");

    //keep listening for data
	//printf("Waiting for data...\n");
	fflush(stdout);
	//clear the buffer by filling null, it might have previously received data
	memset(bufR, '\0', MAXBUFLEN);
	
	//u_long iMode = 0; // blocking mode
	u_long iMode = 1; // non-blocking mode
	
	int iResult = ioctlsocket(s, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
		printf("blocking/non blocking failed with error: %ld\n", iResult);

	//try to receive some data, this is a blocking call
	int coun =0;
	while (coun<50)
	{
		if ((recv_len = recvfrom(s, bufR, MAXBUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			iResult=WSAGetLastError();
			if (iResult ==10035) // no data to read yet
			{
				Sleep(100);
				coun ++;
				//fflush(stdout);
				//clear the buffer by filling null, it might have previously received data
				memset(bufR, '\0', MAXBUFLEN);
			}
			else
			{
				printf("recvfrom() failed with error code : %d", iResult);
				return -1;
				break;
			}
		}
		else
			break;

	}
	closesocket(s);
	if(coun ==50)
		return -1;
	else
		return 1;
	//print details of the client/peer and the data received
	//printf("Received packet from ip %s, port:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	//printf("Data: %s\n", buf);
   // WSACleanup();

}

//Not sure what this function will be used for. 
int RADIOCheck(void)
{
	SOCKET sTemp;
    struct sockaddr_in server, si_other;
    int slen, recv_len;
    int count;
	char bufR[1024];
    slen = sizeof(si_other);

    //Create a socket
    if ((sTemp = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("RADIOCheck: Could not create socket : %d", WSAGetLastError());
    }
    //printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORTA);

    //Bind
    if (bind(sTemp, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("RADIOCheck: Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    //printf("Bind done\n");

	fflush(stdout);
	//clear the buffer by filling null, it might have previously received data
	memset(bufR, '\0', MAXBUFLEN);
	
	u_long iMode = 1; // non-blocking mode
	
	int iResult = ioctlsocket(sTemp, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
		printf("RADIOCheck: blocking/non blocking failed with error: %ld\n", iResult);

	//try to receive some data, this is a non-blocking call
	if ((recv_len = recvfrom(sTemp, bufR, MAXBUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
	{
		iResult=WSAGetLastError();
		if (iResult ==10035) // no data to read
		{
			//fflush(stdout);
			//clear the buffer by filling null, it might have previously received data
			printf("RADIOCheck: Nothing to read\n");
			memset(bufR, '\0', MAXBUFLEN);
			closesocket(sTemp);
			return 0;
		}
		else
		{
			printf("RADIOCheck: Failed with error code : %d\n", iResult);
			closesocket(sTemp);
			return -1;
		}
	}
	else
		printf("RADIOCheck: Read %d bytes of data\n",recv_len);
	closesocket(sTemp);
	return (recv_len>0);
}



int RADIORelease(SOCKET sockfdListening)
{
	if (closesocket(sockfdListening) == -1) {
		printf("Unable to close Socket\n");
		return -1;
	}
	return 0;
}

// Filters the robots from ARP cache which are active by pinging them individually
int FilterActive(int IDlistF[], int IDlistNF[])
{
	int k=0,j = 0;
	int PrevID = 0;
	char* fgetret[256];
	printf("Scanning for active bots.\n");
	for (k = 0; k < 255; k++)
	{
		if (IDlistNF[k] != 256)
		{
			int len = 256;
			char PingString[256];
			char psBuffer2[256];
			sprintf(PingString, "ping -n 1 %u.%u.%u.%u", ip1, ip2, ip3, IDlistNF[k]);

			FILE* fd2;
			fd2 = _popen(PingString, "r");
			printf(PingString);
			printf("\n");
			if (fd2 == NULL)
			{
				exit(1);
				printf("Pipe did not open\n");
			}
			fgets(psBuffer2, len, fd2);
			while (fgetret == NULL)
			{
				printf("Null returned\n");
			}

			do{
				if (strstr(psBuffer2, "TTL=") != NULL)
				{
					printf("Found Active Eyebot with IP %u.%u.%u.%u\n", ip1, ip2, ip3, IDlistNF[k]);
					IDlistF[j] = IDlistNF[k];
					j++;
					PrevID = IDlistNF[k];
				}
				else
				{
					continue;
				}

			} while ((fgets(psBuffer2, len, fd2)) != NULL);
			if (feof(fd2))
			{
				//printf("\nProcess returned %d\n", _pclose(fd2));
			}
			else
			{
				printf("Error: Failed to read the pipe to the end.\n");
			}
			_pclose(fd2);
			
		}
		else
		{
			printf("Total active EyeBots found: %d\n", j);
			
			return j;
		}
	}
	return j;
}

int RADIOStatus(int IDlist[])
// Returns number of robots (excl. self) and list of IDs in network
{
	FILE* fd;
	////Omar PC Mac a0-48-1c
	////Naeha PC MAC 44-6d-57
	fd = _popen("arp -a | FIND \"b8-27-eb\"", "r");
	//fd = _popen("arp -a | FIND \"44-6d-57\"", "r");
	int ipA, ipB, ipC, ipD;
	char   psBuffer[128];
	int   i = 0, j = 0;
	
	int allIDlist[256];

	if (fd  == NULL)
		exit(1);

	/* Read pipe until end of file, or an error occurs. */
	printf("Scanning ARP Cache\n");
	while (fgets(psBuffer, 120, fd))
	{ 
			sscanf(psBuffer, "%u.%u.%u.%u", &ipA, &ipB, &ipC, &ipD);
			
			printf(psBuffer);
			allIDlist[j] = ipD;
			j++;
	}
	allIDlist[j] = 256; // not a valid ip represents end of array
	printf("EyeBots found in ARP Cache: %d\n",j);
	if (feof(fd))
	{
		//printf("\nProcess returned %d\n", _pclose(fd));
	}
	else
	{
		printf("Error: Failed to read the pipe to the end.\n");
	}
	/* Close pipe and print return value of pPipe. */

	//i = j;
	i = FilterActive(IDlist, allIDlist);
	_pclose(fd);
	return i;
}

void PingNetwork(void)
{
	int k=0,j = 0;
	char* fgetret[256];

	
	printf("This process will take about two minute to complete.\n");
	printf("Scanning for active devices on Network...\n");
	for (k = 0; k < 255; k++)
	{
		int len = 256;
		char PingString[256];
		char psBuffer2[256];
		sprintf(PingString, "ping -w 10 -n 1 %u.%u.%u.%u", ip1, ip2, ip3,k);

		FILE* fd2;
		fd2 = _popen(PingString, "r");
		if (fd2 == NULL)
		{
			exit(1);
			printf("Pipe did not open\n");
		}
		
		fgets(psBuffer2, len, fd2);
		while (fgetret == NULL)
		{
			printf("Null returned\n");
		}

		do{
			if (strstr(psBuffer2, "TTL=") != NULL)
			{
				printf("Found network device with IP %u.%u.%u.%u\n", ip1, ip2, ip3, k);
				j++;
			}
			else
			{
				continue;
			}

		} while ((fgets(psBuffer2, len, fd2)) != NULL);
		if (feof(fd2))
		{
			//printf("\nProcess returned %d\n", _pclose(fd2));
		}
		else
		{
			printf("Error: Failed to read the pipe to the end.\n");
		}
		_pclose(fd2);

	}
	printf("Active devices found %d\n", j);
	//return j;
}

int RADIOGetID(void)
// Get own radio ID
{
	return ip4;
}

// ======================================================================================================
//The program I wrote starts here

static void SetPos(int x, int y){      //From https://blog.csdn.net/edc370/article/details/79944251 
    COORD point = { x, y }; //the set position of cursor x,y
    HANDLE HOutput = GetStdHandle(STD_OUTPUT_HANDLE);//Use GetStdHandle(STD_OUTPUT_HANDLE) to get standard output handle
	SetConsoleCursorPosition(HOutput, point); //Set cursor position
}

int UI(void){ // Display the user Interface 
	int i,j,k;
	system("cls");
	printf("Please type the commonds in the format of 'soccerbot_num command_num'\n");
	printf("**************************************************\n");
	printf("1 Move/ Stop\n" );
	printf("2 Turn left at the next crossing/ Cancel\n" ); 
	printf("3 Turn right at the next crossing/ Cancel\n" );
	printf("4 Turn around when it's possible/ Cancel\n" );
	printf("5 Park at the next parking zone/ Cancel\n" );
	printf("6 Follow mode\n" );
	printf("0 Exit base station\n");
	printf("********************Refresh in ********************\n");
	printf("Car  IP   Stop   Left   Right  Around Park   Follow\n");
	for(i=1,k=1;i<10;i++){
		if(num2ip[i]==-1)continue;
		printf(" %d   %d    ",i,num2ip[i]);
		for(j=0; j<6; j++)printf("%c      ",info[i][j]?'A':' ');
		printf("\n");
		row2ip[k++]=num2ip[i];
	}
	return 0;
}

int main ()
{ 
	int l, i, j, k, carnum=0, interval;
	time_t nowtime, ptime;
	int id[256];

	int myid,partnerid; 
	char bufS[MAX]; //Sending buffer 
	char *bufR = (char*)malloc(MAX * sizeof(char));  //Receiving buffer
	int car,cmd=10;
	
	memset(row2ip, -1, 40);  // this function can set this array 
		
	SOCKET sockfdListening = INVALID_SOCKET;
	//SOCKET sockfdReceiving = INVALID_SOCKET;

	printf("Initializing Radio\n");
	sockfdListening=RADIOInit(sockfdListening);

	myid = RADIOGetID();
	//printf("My id is %d\n", myid);

	printf("Do you want to scan whole network for active devices? (y/n)\n");
	char scan;
	scanf(" %c", &scan);
	if (scan == 'Y' | scan=='y')
		PingNetwork();
  // Use Ping Netwrok only if the Host is unabale to find the robots in network.
  // Pinging all the IP adressess takes time but it also updates the ARP cache with all available devices/
  // Function RadioStatus uses the ARP cache to identify the robots among other devices.
	printf("Scanning for Robots...\n");
	l = RADIOStatus(id); 
	RADIOCheck();
	if (l > 0)
	{
		printf("IP of active Eyebots: ");
		for (i = 0; i < l; i++)
		{
			printf("%d  ", id[i]);
			partnerid = id[i];
		}
		printf("\n");		
	}
	else
	{
		printf("No other robots found.\n");
		partnerid = 0;
	}
	RADIOCheck();
	
	sprintf(bufS, "Base station");  //Sending Base station to clarify the identity
	for(i=0;i<l;i++){
		partnerid=id[i];
		printf("Sending Message to ip %d\n",partnerid);
		RADIOSend(partnerid, bufS, sockfdListening);
		if (RADIOReceive( id[i], bufR ) == 1){  //Collecting information from cars
			sscanf (bufR, "This is car No.%d,%d,%d,%d,%d,%d,%d", &k,&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
            num2ip[k]=id[i];//adjust the number so that the number matches the number in reality
            for(j=0; j<6 ;j++ )info[k][j]=temp[j];
            ip2num[ id[i] ]=k;  //transfer from ip address to car number
            carnum++;
			printf("Found car No.%d\n",k);
		}
        //Test radioreceive only
	}
	for(j=1,k=1; j<10; j++){
    	if( num2ip[j] != -1 ) num2row[j] = k++;
        //printf("XXX j=%d,ip=%d,row=%d\n",j,num2ip[j],num2row[j]);
	}
	ptime = time(NULL);
	printf("Press any key to continue ......");
    //scanf("%c",bufS[0]);
	
	UI();
	
	while(cmd!=0){
		
		/*SetPos(0,17);  //Not working, RADIOCheck() always return 0 or -1 
		if( RADIOCheck()>0 ) {
			RADIOReceive(251, bufR );
			printf("RADIOCheck Received:%s",bufR);
			Sleep(5000);
		}*///Never run successfully 
		
		if(kbhit()){  //if some type in some words
			scanf("%d %d",&car,&cmd);  //scan the command
			printf("car no.%d,cmd no.%d\n",car,cmd);
			if( cmd == 0){  //if command is 0, exit the program 
				printf("now existing...\n");
				break;
			}
			else if( car < 0 || car > 6 || num2ip[car]==-1 || cmd < 0 || cmd > 6 ){  //If got wrong command, print error information
				printf("Wrong command, please try again\n");
				Sleep(1000);
				SetPos(0,carnum+11);
				for(i=0;i<10;i++)printf("                                                  \n");
				SetPos(0,carnum+11);
				continue;
			}
			sprintf(bufS, "%d", cmd );
			if(car == 0){  // Type in 0 means sending commands to all cars 
				for(i = 1; i < 10; i++ ){
					if( row2ip[i] == -1 )continue;
					RADIOSend(row2ip[i], bufS, sockfdListening);
					for(j=0; j<3; j++ ){ //Try 3 times before giving up 
						RADIOReceive(row2ip[i], bufR ); //Collecting information from cars
						if( bufR[0]==cmd+'0' )break;
						printf("Fail to receive the ACK, Receive %s\n",bufR);
						sprintf(bufS, "9%c",'\0');
						RADIOSend(num2ip[car], bufS, sockfdListening);
					}
					SetPos(12,10+i);  // refresh all information table
					for(j=1;j<7;j++)printf("%c      ",bufR[j]-'0'?'A':' ');
				}
				SetPos(0,carnum+11);
				for(i=0;i<10;i++)printf("                                                  \n");
				SetPos(0,carnum+11);
				continue;
			}
			RADIOSend(num2ip[car], bufS, sockfdListening);  //Normal command to one car 
			for(j=0; j<3; j++ ){
				RADIOReceive( num2ip[car], bufR );
				printf("Received: %s",bufR);
				if(bufR[0]==cmd+'0')break;
				printf("Fail to receive the ACK\n");
				sprintf(bufS, "9%c",'\0');
				RADIOSend(num2ip[car], bufS, sockfdListening);
			}
			SetPos(12,10+num2row[car]);  //refresh all information table
			for(i=1;i<7;i++)printf("%c      ",bufR[i]-'0'?'A':' ');
			Sleep(1000);
			SetPos(0,carnum+11);
			for(i=0;i<10;i++)printf("                                                  \n");
			SetPos(0,carnum+11);
		}
		nowtime = time(NULL);
		if( interval != nowtime - ptime){
			SetPos(31,9);//Display Refreshing time 
			interval = nowtime - ptime; // k = time interval
			printf("%ds*",30-interval);
			SetPos(0,carnum+11);
		}
		if( interval > 30 ){  //Refresh the network state every 1 minute
			
			memset(num2ip,-1,40);
			memset(num2row,-1,40);
			memset(row2ip,-1,40);
			
			num2ip[0]=0;
			carnum=0;
			
			l = RADIOStatus(id); 
			RADIOCheck();
			if (l < 0){
				printf("No other robots found.\n");
				partnerid = 0;
			}
			RADIOCheck();
			 
			memset(bufS, '\0', MAX);
			sprintf(bufS, "Base station");  //Sending Base station to clarify the identity
			for(i=0;i<l;i++){
				partnerid=id[i];
				RADIOSend(partnerid, bufS, sockfdListening);
				if (RADIOReceive( id[i], bufR ) == 1){  //Collecting information from cars
					sscanf (bufR, "This is car No.%d,%d,%d,%d,%d,%d,%d", &k,&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
         			num2ip[k]=id[i];//adjust the number so that the number matches the number in reality
            		for(j=0; j<6 ;j++ )info[k][j]=temp[j];
           			ip2num[ id[i] ]=k;  //transfer from ip address to car number
            		carnum++;
					//printf("Found car No.%d\n",k);
				}
			}
			for(j=1,k=1; j<10; j++){
            	if( num2ip[j] != -1 ) num2row[j] = k++;
            	//printf("XXX j=%d,ip=%d,row=%d\n",j,num2ip[j],num2row[j]);
			}
			Sleep(5000);
			UI();
			ptime = time(NULL);
		}
	}

	printf("Program will terminate in 5 sec\n");
	printf("have a nice day ;>");
	RADIORelease(sockfdListening);
	Sleep(5000);
}

