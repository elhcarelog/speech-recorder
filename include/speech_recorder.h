#pragma once

#include <napi.h>

#include <atomic>
#include <functional>
#include <thread>

#include "aligned.h"
#include "audio_recorder.h"  // New class replacing ChunkProcessor

struct SpeechRecorderCallbackData {
  std::string event = "";
  std::vector<short> audio;
  double volume = 0.0;
};

struct SimplifiedOptions {
  int device;
  int samplesPerFrame;
  int sampleRate;
  std::function<void(std::vector<short>, double)> onAudio;
};

class SpeechRecorder : public Napi::ObjectWrap<SpeechRecorder> {
 private:
  std::thread thread_;
  Napi::ThreadSafeFunction threadSafeFunction_;
  std::atomic<bool> stopped_;
  BlockingReaderWriterQueue<SpeechRecorderCallbackData*> queue_;
  Napi::FunctionReference callback_;
  std::function<void(Napi::Env, Napi::Function, SpeechRecorderCallbackData*)>
      threadSafeFunctionCallback_;
  SimplifiedOptions options_;
  AudioRecorder processor_;

  void Start(const Napi::CallbackInfo& info);
  void Stop(const Napi::CallbackInfo& info);

 public:
  SpeechRecorder(const Napi::CallbackInfo& info);
  static Napi::Object Init(Napi::Env env, Napi::Object exports);

  ALIGNED
};

Napi::Value GetDevices(const Napi::CallbackInfo& info);
Napi::Object Init(Napi::Env env, Napi::Object exports);