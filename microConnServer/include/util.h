#ifndef _UTIL_H
#define _UTIL_H
#include<stdlib.h>
ssize_t writen(int fd,const void*vptr,size_t n);
ssize_t readn(int fd,void*vptr,size_t n);
#endif