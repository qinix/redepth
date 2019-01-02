extern "C" {
#include "redismodule.h"

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
}

#include "depth.h"

static RedisModuleType *DepthType;
#define DEPTH_TYPE_ENCODING_VERSION 0

Depth* DepthTypeOpenKey(RedisModuleCtx *ctx, RedisModuleString *keystr) {
    RedisModuleKey *key = (RedisModuleKey*)RedisModule_OpenKey(ctx,keystr,
                                                               REDISMODULE_READ|REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
    if (type != REDISMODULE_KEYTYPE_EMPTY &&
        RedisModule_ModuleTypeGetType(key) != DepthType) {
        RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
        return NULL;
    }

    Depth *depth;
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
        depth = new Depth();
        RedisModule_ModuleTypeSetValue(key, DepthType, depth);
    } else {
        depth = (Depth*)RedisModule_ModuleTypeGetValue(key);
    }

    return depth;
}

std::string NormalizeRedisString(const RedisModuleString *str) {
    size_t len;
    const char* rawstr = RedisModule_StringPtrLen(str, &len);
    return std::string(rawstr, len);
}

bool NormalizeRedisBoolean(const RedisModuleString *rawstr) {
    std::string str = NormalizeRedisString(rawstr);
    if (str == "0" || str == "FALSE" || str == "false") return false;
    return true;
}

bool CheckRedisStringSide(RedisModuleCtx *ctx, RedisModuleString *side) {
    if (RedisModule_StringCompare(side, RedisModule_CreateString(ctx, "BID", 3)) != 0 &&
        RedisModule_StringCompare(side, RedisModule_CreateString(ctx, "ASK", 3)) != 0) {
        RedisModule_ReplyWithError(ctx, "Wrong side, must be one of BID or ASK");
        return false;
    }

    return true;
}

bool RedisStringSideIsBid(const RedisModuleString *str) {
    return NormalizeRedisString(str) == "BID";
}

void *DepthTypeRdbLoad(RedisModuleIO *rdb, int encver) {
    RedisModuleString *redis_string = RedisModule_LoadString(rdb);
    return new Depth(NormalizeRedisString(redis_string));
}

void DepthTypeRdbSave(RedisModuleIO *rdb, void *value) {
    std::string str = static_cast<Depth*>(value)->to_protobuf();
    RedisModule_SaveStringBuffer(rdb, str.data(), str.size());
}

void DepthTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
    std::string str = static_cast<Depth*>(value)->to_protobuf();
    RedisModule_EmitAOF(aof, "REDEPTH.MERGE", "ss", key, str.c_str());
}

void DepthTypeFree(void *value) {
    delete static_cast<Depth*>(value);
}

// REDEPTH.ADD key price amount side -> 'OK'
int DepthTypeAdd_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 5) return RedisModule_WrongArity(ctx);
    if (!CheckRedisStringSide(ctx, argv[4])) return REDISMODULE_ERR;

    Decimal price(NormalizeRedisString(argv[2])), amount(NormalizeRedisString(argv[3]));
    bool is_bid = RedisStringSideIsBid(argv[4]);

    Depth *depth = DepthTypeOpenKey(ctx, argv[1]);
    if (depth == NULL) return REDISMODULE_ERR;

    depth->add(price, amount, is_bid);
    RedisModule_ReplyWithSimpleString(ctx, "OK");
    return REDISMODULE_OK;
}

// REDEPTH.FILL key price filled_amount side is_fully_filled
int DepthTypeFill_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 6) return RedisModule_WrongArity(ctx);
    if (!CheckRedisStringSide(ctx, argv[4])) return REDISMODULE_ERR;

    Decimal price(NormalizeRedisString(argv[2])), amount(NormalizeRedisString(argv[3]));
    bool is_bid = RedisStringSideIsBid(argv[4]), is_fully_filled = NormalizeRedisBoolean(argv[5]);

    Depth *depth = DepthTypeOpenKey(ctx, argv[1]);
    if (depth == NULL) return REDISMODULE_ERR;

    depth->fill(price, amount, is_bid, is_fully_filled);
    RedisModule_ReplyWithSimpleString(ctx, "OK");
    return REDISMODULE_OK;
}

// REDEPTH.CLOSE key price amount side
int DepthTypeClose_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 5) return RedisModule_WrongArity(ctx);
    if (!CheckRedisStringSide(ctx, argv[4])) return REDISMODULE_ERR;
    Decimal price(NormalizeRedisString(argv[2])), amount(NormalizeRedisString(argv[3]));
    bool is_bid = RedisStringSideIsBid(argv[4]);

    Depth *depth = DepthTypeOpenKey(ctx, argv[1]);
    if (depth == NULL) return REDISMODULE_ERR;

    depth->close(price, amount, is_bid);
    RedisModule_ReplyWithSimpleString(ctx, "OK");
    return REDISMODULE_OK;
}

// REDEPTH.GET key [limit]
int DepthTypeGet_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 2 && argc != 3) return RedisModule_WrongArity(ctx);
    long long limit = 0;

    if (argc == 3 && RedisModule_StringToLongLong(argv[2], &limit) == REDISMODULE_ERR) {
        RedisModule_ReplyWithError(ctx, "Wrong limit");
        return REDISMODULE_ERR;
    }

    Depth *depth = DepthTypeOpenKey(ctx, argv[1]);
    if (depth == NULL) return REDISMODULE_ERR;

    std::string pb = depth->to_protobuf(limit);
    RedisModule_ReplyWithStringBuffer(ctx, pb.data(), pb.size());
    return REDISMODULE_OK;
}

// REDEPTH.MERGE key protobuf_encoded_depth
int DepthTypeMerge_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 3) return RedisModule_WrongArity(ctx);

    Depth *depth = DepthTypeOpenKey(ctx, argv[1]);
    if (depth == NULL) return REDISMODULE_ERR;

    Depth new_depth(NormalizeRedisString(argv[2]));
    depth->merge(new_depth);
    RedisModule_ReplyWithSimpleString(ctx, "OK");
    return REDISMODULE_OK;
}

// REDEPTH.NEWCHANGES key
int DepthTypeNewchanges_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 2) return RedisModule_WrongArity(ctx);

    Depth *depth = DepthTypeOpenKey(ctx, argv[1]);
    if (depth == NULL) return REDISMODULE_ERR;

    std::string pb = depth->newchanges();
    RedisModule_ReplyWithStringBuffer(ctx, pb.data(), pb.size());
    return REDISMODULE_OK;
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
    };

    DepthType = RedisModule_CreateDataType(ctx, "redepthqz", DEPTH_TYPE_ENCODING_VERSION, &tm);
    if (DepthType == NULL) return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "redepth.add", DepthTypeAdd_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.fill", DepthTypeFill_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.close", DepthTypeClose_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.get", DepthTypeGet_RedisCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.merge", DepthTypeMerge_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "redepth.newchanges", DepthTypeNewchanges_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)  return REDISMODULE_ERR;

    return REDISMODULE_OK;
}
