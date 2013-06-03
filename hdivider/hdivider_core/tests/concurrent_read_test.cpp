#include "hdivider_test.h"

void *worker_func2(void *a)
{
    worker_args2 *args = (worker_args2*)a;
    HdividerWorker *hdivider = args->hdivider;
    map<InputId, int > *input_data = args->input_data;
    map<int, int> *result = args->result;
    string worker_id = args->worker_id;

    while (!hdivider->isFinished())
    {
        vector<InputId> ids = hdivider->getInput(17, worker_id);
     
        if (ids.size()==0) break;
       
        for (int j = 0; j<ids.size(); j++)
        {
            int value = input_data->at(ids[j]);
            
            pthread_mutex_lock(&HdividerTests::result_lock);
            if (value%2==0)
            {
                hdivider->lockResult(2, worker_id);
                (*result)[2] += 2;
                hdivider->unlockResult(2, worker_id);
            }
            if (value%3==0)
            {
                hdivider->lockResult(3, worker_id);
                (*result)[3] += 3;
                hdivider->unlockResult(3, worker_id);
            }
            if (value%5==0)
            {
                hdivider->lockResult(5, worker_id);
                (*result)[5] += 5;
                hdivider->unlockResult(5, worker_id);
            }
            if (value%7==0)
            {
                hdivider->lockResult(7, worker_id);
                (*result)[7] += 7;
                hdivider->unlockResult(7, worker_id);
            }
            
            (*result)[9] += value;
            pthread_mutex_unlock(&HdividerTests::result_lock);
            
            hdivider->setHandled(ids[j]);
          
            
        }    
    }
    
    return 0;   
}

void HdividerTests::testConcurrentReadInput()
    {
        map<InputId, int > *input_data = new map<InputId, int >;
        map<int, int> *result = new map<int, int>;
        
        int first_summ = 0;
        int first_elem_summ = 0;
        for (int i = 0; i<100; i++)
        {
                input_data->insert(pair<InputId, int>(i , i));
                
                if (i%2==0)
                {
                    first_summ +=2;
                }
                if (i%3==0)
                {
                    first_summ +=3;
                }
                if (i%5==0)
                {
                    first_summ += 5;
                }
                if (i%7==0)
                {
                    first_summ += 7;
                }
                first_elem_summ += i;
        }
        
        (*result)[2] = (*result)[3] = (*result)[5] = (*result)[7] = (*result)[9] = 0;
        
        HdividerWatcher* watcher = new HdividerWatcher(new HdividerTestInputIdIt (input_data), \
                new HdividerTestStateAccessor());
        
        int nthreads = 1000;
        vector<pthread_t> ths;
        
        for (int i = 0; i<nthreads; i++)
        {
            pthread_t th;
            char bf[10];
            sprintf(bf, "%d", i);
            string worker_id( "worker"+string(bf));
            
            worker_args2 *args1 = new worker_args2(new HdividerTestWorker(watcher), \
                input_data, result, worker_id, 0);
            pthread_create(&th, NULL, worker_func2, (void*)args1);
            ths.push_back(th);
        }
     
        for (int i = 0; i<nthreads; i++)
        {
                pthread_join(ths[i], NULL);
        }
        
        map<int, int>::iterator it = result->begin();
 
        int summ = 0;
        
        summ += (*result)[2] + (*result)[3] + (*result)[5] +(*result)[7];
        
        
        TS_ASSERT(first_summ == summ);
        cout << "first_summ: " << first_summ << endl;
        cout << "summ: " << summ << endl;
        
        // read test
        TS_ASSERT((*result)[9] == first_elem_summ);
        cout << "first_summ: " << first_elem_summ << endl;
        cout << "summ: " << (*result)[9] << endl;
    }
