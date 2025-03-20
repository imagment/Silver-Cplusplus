#ifndef SILVER_MUSIC_HPP
#define SILVER_MUSIC_HPP

#include "miniaudio.h"
#include "SilverMusic.hpp"
#include <atomic>
#include <thread>
#include <string>

class AudioPlayer {
public:
  explicit AudioPlayer(const std::string &filePath);
  ~AudioPlayer();

  void PlayAsync();
  void Stop();
  void SetVolume(float newVolume);
  void Pause();
  void Resume();

private:
  std::string filePath;
  ma_decoder decoder;
  ma_device device;
  std::thread playThread;
  std::atomic<bool> isPlaying;
  std::atomic<bool> isPaused;
  float volume;
  bool decoderInitialized;
  bool deviceInitialized;

  static void DataCallback(ma_device *device, void *output, const void *input, ma_uint32 frameCount);
  bool InitializeDecoder();
  bool InitializeDevice();
  void Cleanup();
  bool Initialize();
};

#endif
