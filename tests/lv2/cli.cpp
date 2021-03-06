#include <iostream>
#include <maolan/audio/oss/out.h>
#include <maolan/engine.h>
#include <maolan/midi/oss/in.h>
#include <maolan/plugin/instrument.h>


namespace lv2 = maolan::plugin::lv2;
namespace midi = maolan::midi;
namespace audio = maolan::audio;


int main(int argc, char **argv)
{
  audio::OSSOut out("/dev/dsp", 2, 4);
  midi::OSSMIDIIn midiIn("/dev/umidi0.0");
  auto drumgizmo =
      new maolan::plugin::Instrument("http://drumgizmo.org/lv2", "lv2");
  drumgizmo->connect(&midiIn);
  out.connect(drumgizmo->audio());

  maolan::Engine::init();
  std::cerr << "Playing ...";
  maolan::Engine::play();
  std::this_thread::sleep_for(std::chrono::seconds(14));
  std::cerr << " done\n";
  std::cerr << "Stopping ...";
  maolan::Engine::stop();
  std::cerr << " done\n";
  std::cerr << "Exiting ...";
  maolan::Engine::quit();
  std::cerr << " done\n";

  delete drumgizmo;
  return 0;
}
