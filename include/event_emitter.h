#ifndef __event_emitter_h__
#define __event_emitter_h__

#include <readerwriterqueue/readerwriterqueue.h>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

using namespace moodycamel;

using json = nlohmann::json;

class event{
public:
    std::string topic;
    std::string content;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(event, topic, content);
};

using callback = std::function<void(std::string)> ;

class observer {
public:
    callback cb;
    bool operator< (const observer& a) const{
        return this < &a;
    }
};

class event_emitter{
public:
    virtual void emit(std::string topic, std::string content) = 0;
    virtual void on(std::string topic, callback cb) = 0;
    virtual void loop() = 0;
    virtual void next_tick() =0;
};

#endif