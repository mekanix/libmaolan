#include <maolan/audio/buffer.h>

using namespace maolan::audio;

BufferData::BufferData(const size_t &size) { data = new float[size]; }


BufferData::~BufferData() { delete[] data; }
