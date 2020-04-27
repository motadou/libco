/*
* Tencent is pleased to support the open source community by making Libco available.

* Copyright (C) 2014 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, 
* software distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include "co_routine.h"
using namespace std;
struct stTask_t
{
	int id;
};
struct stEnv_t
{
	stCoCond_t* cond;
	queue<stTask_t*> task_queue;
};

void* Producer(void* args)
{
    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	co_enable_hook_sys();
	stEnv_t* env=  (stEnv_t*)args;
	int id = 0;
	while (true)
	{
		stTask_t* task = (stTask_t*)calloc(1, sizeof(stTask_t));
		task->id = id++;
		env->task_queue.push(task);
		printf("%s:%d produce task %d >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", __func__, __LINE__, task->id);
		co_cond_signal(env->cond);
		poll(NULL, 0, 1000*10);
	}
	return NULL;
}

void* Consumer(void* args)
{
    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	co_enable_hook_sys();
	stEnv_t* env = (stEnv_t*)args;
	while (true)
	{
        printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		if (env->task_queue.empty())
		{
            printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			co_cond_timedwait(env->cond, -1);
			continue;
		}

        printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		stTask_t* task = env->task_queue.front();
		env->task_queue.pop();
		printf("%s:%d consume task %d\n", __func__, __LINE__, task->id);
		free(task);
	}
	return NULL;
}


#include <sys/socket.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/un.h>

#include <dlfcn.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <pthread.h>

#include <resolv.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    struct in_addr in;

    struct sockaddr_in local_addr;


    printf(">>>111111111111111111111111\n");

    struct hostent * aa = gethostbyname("www.baidu.com");

    memcpy(&local_addr.sin_addr.s_addr, aa->h_addr, 4);

    in.s_addr = local_addr.sin_addr.s_addr;

    printf(">>>:%s\n", inet_ntoa(in));

	stEnv_t* env = new stEnv_t();
	env->cond = co_cond_alloc();

	stCoRoutine_t* consumer_routine;

    co_create(&consumer_routine, NULL, Consumer, env);

    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	co_resume( consumer_routine);

	stCoRoutine_t* producer_routine;
	co_create(&producer_routine, NULL, Producer, env);

    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	co_resume( producer_routine);

    printf("\n\n\n\n\n%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	co_eventloop(co_get_epoll_ct(), NULL, NULL);

	return 0;
}
