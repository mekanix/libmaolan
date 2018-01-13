#pragma once
#include "audioio.h"


class AudioConnection
{
  public:
    AudioConnection(AudioIO *connectTo, std::size_t &ch);

    AudioIO * get();
    AudioChunk pull();

  protected:
    AudioIO *to;
    std::size_t channel;
};
