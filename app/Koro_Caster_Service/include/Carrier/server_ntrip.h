#pragma once
#include "ntrip_global.h"
#include "process_queue.h"

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "Compontent/caster_core.h"

#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class server_ntrip
{
private:
    json _conf;
    json _info;

    int _connect_timeout=0;
    bool _heart_beat_switch;
    int _heart_beat_interval;
    std::string _heart_beat_msg;

    std::string _connect_key;
    std::string _mount_point;

    std::string _ip;
    int _port;

    bool _NtripVersion2 = false;
    bool _transfer_with_chunked = false;

    size_t _chuncked_size = 0;

    bufferevent *_bev;
    evbuffer *_evbuf;

    // 定时器和定时事件
    event *_timeout_ev;
    timeval _timeout_tv;

    timeval _bev_read_timeout_tv;

public:
    server_ntrip(json req, bufferevent *bev);
    ~server_ntrip();

    std::string get_connect_key();

    int start();
    int stop();

    int bev_send_reply();

    static void ReadCallback(struct bufferevent *bev, void *arg);
    static void EventCallback(struct bufferevent *bev, short events, void *arg);

    static void TimeoutCallback(evutil_socket_t fd, short events, void *arg);

    int send_heart_beat_to_server();

    int publish_data_from_chunck();
    int publish_data_from_evbuf();
};
