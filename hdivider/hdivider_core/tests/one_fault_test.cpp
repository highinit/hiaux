#include "hdivider_test.h"

void HdividerTests::testOneFault()
    {
         map<InputId, int > *input_data = new map<InputId, int >;
        map<int, int> *result = new map<int, int>;
        
        int first_summ = 0;
        int first_elem_summ = 0;
        for (int i = 0; i<100000; i++)
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
        
        pthread_t th;
        // thread to fault
        worker_args2 *args1 = new worker_args2(new HdividerTestWorker(watcher), \
                input_data, result, "worker_will_fault", 1);
            pthread_create(&th, NULL, worker_func3, (void*)args1);
            ths.push_back(th);
        
        for (int i = 1; i<nthreads; i++)
        {
            pthread_t th;
            char bf[10];
            strcpy(bf, "");
            sprintf(bf, "%d", i);
            string worker_id( "worker"+string(bf));
            
            worker_args2 *args2 = new worker_args2(new HdividerTestWorker(watcher), \
                input_data, result, worker_id, 0);
            pthread_create(&th, NULL, worker_func3, (void*)args2);
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
