#include "hdivider_test.h"


void worker_func(void *a)
{
    worker_args *args = (worker_args*)a;
    HdividerWorker *hdivider = args->hdivider;
    map<InputId, int > *input_data = args->input_data;
    vector<int> *result = args->result;
   
    while (!hdivider->isFinished())
    {
        vector<int64_t> ids = hdivider->getInput(312, "worker1");
        
        for (int j = 0; j<ids.size(); j++)
        {
            map<int64_t, int >::iterator it = input_data->find(ids[j]);
             
            if (it==input_data->end())
            {
                cout << "input_data[" << ids[j] << "] not found";
            }
            else
            {
                result->push_back(2*it->second);
                hdivider->setHandled(ids[j]);
               
            }
        }
    }
}

void HdividerTests::testOneWorker()
    {
     // multiply on 2 all inputs and write to result. 1 worker   
        
        int first_summ;
        
        map<InputId, int > *input_data = new map<InputId, int > ;
        vector<int> *result = new vector<int>;
        first_summ = 0;
        for (int i = 0; i<1000; i++)
        {
                input_data->insert(pair<InputId, int>(i , i));
                first_summ += i;
        }
      
        HdividerWatcher* watcher = new HdividerWatcher(new HdividerTestInputIdIt (input_data), \
                new HdividerTestStateAccessor());
   
        
        worker_func((void*) (new worker_args(new HdividerTestWorker(watcher), input_data, result)));
       
        TS_ASSERT(result->size() == 1000);
        int summ = 0;
        for (int i = 0; i<result->size(); i++)
        {
            summ += result->at(i);
        }
        
        TS_ASSERT(2*first_summ == summ);
    }
