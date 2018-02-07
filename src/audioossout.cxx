#include <iostream>
#include <cstring>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>
#include <maolan/audioossout>
#include <maolan/config>


using namespace std;


AudioOSSOut::AudioOSSOut(const size_t &chs)
  : AudioIO(chs)
{
  inputs.resize(chs);
  outputs.resize(chs);
  string device = "/dev/dsp";
  format = AFMT_S32_NE;
  samplerate = 48000;
  oss_audioinfo ai;
  int tmp;
  int devcaps;

  if ((fd = open(device.data(), O_WRONLY, 0)) == -1)
  {
    cerr << device;
    cerr << strerror(errno) << endl;
    exit(1);
  }
  ai.dev = -1;
  if (ioctl(fd, SNDCTL_ENGINEINFO, &ai) != -1)
  {
    cout << "Using audio engine " << ai.dev;
    cout << " = " << ai.name << " for output" << endl;
  }

  if (ioctl(fd, SNDCTL_DSP_GETCAPS, &devcaps) == -1)
  {
    cerr << "SNDCTL_DSP_GETCAPS";
    cerr << strerror(errno) << endl;
    exit(1);
  }

  tmp = 0;
  if (ioctl(fd, SNDCTL_DSP_POLICY, &tmp) == -1)
  {
    cerr << "SNDCTL_DSP_POLICY";
    cerr << strerror(errno) << endl;
    exit(1);
  }

  tmp = channels();
  if (ioctl(fd, SNDCTL_DSP_CHANNELS, &tmp) == -1)
  {
    cerr << "SNDCTL_DSP_CHANNELS: ";
    cerr << strerror(errno) << endl;
    exit(1);
  }
  if (tmp != channels())
  {
    cerr << device << " doesn't support ";
    cerr << channels() << " channels, but ";
    cerr << tmp << " instead" << endl;
    exit(1);
  }

  tmp = format;
  if (ioctl(fd, SNDCTL_DSP_SETFMT, &tmp) == -1)
  {
    cerr << "SNDCTL_DSP_SETFMT";
    cerr << strerror(errno) << endl;
    exit(1);
  }
  if (tmp != AFMT_S32_NE && tmp != AFMT_S32_OE)
  {
    cerr << device << " doesn't support 32 bit sample format (";
    cerr << tmp << ")" << endl;
    exit(1);
  }

  tmp = samplerate;
  if (ioctl(fd, SNDCTL_DSP_SPEED, &tmp) == -1)
  {
    cerr << "SNDCTL_DSP_SPEED";
    cerr << strerror(errno) << endl;
    exit(1);
  }

  if (ioctl(fd, SNDCTL_DSP_GETBLKSIZE, &fragsize) == -1)
  {
    cerr << "SNDCTL_DSP_GETBLKSIZE";
    cerr << strerror(errno) << endl;
    exit(1);
  }

  cout << "Sample parameters for output set OK. Using fragment size " << fragsize << endl;
}


AudioOSSOut::~AudioOSSOut()
{
  close(fd);
}


void AudioOSSOut::sync() const
{
  auto trig = PCM_ENABLE_INPUT | PCM_ENABLE_OUTPUT;
  ioctl(fd, SNDCTL_DSP_SETTRIGGER, &trig);
  ioctl(fd, SNDCTL_DSP_SYNC, NULL);
}


void AudioOSSOut::fetch()
{
  for (size_t i = 0; i < channels(); ++i)
  {
    outputs[i] = inputs[i].pull();
  }
}


void AudioOSSOut::process()
{
  int result;
  float element;
  for (size_t i = 0; i < Config::audioChunkSize; ++i)
  {
    for (auto &channel : outputs)
    {
      if (channel == nullptr) {
        result = 0;
      }
      else
      {
        element = channel->data[i];
        if (element > 1.0)
        {
          result = numeric_limits<int>::max();
        }
        else if (element < -1.0)
        {
          result = numeric_limits<int>::min();
        }
        else
        {
          result = element * numeric_limits<int>::max();
        }
      }
      normalizedOut.push_back(result);
    }
  }
  int dataSize = normalizedOut.size() * sizeof(*normalizedOut.data());
  write(fd, normalizedOut.data(), dataSize);
  normalizedOut.clear();
}


size_t AudioOSSOut::channels() const
{
  return outputs.size();
}


void AudioOSSOut::connect(AudioIO *to)
{
  for (size_t channel = 0; channel < channels(); ++channel)
  {
    AudioConnection conn(to, channel);
    inputs[channel].add(conn);
  }
}


void AudioOSSOut::connect(AudioIO *to, std::size_t inCh, std::size_t outCh)
{
  AudioConnection conn(to, outCh);
  inputs[inCh].add(conn);
}