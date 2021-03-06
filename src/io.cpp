#include "maolan/io.hpp"
#include "maolan/utils.hpp"


using namespace maolan;


IO *IO::ios = nullptr;
IO *IO::last = nullptr;
IO *IO::_current = nullptr;
unsigned IO::_stage = 0;
bool IO::_rec = false;
bool IO::_playing = false;
bool IO::_quit = false;
std::size_t IO::_playHead = 0;
std::atomic_size_t IO::_count{0};
std::mutex IO::m;
std::condition_variable IO::cv;
std::vector<maolan::Config *> IO::_devices;


enum Stage
{
  FETCH = 0,
  PROCESS,
  TOTAL
};


IO::IO(const std::string &argName, const bool &front, const bool &reg)
  : _next{nullptr}
  , _previous{nullptr}
  , _name{argName}
{
  if (reg)
  {
    if (front)
    {
      this->_next = ios;
      if (ios != nullptr)
      {
        ios->_previous = this;
      }
      ios = this;
      if (last == nullptr)
      {
        last = this;
      }
    }
    else
    {
      this->_previous = last;
      last = this;
      if (ios == nullptr)
      {
        ios = this;
      }
    }
  }
  if (_current == nullptr)
  {
    _current = ios;
  }
}


IO::~IO()
{
  if (this->_previous != nullptr)
  {
    this->_previous->next(this->_next);
  }
  else
  {
    ios = this->_next;
  }
  if (this->_next != nullptr)
  {
    this->_next->previous(this->_previous);
  }
  else
  {
    last = this->_previous;
  }
}


void IO::work()
{
  if (_stage == FETCH)
  {
    setup();
    fetch();
  }
  else if (_stage == PROCESS)
  {
    process();
  }
  --_count;
}


IO *IO::loadFromXml(pugi::xml_node *n)
{
  auto io = xmlElement2IO(n);
  for (auto node = n->first_child(); node != nullptr;
       node = node.next_sibling())
  {
    io->loadFromXml(&node);
  }
  return io;
}


IO *IO::task()
{
  std::unique_lock<std::mutex> lk(m);
  cv.wait(lk, [] { return IO::check(); });
  if (_quit)
  {
    return nullptr;
  }
  ++_count;
  auto result = _current;
  if (_current != nullptr)
  {
    _current = _current->next();
  }
  lk.unlock();
  cv.notify_one();
  return result;
}


bool IO::check()
{
  if (_quit)
  {
    return true;
  }
  if (ios == nullptr)
  {
    return false;
  }
  if (!_playing)
  {
    return false;
  }
  if (_current == nullptr)
  {
    if (_count == 0)
    {
      _current = ios;
      _stage = ++_stage % TOTAL;
      if (_stage == FETCH)
      {
        _playHead += Config::audioBufferSize;
        if (Config::tempoIndex + 1 < Config::tempos.size())
        {
          // TODO: move to next tempo
          auto tempo = Config::tempos[Config::tempoIndex];
          while (Config::tempoIndex < Config::tempos.size() &&
                 _playHead <= tempo.time)
          {
            ++Config::tempoIndex;
          }
        }
      }
      return _current != nullptr;
    }
    return false;
  }
  return true;
}


void IO::play()
{
  _playing = true;
  _playHead = -Config::audioBufferSize;
  cv.notify_one();
}


void IO::stop()
{
  _playing = false;
  cv.notify_one();
}


void IO::quit()
{
  _quit = true;
  cv.notify_one();
}

void IO::parent(IO *p) {}
void IO::rec(bool record) { _rec = record; }
bool IO::rec() { return _rec; }
void IO::stage(const bool &s) { _stage = s; }
bool IO::stage() { return _stage; }
IO *IO::begin() { return ios; }
std::string IO::type() { return _type; }
void IO::type(const std::string &argType) { _type = argType; }
std::string IO::name() { return _name; }
void IO::name(const std::string &argName) { _name = argName; }
uint64_t IO::playHead() { return _playHead; }
void IO::playHead(const std::size_t &argPlayHead) { _playHead = argPlayHead; }
void IO::setup() {}
void IO::next(IO *n) { _next = n; }
IO *IO::next() { return _next; }
void IO::previous(IO *p) { _previous = p; }
IO *IO::previous() { return _previous; }
