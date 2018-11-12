#include "server_main.h"
#include "udp_server.h"
#include "udp_log.h"

static udp_server serd;

void *consumer_run(void *arg)
{
	for( ; ; ){
		print_log("consumer run....");
		serd.broadcast_msg();
	}
}

void *producter_run(void *arg)
{
	for( ; ; ){
		print_log("product run....");
		serd.recv_msg();
	}
	return NULL;
}

int main()
{
	daemon(0, 0);
	serd.init();
	pthread_t consumer, producter;
	pthread_create(&consumer, NULL, consumer_run, NULL);
	pthread_create(&producter, NULL, producter_run, NULL);
	pthread_join(consumer, NULL);
	pthread_join(producter, NULL);
	return 0;
}
