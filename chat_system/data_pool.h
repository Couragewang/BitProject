#ifndef _DATA_POOL_H_
#define _DATA_POOL_H_

#include <iostream>
#include <semaphore.h>
#include <string>
#include <vector>

#define POOL_SIZE 512
class data_pool{
	public:
		data_pool(int _size=POOL_SIZE);
		~data_pool();
        bool get_msg(std::string &_msg);
        bool put_msg(const std::string &_msg);
	private:
		const int pool_size;
		std::vector<std::string> pool;
		sem_t product_sem;
		sem_t consumer_sem;

		int start;
		int end;
};
#endif
