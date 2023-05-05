#ifndef __js_h__
#define __js_h__


#include <string>
#include <map>
#include <shared_mutex>

extern "C" {
#include "quickjs/quickjs.h"
#include "quickjs/quickjs-libc.h"
}

#include "event_emitter.h"

typedef std::function<JSValue(JSContext*, JSValueConst, int, JSValueConst*)> vhf;

class vm {

private:
    JSRuntime* rt;
    JSContext* ctx;
    JSValue global;
    JSValue native_handler;
    JSValue js_handler;

    vhf f;
    event_emitter* evt;
    static std::map<JSContext*, vhf> global_ctx_map;
    static std::shared_mutex global_rw_lock;
    static JSValue global_handler(JSContext* ctx, JSValueConst js_this, int argc, JSValueConst* argv);
    JSValue handler(JSContext* ctx, JSValueConst js_this, int argc, JSValueConst* argv);
    void new_runtime();
    void new_handler();
    void try_catch(JSValue ret);
public:

    vm()=default;
    vm(event_emitter *evt);
    ~vm();
   
    void eval(std::string script);
    void on(std::string topic, callback cb);           // native on js evt
    void emit(std::string topic, std::string context) ; // native evt to js
};



#endif