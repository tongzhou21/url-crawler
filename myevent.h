#include "buffer.h"


typedef struct EVENT_ARG{

	char url[URLLENGTH];
	struct event *eventWrite;
	struct event *eventRead;

}event_arg;


void recevieCallBack(evutil_socket_t fd, short what, void *arg){
	//printf("receive\n");

	struct EVENT_ARG *eventArg = (struct EVENT_ARG*)arg;
	// printf("father url <%s>\n", eventArg -> url);

	pthread_mutex_lock(&lock);
	int fatherNum = getNumbyUrl(eventArg -> url);
	pthread_mutex_unlock(&lock);

	char text[BUFFERSIZE];

	int okFlag = 1;

	memset(text, 0x0, BUFFERSIZE);
	int contextLength = recv(fd, (void*)text, BUFFERSIZE * sizeof(char), 0);
	//printf("%s",text);
	if(contextLength <= 0){

		if(eventArg != NULL){
			event_free(eventArg -> eventRead);
			free(eventArg);
			eventArg = NULL;
		}

	}else{

		if(text[9] != '2'){

			if(eventArg != NULL){
				event_free(eventArg -> eventRead);
				free(eventArg);
				eventArg = NULL;
			}
			okFlag = 0;
		}

	}

	int retryCount = 0;
	while(okFlag){
		memset(text, 0x0, BUFFERSIZE);
		contextLength = recv(fd, (void*)text, BUFFERSIZE * sizeof(char), 0);

		if(contextLength == 0){

			if(eventArg != NULL){
				event_free(eventArg -> eventRead);
				free(eventArg);
				eventArg = NULL;
			}
			break;

		}else if(contextLength < 0){

			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){
				++retryCount;
				if(retryCount > 9000000){
					// printf("url <%s> caused too many retry, break!\n", eventArg -> url);
					
					if(eventArg != NULL){
						event_free(eventArg -> eventRead);
						free(eventArg);
						eventArg = NULL;
					}
					break;
				}

				continue;

			}else{

				if(eventArg != NULL){
					event_free(eventArg -> eventRead);
					free(eventArg);
					eventArg = NULL;
				}
				break;
			}
			

		}else{
			retryCount = 0;
			//printf("pass to bufferMain");

			bufferMain(text, fatherNum);

		}

	}

	close(fd);
	//printf("recv return \n");
	return;

}

void sendCallBack(evutil_socket_t fd, short what, void *arg){
	//printf("send\n");

	struct EVENT_ARG *eventArg = (struct EVENT_ARG*)arg;
	//http://news.sohu.com/20170315/n483393163.shtml
	//0123456789abcdefghijk
	//only for news.sohu.com
	if((eventArg -> url)[20] == '\0'){
		strcat(eventArg -> url, "/");
	}

	char request[BUFFERSIZE];

	memset(request, 0x0, BUFFERSIZE);
	strcat(request, "GET ");
	strcat(request, &((eventArg -> url)[20]));
	strcat(request, " HTTP/1.1\r\n");
	strcat(request, "Accept: */*\r\n");
	strcat(request, "Accept-Language: cn\r\n");
	strcat(request, "User-Agent: Mozilla/4.0\r\n");
	strcat(request, "HOST: ");
	strcat(request, HOST);
	strcat(request, "\r\n");
	strcat(request, "Cache-Control: no-cache\r\n");
	strcat(request, "Connection: Keep-Alive\r\n");
	strcat(request,"\r\n\r\n");
	//printf("request:\n%s\n", request);

	int sendReturn = send(fd, (void*)request, strlen(request), 0);

	if(sendReturn < 0){
		close(fd);
	}

	return;

}

void addEvent(){

	struct event_base *base = event_base_new();
	int socketfdCount = 0;

	while(socketfdCount < 100){
		char *url = NULL;
		pthread_mutex_lock(&lock);
		
		int queueSize = queue -> queueSize;
		if(queueSize > 0){
			url = deQueue(queue);
			pthread_mutex_unlock(&lock);
		}else{
			if(socketfdCount != 0){
				pthread_mutex_unlock(&lock);
				break;
			}
			if(urlNum >= MAXURLNUM){
				ENDFLAG = 1;
				printf("ENDFLAG == 1!!!!!!!!!!!!!!!!!!!!!!!!\n");
				pthread_mutex_unlock(&lock);
				return;
			}
			
			pthread_mutex_unlock(&lock);
			srand((unsigned)time(NULL));
			double interval = (double)(rand() % 10) / 100.1;
			sleep(interval);
			continue;
		}
		//printf("dedaoziyuan%s\n",url);
		evutil_socket_t socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		struct sockaddr_in addr;
		struct event *eventWrite = NULL;
		struct event *eventRead = NULL;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(80);
		addr.sin_addr.s_addr = inet_addr(IP);

		int connectReturn = connect(socketfd, (struct sockaddr*)&addr, sizeof(addr));
		///////?
		if(connectReturn < 0 && connectReturn != EINPROGRESS){
			free(url);
			url = NULL;
			//printf("lianjiecuowu\n");
			continue;

		}else if(connectReturn == 0){
			++socketfdCount;
		}

		evutil_make_socket_nonblocking(socketfd);

		struct EVENT_ARG* eventArg = (struct EVENT_ARG*)malloc(sizeof(struct EVENT_ARG));

		eventWrite = event_new(base, socketfd, EV_WRITE, sendCallBack, (void*)eventArg);
		eventRead = event_new(base, socketfd, EV_READ | EV_PERSIST, recevieCallBack, (void*)eventArg);
		
		eventArg -> eventWrite = eventWrite;
		eventArg -> eventRead = eventRead;

		strcpy(eventArg -> url, url);
		/*int i;
		for(i = 0; url[i] != '\0'; ++i){
			(eventArg -> url)[i] = url[i];
		}
		(eventArg -> url)[i] = '\0';*/


		event_add(eventWrite, 0);
		event_add(eventRead, 0);
		free(url);
		url = NULL;
		//printf("myevent while\n");
		
	}
	
	event_base_dispatch(base);
	event_base_free(base);

	return ;
}
