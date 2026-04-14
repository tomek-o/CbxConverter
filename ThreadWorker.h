//---------------------------------------------------------------------------

#ifndef ThreadWorkerH
#define ThreadWorkerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------

#include "Job.h"
#include "Mutex.h"
#include <deque>

class Job;

class ThreadWorker : public TThread
{
public:
	enum State
	{
		IDLE = 0,
		RUNNING,
		DONE,
		STOPPED
	};
private:
protected:
	void __fastcall Execute();
	const int id;
	Job job;
	Mutex mutex;
	volatile enum State state;
	bool abort;
	int priority;
public:
	__fastcall ThreadWorker(int id);
	void AddJob(const Job& job);
	enum State GetState(void) {
    	return state;
	}
	void Abort(void) {
		abort = true;
	}
	bool isIdle(void) {
        return (state != RUNNING);
	}
	int getId(void) const {
    	return id;
	}
};
//---------------------------------------------------------------------------
#endif
