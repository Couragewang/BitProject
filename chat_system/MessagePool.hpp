#ifndef _DATA_POOL_H_
#define _DATA_POOL_H_

#include <iostream>
#include <string>
#include <vector>
#include <semaphore.h>

#define MESSAGE_POOL_SIZE 512

class MessagePool{
	private:
		const int pool_size;
		std::vector<std::string> pool;
		sem_t product_sem;
		sem_t consume_sem;

		int p_step;
		int c_step;
	public:
        MessagePool(int size_ = MESSAGE_POOL_SIZE):pool_size(size_),pool(size_)
        {
        	p_step = c_step = 0;
        	sem_init(&product_sem, 0, size_);
        	sem_init(&consume_sem, 0, 0);
        }
        bool GetMessage(std::string &message_)
        {
        	sem_wait(&consume_sem);
        	message_ = pool[c_step];
        	sem_post(&product_sem);
            c_step++;
        	c_step %= pool_size;
        }
        
        bool PutMessage(const std::string &message_)
        {
        	sem_wait(&product_sem);
        	pool[p_step] = message_;
        	sem_post(&consume_sem);
            p_step++;
        	p_step %= pool_size;
        }
        ~MessagePool()
        {
        	sem_destroy(&product_sem);
        	sem_destroy(&consume_sem);
        }
};

#endif

