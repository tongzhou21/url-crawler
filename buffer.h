
#include "map.h"

int hostFilter(char *url, int length){
	int i = 0;
	if(length < strlen(HOSTSTR)){
		return 0;
	}

	while(i < strlen(HOSTSTR)){
		if(url[i] != HOSTSTR[i]){
			return 0;
		}
		++i;
	}
	return 1;

}

int suffixFilter(char *url, int end){
	int j = end;
	if(url[j] == '/'){
		--j;
	}
	int i = j;
	while(i >= 0 && url[i] != '.'){
		--i;
	}
	++i;
	if(j - i > 3){
		return 1;
	}
	char suffix[5];
	for(int temp = i; temp <= j; ++temp){
		suffix[temp - i] = url[temp];
	}
	if(j - i + 1 >= 0){
		suffix[j - i + 1] = '\0';
	}else{
		return 0;
	}

	if(strcmp(suffix,"jpg") == 0 || strcmp(suffix,"png") == 0 || strcmp(suffix,"js") == 0 || strcmp(suffix,"gif") == 0 || strcmp(suffix,"ts") == 0){
		return 0;
	}
	return 1;
}

char *subString(char *str, int low, int high){

	char *s = (char*)malloc(sizeof(char) * (high - low + 1));
	int i = low;
	int j = 0;

	while(i < high){
		s[j++] = str[i++];
	}
	s[j] = '\0';

	return s;
}
int combineTwoFiles(char * urlsfile, char * consfile){
    urlFile = fopen(urlsfile, "a+");
    conFile = fopen(consfile,"r");
    char buf[500];
    fprintf(urlFile, "\n" );
    while(fgets(buf, 500, conFile) != NULL){
        // printf("%s\n",buf );
        fprintf(urlFile, "%s", buf);
    }
    fclose(urlFile);
    fclose(conFile);
}
int bufferMain(char *text, int fatherNum){
	//printf("%s\n", text);

	regmatch_t pm[4];
	regex_t preg;
	char *pText = text;
	char pattern[] = "(<[Aa].{1,20}?href\\ *=\\ *\\\")(http[^\\\"]*)(\\\")";
	
	if(regcomp(&preg, pattern, REG_EXTENDED | REG_NEWLINE) != 0){
		return -1;
	}

	while(pText && regexec(&preg, pText, 4, pm, REG_NOTEOL) != REG_NOMATCH){
		//printf("match\n");

		if(pm[2].rm_eo - pm[2].rm_so < URLLENGTH){
			char *url = subString(pText, pm[2].rm_so, pm[2].rm_eo);
			//printf("filter%s,h = %d,s = %d\n",url,hostFilter(url, strlen(url)),suffixFilter(url,strlen(url) -1));

			if(hostFilter(url, strlen(url)) == 1 && suffixFilter(url, strlen(url) - 1) == 1){

				pthread_mutex_lock(&lock);
				int sonNum = getNumbyUrl(url);
				pthread_mutex_unlock(&lock);

				pthread_mutex_lock(&lock);
				urlFile = fopen("url.txt", "a+");
				conFile = fopen("con.txt", "a+");
				//printf("son = %d\n", sonNum);
				if(sonNum == -1){
					if(urlNum < MAXURLNUM){

						fprintf(urlFile, "%d %s\n", urlNum, url);
						fprintf(conFile, "%d %d\n", fatherNum, urlNum);
						printf("URLNUM(%d), fathreNum(%d), url = %s\n", urlNum, fatherNum, url);
						
						enQueue(queue, url);
						writeIntoMap(url, urlNum);
						++urlNum;

						
					}

				}else{
					printf("URLNUM(%d), fathreNum(%d), url = %s\n", sonNum, fatherNum, url);
					fprintf(conFile, "%d %d\n", fatherNum, sonNum);
				}
				gettimeofday(&t2, NULL);
						// diff-second
				double time_diff_sec = (t2.tv_sec-t1.tv_sec) + (t2.tv_usec-t1.tv_usec)/1000000;
				printf("Time Passed --- %d\n",(int)time_diff_sec );
				fclose(urlFile);
				fclose(conFile);
				pthread_mutex_unlock(&lock);

			}
			free(url);

		}

		pText = &pText[pm[3].rm_eo];

	}
	regfree(&preg);

	return 1;
}
