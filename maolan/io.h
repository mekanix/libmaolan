#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <pugixml.hpp>


namespace maolan
{
class IO
{
public:
  IO(const std::string &argName = "", const bool &front = true,
     const bool &reg = true);
  ~IO();

  static void rec(bool record);
  static bool rec();
  static void play();
  static void stop();
  static void quit();
  static void playHead(const uint64_t &argPlayHead);
  static uint64_t playHead();
  static IO * loadFromXml(pugi::xml_node *n);
  static IO * task();
  static IO * begin();

  virtual void setup();
  virtual void fetch() = 0;
  virtual void process() = 0;
  virtual void parent(IO *p);
  void work();
  void next(IO *);
  IO * next();
  void previous(IO *);
  IO * previous();
  void stage(const bool &s);
  bool stage();
  void type(const std::string &);
  std::string type();
  void name(const std::string &);
  std::string name();

protected:
  static bool check();

  static IO *ios;
  static IO *last;
  static IO *_current;
  static unsigned _stage;
  static bool _rec;
  static bool _playing;
  static bool _quit;
  static uint64_t _playHead;
  static std::atomic_size_t _count;
  static std::mutex m;
  static std::condition_variable cv;

  std::string _type;
  std::string _name;
  int state;
  IO *_next;
  IO *_previous;
};
} // namespace maolan
