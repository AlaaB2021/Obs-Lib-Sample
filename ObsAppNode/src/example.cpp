#include "example.h"
#include "StreamMgr.h"

using namespace std;

static StreamMgr* StreamPtr;

bool example::start(int i){
	StreamPtr = new StreamMgr();

	return StreamPtr->Init();
}

void example::startStream() {
	StreamPtr->StartStreaming();
}

void example::stop() {
	StreamPtr->StopStreaming();
}

Napi::Boolean example::startWrapped(const Napi::CallbackInfo& info){
	Napi::Env env = info.Env();
	Napi::Boolean returnValue = Napi::Boolean::New(env, example::start(1));
 return returnValue;
}
void example::startStreamWrapped(const Napi::CallbackInfo& info) {
	example::startStream();
}
void example::stopWrapped(const Napi::CallbackInfo& info) {
	example::stop();
}
Napi::Object example::Init(Napi::Env env, Napi::Object exports)
{
  //export Napi function
  exports.Set("start", Napi::Function::New(env, example::startWrapped));
  exports.Set("startStream", Napi::Function::New(env, example::startStreamWrapped));
  exports.Set("stop", Napi::Function::New(env, example::stopWrapped));

  return exports;
}