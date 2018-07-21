#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <regex.h> 
#include <errno.h>
#include <assert.h>
#include <event.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/http.h>
#include <event2/dns.h>
#include <pthread.h>
#include <time.h>
#define URLLENGTH 250
//#define BUFFERSIZE 8192
int ENDFLAG = 0;
//int URLLENGTH = 250;
int urlNum = 1;

//timer
struct timeval t1, t2;

char HOSTSTR[] = "http://news.sohu.com";
FILE *urlFile;
FILE *conFile;

int BUFFERSIZE = 8192;
int MAXURLNUM = 160000;
char HOST[] = "news.sohu.com";
char IP[] = "123.126.104.68";
int threadReturnCount = 0;
int THREADNUM = 4;
pthread_mutex_t lock;

typedef struct worker{
	void *(*process) (void *arg);
	void *arg;
	struct worker *next;
}CThread_worker;

typedef struct {
	pthread_mutex_t queue_lock;
	pthread_cond_t queue_ready;

	CThread_worker *queue_head;
	int shutdown;
	pthread_t *threadid;

	int max_thread_num;
	int cur_queue_size;
}CThread_pool;

CThread_pool *pool = NULL;
