extern "C" {
#include "redismodule.h"

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
}

#include "depth.h"

static RedisModuleType *DepthType;
#define DEPTH_TYPE_ENCODING_VERSION 0

void *DepthTypeRdbLoad(RedisModuleIO *rdb, int encver) {

}

void DepthTypeRdbSave(RedisModuleIO *rdb, void *value) {

}

void DepthTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {

}

void DepthTypeDigest(RedisModuleDigest *md, void *value) {

}

void DepthTypeFree(void *value) {

}

int DepthTypeAdd_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

}

int DepthTypeFill_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

}

int DepthTypeClose_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

}

int DepthTypeGet_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

}

int DepthTypeSet_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

}

int DepthTypeNewchanges_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    // Register the module itself
    if (RedisModule_Init(ctx, "redepth", 1, REDISMODULE_APIVER_1) ==
        REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    RedisModuleTypeMethods tm = {
                                 .version = REDISMODULE_TYPE_METHOD_VERSION,
                                 .rdb_load = DepthTypeRdbLoad,
                                 .rdb_save = DepthTypeRdbSave,
                                 .aof_rewrite = DepthTypeAofRewrite,
                                 .free = DepthTypeFree,
                                 .digest = DepthTypeDigest,
    };

    DepthType = RedisModule_CreateDataType(ctx, "redepthqz", DEPTH_TYPE_ENCODING_VERSION, &tm);
    if (DepthType == NULL) return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "redepth.add", DepthTypeAdd_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.fill", DepthTypeFill_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.close", DepthTypeClose_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.get", DepthTypeGet_RedisCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.set", DepthTypeSet_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.newchanges", DepthTypeNewchanges_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;

    return REDISMODULE_OK;
}
