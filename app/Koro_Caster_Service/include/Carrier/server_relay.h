#pragma once
#include "ntrip_global.h"
#include "process_queue.h"

#include "iostream"

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "Compontent/caster_core.h"

#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class server_relay
{
private:
    /* data */

private:
    json _info;

    int _Carrier_Type;
    std::string _connect_key;
    std::string _mount_point;
    std::string _mount_group;
    std::string _req_user_name;
    std::string _req_connect_key;
    std::string _ip;
    int _port;

    bool _NtripVersion2 = false;
    bool _transfer_with_chunked = false;

    size_t _chuncked_size = 0;

    evhttp_request *_hev;
    bufferevent *_bev;
    evbuffer *_evbuf;

public:
    server_relay(json req, bufferevent *bev);
    ~server_relay();

    int start();
    int stop();

    static void ReadCallback(struct bufferevent *bev, void *arg);
    static void EventCallback(struct bufferevent *bev, short events, void *arg);

    int publish_data_from_chunck();
    int publish_data_from_evbuf();

    // static void Redis_Recv_Callback(redisAsyncContext *c, void *r, void *privdata);

    //static void Redis_Unsub_Callback(redisAsyncContext *c, void *r, void *privdata);

    int send_del_req();
};
