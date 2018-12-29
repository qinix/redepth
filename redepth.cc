extern "C" {
#include "redismodule.h"

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
}

static RedisModuleType *DepthType;
#define DEPTH_TYPE_ENCODING_VERSION 0

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    // Register the module itself
    if (RedisModule_Init(ctx, "redepth", 1, REDISMODULE_APIVER_1) ==
        REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    return REDISMODULE_OK;
}
