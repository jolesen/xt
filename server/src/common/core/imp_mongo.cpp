#include "sconfig.h"
#include "mongo.h"
#include "log.h"

CMongo::CMongo()
{
}

CMongo::~CMongo()
{
}

bool CMongo::Ping()
{
    const SHost &h = theServerConfig.mongoHost;
    char host[64] = { 0 };
    snprintf(host, sizeof(host), "mongodb://%s:%u", h.ip.c_str(), h.port);
    mongoc_init();
    mongoc_client_t *client = mongoc_client_new(host);
    if(!client)
    {
        LOG_ERROR("can't new mongo client by host %s", host);
        return false;
    }

    bson_t ping;
    bson_init(&ping);
    bson_append_int32(&ping, "ping", 4, 1);

    const bson_t *reply;
    bson_error_t error;
    mongoc_database_t *database = mongoc_client_get_database(client, "test");
    mongoc_cursor_t *cursor = mongoc_database_command(database, (mongoc_query_flags_t)0, 0, 1, 0, &ping, NULL, NULL);
    if(mongoc_cursor_next(cursor, &reply))
    {
        char *str = bson_as_json(reply, NULL);
        LOG_ERROR("cursor: %s", str);
        bson_free(str);
    }
    else if(mongoc_cursor_error(cursor, &error))
    {
        LOG_ERROR("Ping failure: %s", error.message);
        return false;
    }

    mongoc_cursor_destroy(cursor);
    bson_destroy(&ping);
    mongoc_client_destroy(client);

    return true;
}

uint CMongo::Query(StringList &listRet, const std::string &db, const std::string &coll)
{
    listRet.clear();

    const SHost &h = theServerConfig.mongoHost;
    char host[64] = { 0 };
    snprintf(host, sizeof(host), "mongodb://%s:%u", h.ip.c_str(), h.port);
    mongoc_init();
    mongoc_client_t *client = mongoc_client_new(host);
    if(!client)
    {
        return 1;
    }

    // query
    bson_t query;
    bson_init(&query);
    mongoc_collection_t *collection = mongoc_client_get_collection(client, db.c_str(), coll.c_str());
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(collection, &query, NULL, NULL);
    const bson_t *doc;
    while(mongoc_cursor_next(cursor, &doc))
    {
        char *str = bson_as_json(doc, NULL);
        listRet.push_back(str);
        bson_free(str);
    }

    bson_error_t error;
    if(mongoc_cursor_error(cursor, &error))
    {
        LOG_ERROR("Cursor Failure: %s", error.message);
        return 111;
    }

    bson_destroy(&query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return 0;
}
