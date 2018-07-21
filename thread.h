
#include "myevent.h"
void *thread_routine (void *arg);
void pool_init(int max_thread_num){

	pool = (CThread_pool*)malloc(sizeof(CThread_pool));

	//pthread_mutex_init(&(pool -> queue_lock), NULL);
	pthread_mutex_init(&lock, NULL);

	pthread_cond_init(&(pool -> queue_ready), NULL);

	pool -> queue_head = NULL;
	pool -> max_thread_num = max_thread_num;
	pool -> cur_queue_size = 0;
	pool -> shutdown = 0;
	pool -> threadid = (pthread_t*)malloc(sizeof(pthread_t) * max_thread_num);

	for(int i = 0;i < max_thread_num; ++i){
		pthread_create(&(pool -> threadid[i]), NULL, thread_routine, NULL);
	}
}

int pool_add_worker(void *(*process) (void *arg), void *arg){

	CThread_worker *newworker = (CThread_worker *)malloc(sizeof(CThread_worker));

	newworker -> process = process;
	newworker -> arg = arg;
	newworker -> next = NULL;

	pthread_mutex_lock(&lock);
	CThread_worker *member = pool -> queue_head;

	if(member != NULL){
		while(member -> next != NULL){
			member = member -> next;
		}
			
		member -> next = newworker;
	}else{
		pool -> queue_head = newworker;
	}
	assert(pool -> queue_head != NULL);
	pool -> cur_queue_size++;
	pthread_mutex_unlock(&lock);

	pthread_cond_signal(&(pool -> queue_ready));


	return 0;
}


int pool_destroy(){
	// printf("pool_destroy1");
	if(pool -> shutdown){
		return -1;
	}
	pool -> shutdown = 1;
	// printf("pool_destroy2");
	pthread_cond_broadcast(&(pool -> queue_ready));
	for(int i = 0; i < pool -> max_thread_num; ++i){
		pthread_join(pool -> threadid[i], NULL);
	}
	free(pool -> threadid);
	// printf("pool_destroy3");
	CThread_worker *head = NULL;
	while(pool -> queue_head != NULL){
		head = pool -> queue_head;
		pool -> queue_head = pool -> queue_head -> next;
		free(head);
	}
	// printf("pool_destroy4\n");
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&(pool -> queue_ready));

	free(pool);
	pool = NULL;
	// printf("pool_destory return\n");

	return 0;
}

void *thread_routine(void *arg){
	printf("starting the thread%lx\n", pthread_self());

	while(1){
		pthread_mutex_lock(&lock);
		
		while(pool -> cur_queue_size == 0 && pool -> shutdown != 1){
			printf("thread%lx is waiting, threadReturnCount = %d\n",pthread_self(), threadReturnCount);
			pthread_cond_wait(&(pool -> queue_ready), &lock);

		}
		if(pool -> shutdown){

			pthread_mutex_unlock(&lock);
			printf("thread%lx will exit1\n", pthread_self());
			pthread_exit(NULL);
			printf("thread%lx will exit\n", pthread_self());


		}


		printf("thread%lx is starting to work\n", pthread_self());

		assert(pool -> cur_queue_size != 0);
		assert(pool -> queue_head != NULL);

		pool -> cur_queue_size--;
		CThread_worker *worker = pool -> queue_head;
		pool -> queue_head = worker -> next;
		pthread_mutex_unlock(&lock);
		(*(worker -> process)) (worker -> arg);



		free(worker);
		worker = NULL;
	}
	pthread_exit(NULL);


}

void *myprocess(void *arg){
	printf ("threadid is 0x%lx, working on task %d\n", pthread_self (),*(int *) arg);

	int queueSize = queue -> queueSize;
	while(ENDFLAG == 0 && queueSize > 0){
		//printf("myprocess while\n");
		srand((unsigned)time(NULL));
		double interval = (double)(rand() % 10) / 1000.1;
		sleep(interval);

		addEvent();

	}

	++threadReturnCount;
	return NULL;
}

void runWithThread(){

    pool_init(THREADNUM);
    int *workingnum = (int*)malloc(sizeof(int) * THREADNUM);
    for(int i = 0; i < THREADNUM; ++i){
        workingnum[i] = i;
        pool_add_worker(myprocess, &workingnum[i]);
    }

    /*

    */
    // printf("runWithThread begin!\n");
    threadReturnCount = 0;
    while(threadReturnCount < THREADNUM){
        srand((unsigned)time(NULL));
		double interval = (double)(rand() % 10) / 200.1;
		sleep(interval);
    }
    // printf("runWithThread end run ::::::::::::");
    pool_destroy();
    free(workingnum);
    // printf("runWithThread end return ::::::::::::");

    return ;
}