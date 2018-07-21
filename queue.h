#include "global.h"

typedef struct QueueNode{
	char url[URLLENGTH];
	struct QueueNode *next;

}QueueNodeType;

typedef struct Queue{
	int queueSize;
	QueueNodeType *head;
	QueueNodeType *rear;

}QueueType;


QueueType *queue = NULL;
int initQueue(QueueType *queue){

	if(queue == NULL){

		struct Queue *newQueue = NULL;

		newQueue = (QueueType*)malloc(sizeof(QueueType));
		newQueue -> queueSize = 0;
		newQueue -> head = NULL;
		newQueue -> rear = NULL;

		queue = newQueue;

	}else{

		queue -> queueSize = 0;
		queue -> head = NULL;
		queue -> rear = NULL;
	}

	return 1;
}

int enQueue(QueueType *queue, char *url){

	if(queue == NULL || url == NULL){
		return -1;
	}

	QueueNodeType *newNode = (QueueNodeType*)malloc(sizeof(QueueNodeType));
	strcpy(newNode -> url, url);
	newNode -> next = NULL;
	queue -> queueSize += 1;

	if(queue -> head == NULL){

		queue -> head = newNode;
		queue -> rear = newNode;
		return 1;
	}
	  
	queue -> rear -> next = newNode;
	queue -> rear = newNode;
	return 1;

}

char *deQueue(QueueType *queue){

	if(queue == NULL || queue -> head == NULL){
		return NULL;
	}
	QueueNodeType *targetNode = queue -> head;
	queue -> head = queue -> head -> next;

	char *returnUrl = (char*)malloc(sizeof(char) * URLLENGTH);
	strcpy(returnUrl, targetNode -> url);
	free(targetNode);

	queue -> queueSize -= 1;
	return returnUrl;
}

int emptyQueue(QueueType *queue){
	if(queue == NULL || queue -> head == NULL){
		return -1;
	}
	while(queue -> queueSize > 0){
		deQueue(queue);
	}
	return 1;
}

int showQueue(QueueType *queue){
	printf("showQueue{\n");
	if(queue == NULL || queue -> head == NULL){
		return -1;
	}
	QueueNodeType *pNode = queue -> head;
	while(pNode != NULL){
		printf("%s\n",pNode -> url);
		pNode = pNode -> next;
	}
	printf("}\n");
	return 1;
}
