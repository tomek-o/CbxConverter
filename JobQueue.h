//---------------------------------------------------------------------------

#ifndef JobQueueH
#define JobQueueH
//---------------------------------------------------------------------------

#include "ThreadWorker.h"
#include "Mutex.h"
#include <deque>

class Job;

class JobQueue
{
private:
	Mutex mutex;
	enum { WORKER_CNT = 16 };
	ThreadWorker* workers[WORKER_CNT];
	std::deque<Job> scheduledJobs;
public:
	JobQueue(void);
	~JobQueue(void);
	void Add(const Job &job);
	void Poll(void);
	void AbortAll(void);
	bool isEmpty(void) const;
};

extern JobQueue *jobQueue;

#endif
