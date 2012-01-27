#include <iostream>
#include <string>
#include <v8.h>
#include <node.h>

using namespace std;
using namespace v8;
using namespace node;

class Binding
{
public:
  static Handle<Function> Init() {
    HandleScope scope;

    Persistent<FunctionTemplate> b = Persistent<FunctionTemplate>::New(FunctionTemplate::New());

    b->Set(String::NewSymbol("prop"), String::New("It's a regular property"));

    Local<Template> bproto = b->PrototypeTemplate();
    bproto->Set(String::NewSymbol("pprop"), String::New("It's a prototype property!"));

    Local<ObjectTemplate> binst = b->InstanceTemplate();
    binst->SetNamedPropertyHandler(PropGet, PropSet);

    return b->GetFunction();
  }

  static Handle<Value> PropGet(Local<String> prop, const AccessorInfo& info) {
    cout << "Tried to get \"" << *String::Utf8Value(prop) << "\"!\n";
  }

  static Handle<Value> PropSet(Local<String> prop, Local<Value> val, const AccessorInfo& info) {
    cout << "Tried to set \"" << *String::Utf8Value(prop) << "\"!\n";
  }
};

extern "C" {

void Init(Handle<Object> module) {
  module->Set(String::NewSymbol("Binding"), Binding::Init());
}

NODE_MODULE(binding, Init);

}
