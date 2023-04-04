#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<pthread.h>  
#include<signal.h>
#include<netinet/in.h>
#include<netdb.h>




#define SERVER_PORT 5432
#define MAX_LINE 256



void * send_msg(void * pth_arg){

    long cfd =(long)pth_arg;
    int len=0;
    char buf[100]={0};
     while(fgets(buf,sizeof(buf),stdin)){
        buf[MAX_LINE-1]='\0';
        len = strlen(buf)+1;
        send(cfd,buf,len,0);
    }
    pthread_exit(NULL);

}

void*handle_msg(void*pth_arg){
    int ret =0;
    char buf[100]={0};
    long cfd =(long)pth_arg;
    while(ret=recv(cfd,buf,sizeof(buf),0)){
    	printf('print msg from Server\n');
        fputs(buf,stdout);
    }
    pthread_exit(NULL);
}

int main(int argc,char*argv[]){
    FILE*fp;
    struct hostent *hp;
    struct sockaddr_in sin;
    char*host;
    char buf[MAX_LINE];
    int s=-1;
    int len;
    if(argc ==2){
        host = argv[1];
    }
    else{
        fprintf(stderr,"usage:simple-talk host \n");
        exit(1);
    }
    hp = gethostbyname(host);
    bzero((char*)&sin,sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr,(char*)&sin.sin_addr,hp->h_length);
    sin.sin_port = htons(SERVER_PORT);
    s=socket(PF_INET,SOCK_STREAM,0);
    if(s<0){
        perror("simplex-talk:socket");
    }
    if(connect(s,(struct sockaddr*)&sin,sizeof(sin))<0){
        perror("simplex-talk:connect");
        close(s);
        exit(1);
    }

    pthread_t tid;
    while(1){
        bzero(&buf,sizeof(buf));
        int ret1 = pthread_create(&tid,NULL,send_msg,(void*)s);
        int ret2= pthread_create(&tid,NULL,handle_msg,(void*)s);
        if(ret1==-1){
            printf('Send msg to Client Error!');
        }
        if(ret1==-1){
            printf('handle char from Client Error!');
        }
    }
   

}
