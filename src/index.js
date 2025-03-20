const { SpeechRecorder, devices } = require("bindings")("speechrecorder.node");

class Wrapper {
  constructor(options) {
    options = options || {};
    options.device = options.device !== undefined ? options.device : -1;
    options.onAudio =
      options.onAudio !== undefined ? options.onAudio : (audio, volume) => {};
    options.samplesPerFrame =
      options.samplesPerFrame !== undefined ? options.samplesPerFrame : 480;
    options.sampleRate =
      options.sampleRate !== undefined ? options.sampleRate : 16000;

    this.inner = new SpeechRecorder((event, data) => {
      if (event === "audio") {
        options.onAudio({
          audio: data.audio,
          volume: data.volume,
        });
      }
    }, options);
  }

  start() {
    this.inner.start();
  }

  stop() {
    this.inner.stop();
  }
}

exports.SpeechRecorder = Wrapper;
exports.devices = devices;
