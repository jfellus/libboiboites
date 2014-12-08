/*
 * Job.h
 *
 *  Created on: 6 déc. 2014
 *      Author: jfellus
 */

#ifndef JOB_H_
#define JOB_H_

#include <util/utils.h>

namespace libboiboites {

#define JOB_PROGRESS(pct) Job::progress(pct)
#define JOB_SUBMIT(name, func) do { 	\
		class jjjj : public Job {		\
			public:						\
			jjjj(const std::string& z) : Job(z) {}			\
			virtual ~jjjj() {}			\
			virtual void run() { 		\
				func;					\
			}							\
		}; 								\
		Job::add(new jjjj(name));		\
		} while(0);


class Job {
public:
	static std::list<Job*> jobs;
	static Job* cur;

	int _progress = 0;

	std::string name;

public:
	Job(const std::string& jobname) {
		name = jobname;
	}
	virtual ~Job() {}

	virtual void run() = 0;


	static void init();

	static bool has_job() {
		return !jobs.empty();
	}

	static void end() {
		delete cur;
	}

	static void start() {
		cur = jobs.front();
		jobs.pop_front();
		cur->_progress = 0;
	}

	static void progress(int pct) {
		cur->_progress = pct;
	}

	static void add(Job* j);

};

}

#endif /* JOB_H_ */
