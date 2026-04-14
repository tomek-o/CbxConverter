//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ThreadWorker.h"
#include "Job.h"
#include "Log.h"
#include "ScopedLock.h"
#include <assert.h>

#pragma package(smart_init)

namespace {

enum { THREAD_DELAY = 200 };

}
__fastcall ThreadWorker::ThreadWorker(int id)
	: TThread(false),
	id(id),
	job(NULL)
{
	state = IDLE;
}
//---------------------------------------------------------------------------
void __fastcall ThreadWorker::Execute()
{
	while (!Terminated)
	{
		//LOG("ThreadWorker %p: state = %d", this, static_cast<int>(state));
		if (abort)
		{
			state = STOPPED;
			break;
		}

		switch (state)
		{
		case IDLE:
		case DONE:
		case STOPPED:
		default:
			Sleep(THREAD_DELAY);
			break;
		case RUNNING:
			{
				ScopedLock<Mutex> lock(mutex);
				//LOG("ThreadWorker %p: RUNNING", this);
				LOG("ThreadWorker %d: Executing job type = %d (%s)", id, static_cast<int>(job.type), job.GetTypeName());
				job.Execute(&abort, priority);
				LOG("ThreadWorker %d: Done job type = %d (%s)", id, static_cast<int>(job.type), job.GetTypeName());
//						PlaySound("SystemAsterisk", NULL, SND_ASYNC);
				state = DONE;
			}

			break;
		}
		if (abort)
		{
			state = IDLE;
			break;
		}
	}
}
//---------------------------------------------------------------------------

void ThreadWorker::AddJob(const Job& job)
{
	ScopedLock<Mutex> lock(mutex);
	assert(state == IDLE || state == DONE);
	//LOG("ThreadWorker %p: adding job %p", this, job);
	this->job = job;
	state = RUNNING;
}
