#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <maolan/audiofileinput.h>
#include <maolan/ossout.h>
#include <maolan/ossin.h>
#include <maolan/audiotrack.h>
#include <maolan/config.h>
#include <maolan/constants.h>
#include <maolan/midichunk.h>
#include <maolan/midiclip.h>
#include <maolan/io.h>

using namespace maolan::audio;
int main(int argc, char **argv)
{
  /* MIDI file input
   *
   */
  // if (argc < 2)
  // {
    // std::cerr << "Usage: " << argv[0] << " <midi file>" << std::endl;
    // return 1;
  // }
  // Config::bpms.emplace_back();
  // MIDIClip *clip = new MIDIClip();
  // clip->load(argv[1]);
  // MIDIChunk *chunk;
  // while ((chunk = clip->next()) != nullptr)
  // {
    // std::cout << *chunk << std::endl;
  // }


  /* Beats
   * (60000ms / BPM) = Xms -> duration of 1/4 note or beat
  float bps = Config::bpm / 60.0;
  std::cout << Config::samplerate << ' ' << Config::bpm << std::endl;
  std::cout << Config::samplerate * 2 << ' ' << file.division << std::endl;
  float ratio = Config::samplerate / bps / file.division;
  int sample = 400;
  int res = ratio * sample;
  std::cout << ratio << ' ' << res << std::endl;
  */





  /* MIDI device input and output
   *
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <midi device>" << std::endl;
    return 1;
  }

  int fd;
  std::string dev = argv[1];
  if ((fd = open(dev.data(), O_RDWR, 0)) == -1)
  {
    std::cerr << dev.data() << ' ' << std::strerror(errno) << std::endl;
    return 1;
  }
  MIDIIO midi;
  while (true)
  {
    midi.get(fd);
  }
  */





  /* Audio device and file input and output
   *
  OSSIn in("/dev/dsp", 2);
  OSSOut out("/dev/dsp", 2);
  FileInput infile(argv[1]);
  out.connect(&in);
  out.connect(&infile);
  std::cout << "Playing ..." << std::endl;
  while (true)
  {
    for (auto &io : IO::ios)
    {
      io->fetch();
    }
    for (auto &io : IO::ios)
    {
      io->process();
    }
  }
  std::cout << std::endl;
  */
  OSSOut out("/dev/dsp", 2);
  FileInput infile(argv[1]);
  out.connect(&infile);
  while(1)
  {
    for (auto item = maolan::IO::begin(); item != nullptr; item = item->next())
    {
      item->fetch();
    }
    for (auto item = maolan::IO::begin(); item != nullptr; item = item->next())
    {
      item->process();
    }
  }
  return 0;
}
