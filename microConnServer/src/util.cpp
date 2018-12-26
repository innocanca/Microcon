#include"util.h"
#include<unistd.h>
#include<errno.h>
#include<iostream>

ssize_t readn(int fd,void*vptr,size_t n){
        size_t nleft;
        ssize_t nread;
        void *ptr=0;
        ptr=vptr;
        nleft=n;
        while(nleft>0){
	        if( (nread=read(fd,ptr,nleft))<0){
		        if(errno==EINTR)
		                nread=0;
		        else
		                return(-1);
		        }
		else if(nread==0)
			break;
		nleft-=nread;
		ptr+=nread;
	}
	return n-nleft;
}

ssize_t writen(int fd,const void*vptr,size_t n){
        size_t nleft;
        ssize_t nwritten;
        const void* ptr=0;
        ptr=vptr;
        nleft=n;
        while(nleft>0){
	        if((nwritten=write(fd,ptr,nleft))<0){
	      		  if(nwritten<0&&errno==EINTR)			                                     nwritten=0;
		          else
			         return -1;
		}				
    		nleft-=nwritten;
		ptr+=nwritten;
	}
	return n;
}
