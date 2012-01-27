#include <v8.h>
#include <node.h>
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace v8;
using namespace node;

class Sleepy: Object
{
public:
  static Handle<Function> Init() {
    HandleScope scope;

    Persistent<FunctionTemplate> sleepy = Persistent<FunctionTemplate>::New(FunctionTemplate::New());
    sleepy->SetCallHandler(CallHandler);

    Local<Template> sproto = sleepy->PrototypeTemplate();
    sproto->Set(String::NewSymbol("sleep"), FunctionTemplate::New(Sleep)->GetFunction());

    Local<ObjectTemplate> sinst = sleepy->InstanceTemplate();
    sinst->SetNamedPropertyHandler(NamedPropGet, NamedPropSet);
    sinst->SetIndexedPropertyHandler(IndexedPropGet, IndexedPropSet);

    return sleepy->GetFunction();
  }

  static Handle<Value> CallHandler(const Arguments& args) {
    cout << "Go away! I'm sleepy!\n";
    return Handle<Value>();
  }

  static Handle<Value> NamedPropGet(Local<String> prop, const AccessorInfo& info) {
    cout << "Hey! Why'd you try to get my \"" << *String::Utf8Value(prop) << "\"?!\n";
    return Handle<Value>();
  }

  static Handle<Value> NamedPropSet(Local<String> prop, Local<Value> val, const AccessorInfo& info) {
    cout << "How dare you try to set my \"" << *String::Utf8Value(prop) << "\"?!\n";
    return Handle<Value>();
  }

  static Handle<Value> IndexedPropGet(unsigned int idx, const AccessorInfo& info) {
    cout << "Since when is my index " << idx << " any of your business?!\n";
    return Handle<Value>();
  }

  static Handle<Value> IndexedPropSet(unsigned int idx, Local<Value> val, const AccessorInfo& info) {
    cout << "Why do you need to set my index " << "?!\n";
    return Handle<Value>();
  }

  struct SleepData
  {
    unsigned secs;
    Persistent<Function> cont;
  };

  static Handle<Value> Sleep(const Arguments& args) {
    HandleScope scope;
    // TODO don't need this ref for this fn, but might be nice to have an example that uses it.
    //Sleepy* self = static_cast<Sleepy*>(args.This()->GetPointerFromInternalField(0));

    if(args.Length() < 1)
      return ThrowException(Exception::TypeError(String::New("You need to tell me how long to sleep!")));
    if(!args[0]->IsNumber())
      return ThrowException(Exception::TypeError(String::New("I can only sleep an integer number of seconds!")));
    if(args.Length() < 2)
      return ThrowException(Exception::TypeError(String::New("You need to tell me what to do after I sleep!")));
    if(!args[1]->IsFunction())
      return ThrowException(Exception::TypeError(String::New("I can only do a function after I sleep!")));

    unsigned secs = Integer::Cast(*args[0])->Value();
    Local<Function> cont = Local<Function>::Cast(args[1]);

    SleepData* data = new SleepData;
    data->secs = secs;
    data->cont = Persistent<Function>::New(cont);

    uv_work_t* req = new uv_work_t;
    req->type = UV_WORK;
    req->loop = uv_default_loop(); /* NOTE this should really be Loop() from <node.h>, but that is broke */
    req->data = data;
    req->work_cb = SleepWork;
    req->after_work_cb = SleepAfterWork;
    uv_queue_work(req->loop, req, req->work_cb, req->after_work_cb); /* NOTE yes, for some reason, you really do need to repeat these */

    return Undefined();
  }

  static void SleepWork(uv_work_t *req) {
    SleepData* data = static_cast<SleepData*>(req->data);

    sleep(data->secs);
  }

  static void SleepAfterWork(uv_work_t *req) {
    SleepData* data = static_cast<SleepData*>(req->data);

    TryCatch tc;

    data->cont->Call(Context::GetCurrent()->Global(), 0, NULL);
    data->cont.Dispose();
    delete data;

    if(tc.HasCaught())
      FatalException(tc);
  }
};

extern "C" {

void Init(Handle<Object> module) {
  module->Set(String::NewSymbol("Sleepy"), Sleepy::Init());
}

NODE_MODULE(sleepy, Init);

}
