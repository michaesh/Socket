#include <iostream>
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
#include <sys/wait.h>f
#include <cmath>
#include <sstream>

#define MYPORT "22068"
#define RPORT1 32078
#define RPORT2 33078
#define CASINOA 6078
#define CASINOD 13078

using namespace std;

void *get_in_addr(struct sockaddr *sa){
  if(sa->sa_family == AF_INET) return &(((sockaddr_in*)sa)->sin_addr);}

int getWagon(char [],int);
void StringtoInt(char [],int,int []);
void InttoString(char [],int,int []);

int main(){
    sockaddr *addr = new sockaddr;
    int sockfd;
    int new_fd;
    addrinfo hints;
    addrinfo *res;
    addrinfo *pt;
    socklen_t *temp = new socklen_t;
    int yes = 1;
    sockaddr_storage their_addr;
    socklen_t addr_size;
    char myTempChar;

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
 
    getaddrinfo("nunki.usc.edu",MYPORT,&hints,&res);
    
    char ipstr[64];
    int i=0;
    for(pt=res; pt!=NULL; pt=pt->ai_next){
      void *addr;
      sockaddr_in *ipv4 = (sockaddr_in*)pt->ai_addr;
      addr = &(ipv4->sin_addr);
      inet_ntop(AF_INET,addr,ipstr,64);
      cout<<"Hub Phase 1: The transit hub has TCP Port of number "<<ipv4->sin_port;
      cout<<" and IP Address:"<<ipstr<<endl;
    }

    //ipstr now holds IP Address of nunki.usc.edu
    sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(bind(sockfd,res->ai_addr,res->ai_addrlen)==-1){
       cout<<"Binding Error\n";
       return 0;
    }
    freeaddrinfo(res);

    char s[64];
    listen(sockfd,10);
    bool flag = 0;
    int num=0;
    int sum=0;
    while(num<4){
        num++;
        addr_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr*)&their_addr,&addr_size);
        if(new_fd!=-1){
           inet_ntop(their_addr.ss_family,get_in_addr((sockaddr*)&their_addr),s,sizeof s);
           char message[100]={0};
 	       if(recv(new_fd,message,63,0)!=-1){
              int size = strlen(message);
              cout<<"Hub Phase 1: The transit hub received casino vector from "<<message[size-1]<<" ";
              close(new_fd);
              sum+=getWagon(message,size);
              for(int i=0;i<size;i++){
                 cout<<message[i];
 	             if(message[i]=='>'){
                    cout<<endl;
                    for(int k=0;k<100;k++){
                       message[k]=' ';
                    }
                 }//for if(mess)
              }//for loop
           }
        }
    }
    close(sockfd);
    cout<<"Hub Phase 1: End of Phase 1"<<endl;

    /*Phase 2*/
    struct sockaddr_in client,server,mytemp;
    int sock;
    socklen_t n;
    int len;
    char b1[] ="<0,0,0,0>";
    char b2[100];
    sock=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port= CASINOA;
    server.sin_addr.s_addr=inet_addr("nunki.usc.edu");
    n=sizeof(server);
    if(sendto(sock,b1,sizeof(b1),0,(struct sockaddr *) &server,n)==-1){
       cout<<"Sending error\n";
    }

    mytemp = server;
    len = sizeof(mytemp);
    getsockname(sock, (struct sockaddr *)&mytemp, (socklen_t*)&len);
    cout<<"Hub Phase 2: The transit hub has dynamic UDP port number "<<ntohs(mytemp.sin_port)<<endl;
    
    sock=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port= RPORT1;
    server.sin_addr.s_addr=inet_addr("nunki.usc.edu");
    if(bind(sock,(struct sockaddr *)&server,sizeof(server))==-1){
       cout<<"Bind error\n";                     
    }
    n=sizeof(client);
    cout<<"Hub Phase 2: Sending the train vector with "<<ceil(sum/10.0)<<" wagons to casino A\n";
    if(recvfrom(sock,b1,sizeof(b1),0,(struct sockaddr *) &client,&n)==-1){
       cout<<"Receiving error\n";
    }
    cout<<"Hub Phase 2: The transit hub has static UDP port number "<<server.sin_port<<endl;
    cout<<"Hub Phase 2: Received the train vector "<<b1<<" from casino D\n";
    
    /*Phase 3*/
    int sock2;
    sockaddr_in server2, client2, mytemp2;
    sock2=socket(AF_INET,SOCK_DGRAM,0);
    server2.sin_family=AF_INET;
    server2.sin_port=CASINOD;
    server2.sin_addr.s_addr=inet_addr("nunki.usc.edu");
    n=sizeof(server2);
    char b3[] = "<0,0,0,0>";
    while(sendto(sock2,b3,sizeof(b3),0,(struct sockaddr *) &server2,n)==-1){
       cout<<"Sending error\n";
    }

    mytemp2 = server;
    len = sizeof(mytemp2);
    getsockname(sock2, (struct sockaddr *)&mytemp2, (socklen_t*)&len);
    cout<<"Hub Phase 3: The transit hub has dynamic UDP port number "<<ntohs(mytemp2.sin_port)<<endl;
    cout<<"Hub Phase 3: Sending the train vector <0,0,0,0> to casino D\n";
    sock2=socket(AF_INET,SOCK_DGRAM,0);
    server2.sin_family=AF_INET;
    server2.sin_port= RPORT2;
    server2.sin_addr.s_addr=inet_addr("nunki.usc.edu");
    if(bind(sock2,(struct sockaddr *)&server2,sizeof(server2))==-1){
       cout<<"Bind error\n";                     
    }
    n=sizeof(client);

    if(recvfrom(sock2,b1,sizeof(b1),0,(struct sockaddr *) &client2,&n)==-1){
       cout<<"Receiving error\n";
    }
    cout<<"Hub Phase 3: The transit hub has static UDP port number "<<server2.sin_port<<endl;
    cout<<"Hub Phase 3: Received the train vector "<<b1<<" from casino A\n";
    cout<<"Hub Phase 3: End of Phase 3\n";
    return 0;
}

/*
input: char array sent by casinos in form of <,,,>char, and the size of the message
will read the message starting according to their station.
returns the total amount of passangers
*/
int getWagon(char message[],int size){
    int count;
    if(message[size-1]=='A')
       count = 1;
    if(message[size-1]=='B')
       count = 2;
    if(message[size-1]=='C')
       count = 3;
    if(message[size-1]=='D')
       return 0;
    
    char final[3]={0};
    char dummy[3]={0};
    int num=0;
    int sum=0;
    int i=2*count+1;
    int j=0;
    char temp;
    //0-9
    while(i<size-1){
       j=0;
       while(message[i]!=','&&message[i]!='>'){
          final[j]=message[i];
          i++;
          j++;
       }
       i++;
       num = atoi(final);
       sum+=num;
       for(int k=0;k<3;k++)
          final[k]=' ';
    }
    return sum;
}
