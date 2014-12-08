/*
 * Job.cpp
 *
 *  Created on: 6 déc. 2014
 *      Author: jfellus
 */

#include "Job.h"
#include <semaphore.h>

namespace libboiboites {

std::list<Job*> Job::jobs;
Job* Job::cur;

static sem_t sem;
static pthread_t th_jobs_thread;
static void* jobs_thread(void*) {
	while(true) {
		while(!Job::has_job()) sem_wait(&sem);
		Job::start();
		Job::cur->run();
		Job::end();
	}
	return 0;
}

void Job::init() {
	sem_init(&sem, 0,0);
	pthread_create(&th_jobs_thread, NULL, jobs_thread, NULL);
}

void Job::add(Job* j) {
	jobs.push_back(j);
	sem_post(&sem);
}

}
