#ifndef MRNODE_DISPATCHER
#define MRNODE_DISPATCHER

class MRNodeDispatcher
{
	boost::shared_ptr<MRBatchDispatcher> batch_dispatcher;
public:
	
	void onBatchFinished(std::unordered_map<int64_t, EmitAcessorVecPtr> inter_result);
	
};

#endif