//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "JobQueue.h"
#include "Log.h"
#include "ScopedLock.h"
#include "Settings.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

JobQueue *jobQueue = NULL;

JobQueue::JobQueue(void)
{
	for (unsigned int i=0; i<sizeof(workers)/sizeof(workers[0]); i++) {
		workers[i] = new ThreadWorker(i);
	}
}

JobQueue::~JobQueue(void)
{
	for (unsigned int i=0; i<sizeof(workers)/sizeof(workers[0]); i++)
	{
		if (workers[i])
		{
			workers[i]->Abort();
		}
	}
	Sleep(500);
	for (unsigned int i=0; i<sizeof(workers)/sizeof(workers[0]); i++)
	{
		if (workers[i])
		{
			delete workers[i];
		}
	}
	//Sleep(300);
}

void JobQueue::Add(const Job &job)
{
	ScopedLock<Mutex> lock(mutex);
    LOG("JobQueue: adding job, type = %d (%s), filename [%s]", static_cast<int>(job.type), job.GetTypeName(), job.fileName.c_str());
	scheduledJobs.push_back(job);
}

void JobQueue::Poll(void)
{
	ScopedLock<Mutex> lock(mutex);
	while (!scheduledJobs.empty())
	{
        //LOG("JobQueue: scheduled list non-empty");
		// find idle worker thread
		bool idleFound = false;
		for (int i=0; i<static_cast<int>(sizeof(workers)/sizeof(workers[0])); i++)
		{
			if (i >= appSettings.Worker.threadCount)
				break;
			ThreadWorker* worker = workers[i];
			if (worker->isIdle())
			{
				Job job = scheduledJobs.front();
				LOG("JobQueue: scheduled job type %s -> worker #%d", job.GetTypeName(), worker->getId());
                scheduledJobs.pop_front();
				worker->AddJob(job);
				idleFound = true;
				break;				
			}
		}
		if (!idleFound)
		{
            break;
        }
	}
}

void JobQueue::AbortAll(void)
{
	for (unsigned int i=0; i<sizeof(workers)/sizeof(workers[0]); i++)
	{
		ThreadWorker* worker = workers[i];
		worker->Abort();
	}

	for(;;)
	{
		bool allIdle = true;
		Application->ProcessMessages();
		for (unsigned int i=0; i<sizeof(workers)/sizeof(workers[0]); i++)
		{
			ThreadWorker* worker = workers[i];
			if (worker->isIdle() == false)
			{
				allIdle = false;
				break;
			}
		}
		if (allIdle)
			break;
		Sleep(50);
	}
}

bool JobQueue::isEmpty(void) const
{
	if (!scheduledJobs.empty())
		return false;
	for (unsigned int i=0; i<sizeof(workers)/sizeof(workers[0]); i++)
	{
		ThreadWorker* worker = workers[i];
		if (worker->isIdle() == false)
			return false;
	}
	return true;
}

