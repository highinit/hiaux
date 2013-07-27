/* 
 * File:   hdivider_worker.h
 * Author: phrk
 *
 * Created on April 20, 2013, 12:49 AM
 */

#ifndef HDIVIDER_WORKER_H
#define	HDIVIDER_WORKER_H

#include <string>
#include <vector>

#include "hdivider_watcher.h"

#define InputId int64_t

using namespace std;

class HdividerWorker
{
public:
        
    virtual vector<InputId> getInput(int count, string worker_id) = 0;
    virtual int isFinished() = 0;

    virtual void setHandled(InputId input_id) = 0;
    virtual int isHandled(InputId input_id) = 0;
    virtual int setHandledIfNot(int64_t input_id) = 0;
    virtual void lockResult(InputId result_id, string worker_id) = 0;
    virtual void unlockResult(InputId result_id, string worker_id) = 0;
    
    virtual void fault(string worker_id) = 0;
};


#endif	/* HDIVIDER_WORKER_H */

