#include "data_pool.h"

data_pool::data_pool(int _size):pool_size(_size),pool(_size)
{
	start = end = 0;
	sem_init(&product_sem, 0, _size);
	sem_init(&consumer_sem, 0, 0);
}

data_pool::~data_pool()
{
	sem_destroy(&product_sem);
	sem_destroy(&consumer_sem);
}

bool data_pool::get_msg(std::string &_msg)
{
	sem_wait(&consumer_sem);
	_msg = pool[end];
	sem_post(&product_sem);
	end = (++end)%pool_size;
}

bool data_pool::put_msg(const std::string &_msg)
{
	sem_wait(&product_sem);
	pool[start] = _msg;
	sem_post(&consumer_sem);
	start = (++start)%pool_size;
}

