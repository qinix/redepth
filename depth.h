extern "C" {
#include "redismodule.h"
}

#include <vector>
#include <string>
#include <map>
#include <set>
#include <ctime>
#include "decimal.h"
#include "price_level.h"
#include "depth.pb.h"
#include "boost/date_time/posix_time/posix_time.hpp"

class Depth {
public:
    typedef std::map<Decimal, PriceLevelPtr, std::less<Decimal>> BidLevelMap;
    typedef std::map<Decimal, PriceLevelPtr, std::greater<Decimal>> AskLevelMap;

    static Depth *RdbLoad(RedisModuleIO *rdb, int encver);
    void rdb_save(RedisModuleIO *rdb);
    void aof_rewrite(RedisModuleIO *aof, RedisModuleString *key);
    void digest(RedisModuleDigest *md);

    int redis_add(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
    int redis_fill(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
    int redis_close(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
    int redis_get(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
    int redis_set(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
    int redis_newchanges(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

private:
    BidLevelMap bids_;
    AskLevelMap asks_;
    std::set<PriceLevelPtr> changes_;
    boost::posix_time::ptime updated_at_ = boost::posix_time::microsec_clock::universal_time();

    void serialize_to_protobuf(redepth::Depth& pb);
};
