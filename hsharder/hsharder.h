#include <vector>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <map>

template <class ShardInfo, class KeyType>
class hSharder
{
    std::map<int, ShardInfo> virtual_shards;
    boost::function<int(KeyType)> hash_f;
public:
     
    hSharder(int nvirtual_shards, boost::function<int(KeyType)> hash_f)
    {
        this->hash_f = hash_f;
    }
    
    void assignShardMapping(int virtual_shard, ShardInfo real_shard)
    {
        virtual_shards[virtual_shard] = real_shard;
    }
    
    void assignShardMapping(std::pair<int,int> virtual_shard_range, ShardInfo real_shard)
    {
        for (int i = virtual_shard_range.first; i<virtual_shard_range.second; i++)
        virtual_shards[i] = real_shard;
    }
    
    ShardInfo getRealShard(KeyType &key)
    {
        return virtual_shards[ hash_f(key) % virtual_shards.size() ];
    }
    
};
