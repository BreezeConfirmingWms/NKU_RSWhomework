#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<pthread.h>  
#include<signal.h>
#include<netinet/in.h>
#include<netdb.h>


#define SERVER_PORT 5432
#define MAX_PENDING 5
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
    
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int len;
    int s,new_s;
    bzero((char*)&sin,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);
 
    if((s=socket(PF_INET,SOCK_STREAM,0))<0){
        perror("simplex-talk:socket");
        exit(1);
    } 
    if((bind(s,(struct sockaddr*)&sin,sizeof(sin)))<0){
        perror("simplex-talk:bind");
        exit(1);
    }
    listen(s,MAX_PENDING);
    pthread_t tid;
    while(1){
        new_s=accept(s,(struct sockaddr*)&sin,&len);
        if((new_s)<0){
            perror("simplex-talk:accept");
            exit(1);
        }
       int ret1 = pthread_create(&tid,NULL,send_msg,(void*)new_s);
        int ret2= pthread_create(&tid,NULL,handle_msg,(void*)new_s);
        if(ret1==-1){
            printf('Send msg to Client Error!');
        }
        if(ret1==-1){
            printf('handle char from Client Error!');
        }
        //close(new_s);
    }


}
