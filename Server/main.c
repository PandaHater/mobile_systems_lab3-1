#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>

pthread_t thrs[10];
int thr_vacant[10];

struct thr_args{
	int p_sock;
	int index;
};

void * thr_func(void* arg){
	struct thr_args args;
	args.p_sock = ((struct thr_args *)arg)->p_sock;
	args.index = ((struct thr_args *)arg)->index;
	
	printf("sock: %d",args.p_sock);
	printf("\n");
	printf("index: %d", args.index);	
	thr_vacant[args.index] = 1;
	
}



int main(){
	int listener, sock;
	struct sockaddr_in addr;
	pthread_t pid;
	int i;

	for (i = 0; i<10; i++){
		thr_vacant[i] = 1;
	}	


	listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0)
	{
		perror("listener socket fail");
		exit(1);
	}


	addr.sin_family = AF_INET;
	addr.sin_port = htons(6001);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		exit(2);
	}

	listen(listener, 5);
	sock = 0;
	while(1)
	{
		sock = accept(listener, NULL, NULL);
		struct thr_args *tmp;
		tmp = malloc(sizeof(*tmp));
		tmp->p_sock = sock;
		
		i = 0;
		while(1){

			if (thr_vacant[i])
			{
				tmp->index = i;
				thr_vacant[i] = 0;
				break;
			}
			i = (i+1)%10;
			
		}
		pthread_create(&thrs[tmp->index], NULL, thr_func, (void*)tmp);
	}
}
