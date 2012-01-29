#include <v8.h>
#include <node.h>
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace v8;
using namespace node;

class Sleepy
{
private:
  unsigned secs;

public:
  static Handle<Function> Init() {
    HandleScope scope;

    Handle<FunctionTemplate> sleepy = FunctionTemplate::New(New);

    Local<Template> sproto = sleepy->PrototypeTemplate();
    sproto->Set(String::NewSymbol("sleep"), FunctionTemplate::New(Sleep));

    Local<ObjectTemplate> sinst = sleepy->InstanceTemplate();
    sinst->SetInternalFieldCount(1);
    sinst->SetNamedPropertyHandler(NamedPropGet, NamedPropSet);
    sinst->SetIndexedPropertyHandler(IndexedPropGet, IndexedPropSet);

    return sleepy->GetFunction();
  }

  Sleepy(unsigned _secs) {
    secs = _secs;
  }

  ~Sleepy() {
  }

  static Handle<Value> New(const Arguments& args) {
    HandleScope scope;

    cout << "Go away! I'm sleepy!\n";

    unsigned secs = Integer::Cast(*args[0])->Value();
    Sleepy* sleepy = new Sleepy(secs);

    Persistent<Object> obj(Persistent<Object>::New(args.Holder()));
    obj->SetInternalField(0, External::Wrap(sleepy));
    obj.MakeWeak(NULL, WeakCallback); /* NOTE you could also implement this by passing sleepy as first arg */
    return obj;
  }

  static void WeakCallback(Persistent<Value> obj, void* arg) {
    cout << "Guess you don't need me anymore...\n";
    Sleepy* sleepy = static_cast<Sleepy*>(External::Unwrap(Persistent<Object>::Cast(obj)->GetInternalField(0)));
    delete sleepy;
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
    Sleepy* sleepy;
    Persistent<Function> cont;
  };

  static Handle<Value> Sleep(const Arguments& args) {
    HandleScope scope;

    if(args.Length() < 1)
      return ThrowException(Exception::TypeError(String::New("You need to tell me what to do after I sleep!")));
    if(!args[0]->IsFunction())
      return ThrowException(Exception::TypeError(String::New("I can only do a function after I sleep!")));

    Sleepy* self = static_cast<Sleepy*>(External::Unwrap(args.Holder()->GetInternalField(0)));
    Local<Function> cont = Local<Function>::Cast(args[0]);

    SleepData* data = new SleepData;
    data->sleepy = self;
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

    sleep(data->sleepy->secs);
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
