#include <maolan/audioinput>
#include <maolan/config>


using namespace std;


AudioInput::AudioInput()
  : AudioIO(1)
{}


void AudioInput::add(const AudioConnection &conn)
{
  connections.push_back(conn);
}


void AudioInput::fetch()
{
  vector<AudioChunk> channels;
  bool empty = true;
  for (auto &connection : connections)
  {
    const auto element = connection.pull();
    if (element != nullptr)
    {
      empty = false;
      channels.push_back(element);
    }
  }
  if (empty) {
    outputs[0] = nullptr;
  }
  else if (channels.size() == 1)
  {
    outputs[0] = channels[0];
  }
  else
  {
    const AudioChunk result = AudioChunk(new AudioChunkData(Config::audioChunkSize));
    for (size_t i = 0; i < Config::audioChunkSize; ++i)
    {
      float sum = 0;
      for (auto channel : channels)
      {
        if (channel == nullptr) {continue;}
        if (channel->data[i] == 0.0) {continue;}
        sum += channel->data[i];
      }
      result->data[i] = sum;
    }
    outputs[0] = result;
  }
}


size_t AudioInput::channels() const
{
  return outputs.size();
}


AudioChunk AudioInput::pull()
{
  return outputs[0];
}