#define MINIAUDIO_IMPLEMENTATION
#include "SilverMusic.hpp"
#include "miniaudio.h"
#include "Silver.hpp"

#include <chrono>
#include <cstring>
#include <iostream>

// Constructor
AudioPlayer::AudioPlayer(const std::string &filePath)
    : filePath(filePath), decoderInitialized(false), deviceInitialized(false),
      isPlaying(false), isPaused(false), volume(1.0f) {}

// Destructor
AudioPlayer::~AudioPlayer() {
  Stop();
  Cleanup();
}

// Method: PlayAsync
void AudioPlayer::PlayAsync() {
  Stop();
  isPlaying = true;
  isPaused = false;
  playThread = std::thread([this]() {
    if (!Initialize())
      return;
    if (ma_device_start(&device) != MA_SUCCESS) {
      std::cerr << "Failed to start playback device." << std::endl;
      isPlaying = false;
      return;
    }

    while (isPlaying) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ma_device_stop(&device);
    Cleanup();
  });
}

// Method: Stop
void AudioPlayer::Stop() {
  isPlaying = false;
  if (playThread.joinable()) {
    playThread.join();
  }
}

// Method: SetVolume
void AudioPlayer::SetVolume(float newVolume) {
  volume = newVolume;
  std::cout << "Volume set to: " << volume * 100 << "%" << std::endl;
}

// Method: Pause
void AudioPlayer::Pause() {
  if (isPlaying && !isPaused) {
    isPaused = true;
    std::cout << "Playback paused." << std::endl;
  }
}

// Method: Resume
void AudioPlayer::Resume() {
  if (isPlaying && isPaused) {
    isPaused = false;
    std::cout << "Playback resumed." << std::endl;
  }
}

// Static Method: DataCallback
void AudioPlayer::DataCallback(ma_device *device, void *output,
                                      const void *input, ma_uint32 frameCount) {
  auto *player = (AudioPlayer *)device->pUserData;

  if (player->isPaused) {
    std::memset(output, 0,
                frameCount * ma_get_bytes_per_frame(device->playback.format,
                                                    device->playback.channels));
    return;
  }

  ma_decoder *decoder = &player->decoder;
  if (decoder == nullptr) {
    return;
  }

  ma_uint32 framesToRead = frameCount;
  float *out = (float *)output;
  ma_decoder_read_pcm_frames(decoder, out, framesToRead, nullptr);

  for (ma_uint32 i = 0; i < framesToRead * device->playback.channels; i++) {
    out[i] *= player->volume;
  }
}

// Method: InitializeDecoder
bool AudioPlayer::InitializeDecoder() {
  if (ma_decoder_init_file(filePath.c_str(), nullptr, &decoder) != MA_SUCCESS) {
    std::cerr << "Failed to initialize decoder for file: " << filePath
              << std::endl;
    return false;
  }
  decoderInitialized = true;
  return true;
}

// Method: InitializeDevice
bool AudioPlayer::InitializeDevice() {
  ma_device_config deviceConfig =
      ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format = decoder.outputFormat;
  deviceConfig.playback.channels = decoder.outputChannels;
  deviceConfig.dataCallback = DataCallback;
  deviceConfig.pUserData = this;

  if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
    std::cerr << "Failed to initialize playback device." << std::endl;
    return false;
  }
  deviceInitialized = true;
  return true;
}

// Method: Cleanup
void AudioPlayer::Cleanup() {
  if (deviceInitialized) {
    ma_device_uninit(&device);
    deviceInitialized = false;
  }
  if (decoderInitialized) {
    ma_decoder_uninit(&decoder);
    decoderInitialized = false;
  }
}

// Method: Initialize
bool AudioPlayer::Initialize() {
  Cleanup();
  return InitializeDecoder() && InitializeDevice();
}
