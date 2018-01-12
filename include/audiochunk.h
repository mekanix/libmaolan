#pragma once
#include <memory>


struct AudioChunkData {
  AudioChunkData(const size_t &size);
  ~AudioChunkData();
  float *data;
};


typedef std::shared_ptr<AudioChunkData> AudioChunk;
