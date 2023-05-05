#ifndef __evt_bus_h__
#define __evt_bus_h__


#include <condition_variable>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>

#include "event_emitter.h"

class evt_bus: public event_emitter{
private:
    ReaderWriterQueue<event>* queue;//工作线程队列
    std::map<std::string, std::set<observer>> handler;
    std::shared_mutex rw_lock={};
    std::mutex lock = {};
    std::condition_variable cond={};
public:
    evt_bus();
    ~evt_bus();
    void emit(std::string topic, std::string content) override;
    void on(std::string topic, callback cb) override;
    void loop() override;
    void next_tick() override;
};


#endif