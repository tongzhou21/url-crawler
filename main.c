#include "thread.h"


//-levent -levent_core -lpthread
int main(){
	THREADNUM = 8;
	MAXURLNUM = 3000;
	char url[] = "http://news.sohu.com/guoneixinwen.shtml";
	gettimeofday(&t1, NULL);
	urlFile = fopen("url.txt", "w");
	conFile = fopen("con.txt", "w");

	fprintf(urlFile, "%d %s\n", 0, url);

	queue = (QueueType*)malloc(sizeof(QueueType));

	initQueue(queue);
	enQueue(queue,url);
	showQueue(queue);
	fclose(urlFile);
	fclose(conFile);
	writeIntoMap(url, 0);

	while(queue -> queueSize > 0 && ENDFLAG == 0){
		printf("runWithThread() main() call for >>>>>>>\n");
		runWithThread();
	}
	combineTwoFiles("url.txt","con.txt");
	return 0;

}
