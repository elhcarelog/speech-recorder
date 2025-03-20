#include <napi.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "audio_recorder.h"  // New class replacing ChunkProcessor
#include "devices.h"
#include "portaudio.h"
#include "speech_recorder.h"

Napi::Object SpeechRecorder::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function f =
      DefineClass(env, "SpeechRecorder",
                  {
                      InstanceMethod<&SpeechRecorder::Start>(
                          "start", static_cast<napi_property_attributes>(
                                       napi_writable | napi_configurable)),
                      InstanceMethod<&SpeechRecorder::Stop>(
                          "stop", static_cast<napi_property_attributes>(
                                      napi_writable | napi_configurable)),
                  });

  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(f);

  exports.Set("SpeechRecorder", f);
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  exports.Set(Napi::String::New(env, "devices"),
              Napi::Function::New(env, GetDevices));
  return exports;
}

SpeechRecorder::SpeechRecorder(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<SpeechRecorder>(info),
      stopped_(true),
      queue_(),
      callback_(Napi::Persistent(info[0].As<Napi::Function>())),
      threadSafeFunctionCallback_([&](Napi::Env env, Napi::Function jsCallback,
                                      SpeechRecorderCallbackData* data) {
        Napi::Object object = Napi::Object::New(env);
        object.Set("volume", Napi::Number::New(env, data->volume));

        if (data->audio.size() > 0) {
          Napi::Int16Array buffer =
              Napi::Int16Array::New(env, data->audio.size());
          for (size_t i = 0; i < data->audio.size(); i++) {
            buffer[i] = data->audio[i];
          }
          object.Set("audio", buffer);
        }

        jsCallback.Call({Napi::String::New(env, data->event), object});
        delete data;
      }),
      options_({
          info[1]
              .As<Napi::Object>()
              .Get("device")
              .As<Napi::Number>()
              .Int32Value(),
          info[1]
              .As<Napi::Object>()
              .Get("samplesPerFrame")
              .As<Napi::Number>()
              .Int32Value(),
          info[1]
              .As<Napi::Object>()
              .Get("sampleRate")
              .As<Napi::Number>()
              .Int32Value(),
          [&](std::vector<short> audio, double volume) {
            SpeechRecorderCallbackData* data = new SpeechRecorderCallbackData();
            data->event = "audio";
            data->audio = audio;
            data->volume = volume;
            queue_.enqueue(data);
          },
      }),
      processor_(options_) {}

void SpeechRecorder::Start(const Napi::CallbackInfo& info) {
  stopped_ = false;
  threadSafeFunction_ = Napi::ThreadSafeFunction::New(
      info.Env(), callback_.Value(), "Speech Recorder Start", 0, 1,
      [&](Napi::Env env) { thread_.join(); });

  thread_ = std::thread([&] {
    while (!stopped_) {
      SpeechRecorderCallbackData* data;
      bool element = queue_.try_dequeue(data);
      if (element) {
        threadSafeFunction_.BlockingCall(data, threadSafeFunctionCallback_);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    threadSafeFunction_.Release();
  });

  processor_.Start();
}

void SpeechRecorder::Stop(const Napi::CallbackInfo& info) {
  stopped_ = true;
  processor_.Stop();
}

Napi::Value GetDevices(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  std::vector<speechrecorder::Device> devices = speechrecorder::GetDevices();
  Napi::Array result = Napi::Array::New(env, devices.size());
  for (size_t i = 0; i < devices.size(); i++) {
    Napi::Object e = Napi::Object::New(env);
    e.Set("id", devices[i].id);
    e.Set("name", devices[i].name);
    e.Set("apiName", devices[i].apiName);
    e.Set("maxInputChannels", devices[i].maxInputChannels);
    e.Set("maxOutputChannels", devices[i].maxOutputChannels);
    e.Set("defaultSampleRate", devices[i].defaultSampleRate);
    e.Set("isDefaultInput", devices[i].isDefaultInput);
    e.Set("isDefaultOutput", devices[i].isDefaultOutput);
    result[i] = e;
  }
  return result;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  SpeechRecorder::Init(env, exports);
  return exports;
}

NODE_API_MODULE