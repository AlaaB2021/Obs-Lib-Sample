#include <napi.h>
#include <iostream>
using namespace std;
namespace example{
 //add number function
 bool start(int i);
 void startStream();
 void stop();
 //add function wrapper
 Napi::Boolean startWrapped(const Napi::CallbackInfo& info);
 void startStreamWrapped(const Napi::CallbackInfo& info);
 void stopWrapped(const Napi::CallbackInfo& info);
 //Export API
 Napi::Object Init(Napi::Env env, Napi::Object exports);
 NODE_API_MODULE(addon, Init)
}