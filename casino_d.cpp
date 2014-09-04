#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>

#define PORT "22068" // the port client will be connecting to 
#define MAXDATASIZE 100 // max number of bytes we can get at once 
#define MYPORT 12078
#define MYPORT2 13078
#define THUB 32078
#define CASINOC 11078


using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
input parameter: char array to store vector message from text file
reads stop_ txt, store the number into message[] in <,,,> form
*/
void readFile(char message[],int size,int station[]){
    for(int i=0;i<size;i++)
       message[i] = '\0';
    ifstream fpt;
    string temp, num;
    string myVect="<";
    fpt.open("stop_d.txt");
    fpt>>temp>>temp>>temp>>num;
    myVect += num;
    myVect += ",";
    station[0]= atoi(num.c_str());
    fpt>>temp>>temp>>temp>>num;
    myVect += num;
    myVect += ",";
    station[1]= atoi(num.c_str());
    fpt>>temp>>temp>>temp>>num;
    myVect += num;
    myVect += ",";
    station[2]= atoi(num.c_str());
    fpt>>temp>>temp>>temp>>num;
    myVect += num;
    myVect += ">";
    station[3]= atoi(num.c_str());
    for(int i=0;i<myVect.size();i++)
      message[i]=myVect[i];
    message[myVect.size()]='D';
    fpt.close();
    return;
}
void StringtoInt(char [],int,int []);
void InttoString(char [],int,int []);

int main()
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[64];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("nunki.usc.edu", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
    sockaddr_in *ipv4 = (sockaddr_in*)p->ai_addr;

    freeaddrinfo(servinfo);
    char message[20];
    int myStation[4];
    readFile(message,sizeof(message),myStation);
    sockaddr_in tcp;
    socklen_t tcpSize;
    tcpSize = sizeof(tcp);
    int socknum = getsockname(sockfd,(sockaddr*)&tcp,&tcpSize);
    if((send(sockfd,message,sizeof(message),0))!=-1)
       close(sockfd);
    message[strlen(message)-1]='\0';
    cout<<"Casino D Phase 1: The casino D has dynamic TCP port number "<<ntohs(tcp.sin_port)<<" and IP address "<<s<<endl;
    cout<<"Casino D Phase 1: Sending the casino vector "<<message<<" to the transit hub"<<endl;
    cout<<"Casino D Phase 1: End of Phase 1\n";
    /*Phase 2*/
    struct sockaddr_in client,server,temp;
    int sock;
    socklen_t n;
    int len;
    char b1[100];
    char b2[100];
    sock=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port=MYPORT;
    server.sin_addr.s_addr=inet_addr("nunki.usc.edu");
    if(bind(sock,(struct sockaddr *)&server,sizeof(server))==-1){
       cout<<"Bind error\n";                     
    }
    n=sizeof(client);

    if(recvfrom(sock,b1,sizeof(b1),0,(struct sockaddr *) &client,&n)==-1){
       cout<<"Receiving error\n";
    }
    cout<<"Casino D Phase 2: The casino D has static UDP port number "<<server.sin_port<<endl;
    cout<<"Casino D Phase 2: Received the train vector "<<b1<<" casino C\n";
    
    int received[4];
    StringtoInt(b1,sizeof(b1),received);
    myStation[3]=received[3];
    received[3]=0;
    InttoString(b2,sizeof(b2),received);
    
    int sock2;
    sockaddr_in server2,client2,temp2;
    sock2=socket(AF_INET,SOCK_DGRAM,0);
    server2.sin_family=AF_INET;
    server2.sin_port= MYPORT2;
    server2.sin_addr.s_addr=inet_addr("nunki.usc.edu");
    if(bind(sock2,(struct sockaddr *)&server2,sizeof(server2))==-1){
       cout<<"Bind error\n";                     
    }
    
    sock=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port=THUB;
    server.sin_addr.s_addr=inet_addr("nunki.usc.edu");
    n=sizeof(server);
    if(sendto(sock,b2,sizeof(b2),0,(struct sockaddr *) &server,n)==-1){
       cout<<"Sending error\n";
    }

    temp = server;
    len = sizeof(temp);
    getsockname(sock, (struct sockaddr *)&temp, (socklen_t*)&len);
    cout<<"Casino D Phase 2: The casino D has dynamic UDP port number "<<ntohs(temp.sin_port)<<endl;
    cout<<"Casino D Phase 2: Sending the updated train vector "<<b2<<" to transit hub\n";
    cout<<"Casino D Phase 2: The updated casino vector at D is ";
    char StationS[100];
    InttoString(StationS,sizeof(StationS),myStation);
    cout<<StationS<<endl;    
    cout<<"Casino D Phase 2: End of Phase 2\n";

    /*Phase 3*/
    n=sizeof(client2);
    char b4[100];
    if(recvfrom(sock2,b4,sizeof(b4),0,(struct sockaddr *) &client2,&n)==-1){
       cout<<"Receiving error\n";
    }
    cout<<"Casino D Phase 3: The casino D has static UDP port number "<<server2.sin_port<<endl;
    cout<<"Casino D Phase 3: Received the train vector "<<b4<<" from transit hub\n";

    char b3[100];
    StringtoInt(b4,sizeof(b4),received);
    for(int i=0;i<3;i++){
       received[i]+=myStation[i];
       myStation[i]=0;
    }
    received[3]=0;    
    InttoString(b3,sizeof(b3),received);


    sock2=socket(AF_INET,SOCK_DGRAM,0);
    server2.sin_family=AF_INET;
    server2.sin_port= CASINOC;
    server2.sin_addr.s_addr=inet_addr("nunki.usc.edu");
    n=sizeof(server2);
    if(sendto(sock2,b3,sizeof(b3),0,(struct sockaddr *) &server2,n)==-1){
       cout<<"Sending error\n";
    }

    temp2 = server2;
    len = sizeof(temp2);
    getsockname(sock2, (struct sockaddr *)&temp2, (socklen_t*)&len);
    cout<<"Casino D Phase 3: The casino D has dynamic UDP port number "<<ntohs(temp2.sin_port)<<endl;
    cout<<"Casino D Phase 3: Sending the updated train vector "<<b3<<" to casino C\n";
    cout<<"Casino D Phase 3: The updated casino vector at D is ";
    InttoString(StationS,sizeof(StationS),myStation);
    cout<<StationS<<endl;    

    cout<<"Casino D Phase 3: End of Phase 3\n";
    return 0;
}

void StringtoInt(char message[],int size,int station[]){
     int i=0;
     int j=0;
     int count=0;
     int myNum;
     char temp[3]="";
     for(j=1;j<size;j++){
        i=0;
        while(message[j]!=','&&message[j]!='>'){
           temp[i]=message[j];
           i++;j++;
        }
        station[count]= atoi(temp);
        count++;
        for(int k=0;k<3;k++)
           temp[k]='\0';
        if(message[j]=='>') return;
     }
}

void InttoString(char message[],int size,int station[]){
     for(int i=0;i<size;i++)
        message[i]='\0';

     string temp="<";
     string myNum2;
     char myNum[3]="";
     ostringstream ss;
     ss << station[0];
     for(int i=1;i<4;i++){
        ss << "," << station[i];
        myNum2 = ss.str();
        for(int j=0;j<3;j++)
           myNum[j] = '\0';
     }
     myNum2.append(">");
     temp.append(myNum2);

     for(int i=0;i<temp.size();i++)
        message[i]=temp[i];
}
