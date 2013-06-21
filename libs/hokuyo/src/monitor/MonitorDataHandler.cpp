/*!
  \file
  \brief ���j�^�f�[�^�Ǘ�

  \author Satofumi KAMIMURA

  $Id: MonitorDataHandler.cpp 1483 2009-11-01 23:58:16Z satofumi $

  \todo �^���̃����o���\�b�h���A�e���v���[�g�Ő��������@���������ׂ�
*/

#include "MonitorDataHandler.h"
#include "MonitorEventScheduler.h"
#include "../system/Lock.h"
#include "../system/ticks.h"
#include "../common/split.h"
#include <cstdlib>
#include <fstream>

using namespace qrk;
using namespace std;


namespace
{
  enum {
    DummySize = 0,
  };

  const char LF = 0x0a;

  class FileHandler
  {
  public:
    virtual ~FileHandler(void)
    {
    }

    virtual int fetchFirstLine(const int size, const char* comment = NULL) = 0;
    virtual int fetch(char* data, int size) = 0;
    virtual int fetch(string& data) = 0;
    virtual int fetch(bool& data) = 0;
    virtual int fetch(int& data) = 0;
    virtual int fetch(unsigned int& data) = 0;
    virtual int fetch(short& data) = 0;
    virtual int fetch(long& data) = 0;
  };


  class RecordHandler : public FileHandler
  {
  public:
    ofstream fout_;


    RecordHandler(const char* file)
    {
      fout_.open(file, ios::binary);
      if (! fout_.is_open()) {
        string message = string("open log file: ") + file;
        perror(message.c_str());
        // !!! ��O�𓊂���
      }
    }


    ~RecordHandler(void)
    {
      fout_.close();
    }


    int fetchFirstLine(const int size, const char* comment = NULL)
    {
      int current_ticks = ticks();
      fout_ << current_ticks << "," << size;

      if (comment) {
        fout_ << " # " << comment;
      }
      setEndl(fout_);

      return size;
    }


    int fetch(char* data, int size)
    {
      fout_.write(data, size);
      setEndl(fout_);

      return size;
    }


    // !!! �e���v���[�g���`���Ďg���ׂ�
    // !!! �Ăяo������
    int fetch(string& data)
    {
      fout_ << data;
      setEndl(fout_);

      return data.size();
    }


    int fetch(bool& data)
    {
      fout_ << data;
      setEndl(fout_);

      return data;
    }


    int fetch(int& data)
    {
      fout_ << data;
      setEndl(fout_);

      return data;
    }


    int fetch(unsigned int& data)
    {
      fout_ << data;
      setEndl(fout_);

      return data;
    }


    int fetch(short& data)
    {
      fout_ << data;
      setEndl(fout_);

      return data;
    }


    int fetch(long& data)
    {
      fout_ << data;
      setEndl(fout_);

      return data;
    }


    void setEndl(ofstream& fd)
    {
      char ch = LF;
      fd.write(&ch, 1);
      fd.flush();
    }
  };


  class PlayHandler : public FileHandler
  {
    MonitorEventScheduler* scheduler_;
    ConditionVariable condition_;
    bool no_error_;
    string play_file_;

  public:
    ifstream fin_;


    PlayHandler(const char* file)
      : scheduler_(MonitorEventScheduler::object()), no_error_(true),
        play_file_(file)
    {
      fin_.open(file, ios::binary);
      if (! fin_.is_open()) {
        string message = string("open log file: ") + file;
        perror(message.c_str());
        // !!! ��O�𓊂���
        // !!! ��O�𓊂����@�ȊO�ł́A�G���[�����������Ƃ�ʒm�ł��Ȃ�
      }
    }


    ~PlayHandler(void)
    {
      fin_.close();
      if (condition_.isWaiting()) {
        condition_.wakeup();
      }
    }


    int fetchFirstLine(const int size, const char* comment = NULL)
    {
      static_cast<void>(size);
      static_cast<void>(comment);

      // �P�s�ڂ̓ǂݏo��
      string line;
      getline(fin_, line, LF);
      size_t comment_position = line.find("#");
      if (comment_position != string::npos) {
        // # �ȍ~�̕�����̓R�����g�Ƃ��Ė�������
        line.erase(comment_position);
      }
      vector<string> tokens;
      if (split(tokens, line, ",") != 2) {
        if (no_error_) {
          fprintf(stderr, "mismatch log: %s\n", play_file_.c_str());
        }
        no_error_ = false;
        return -1;
      }

      int target_ticks = atoi(tokens[0].c_str());

      // �w�� ticks �܂őҋ@����
      if (target_ticks > ticks()) {
        scheduler_->registerWakeupTicks(&condition_, target_ticks);
        Lock mutex;
        mutex.lock();
        condition_.wait(&mutex);
	mutex.unlock();
      }

      int data_size = atoi(tokens[1].c_str());
      return data_size;
    }


    void skipEndl(void)
    {
      char end_ch = '\0';
      fin_.read(&end_ch, 1);
    }


    int fetch(char* data, int size)
    {
      if (data) {
        fin_.read(data, size);
      } else {
        // �ǂݎ̂Ă�
        enum { BufferSize = 256 };
        char buffer[BufferSize];
        int left_size = size;
        while (left_size > 0) {
          int readable_size = min(left_size, static_cast<int>(BufferSize));
          fin_.read(buffer, readable_size);
          left_size -= readable_size;
        }
      }
      skipEndl();

      return size;
    }


    int fetch(string& data)
    {
      getline(fin_, data, LF);

      return data.size();
    }


    int fetch(bool& data)
    {
      char ch = '\0';
      fin_.read(&ch, 1);
      skipEndl();

      data = (ch == '0') ? false : true;
      return data;
    }


    int fetch(int& data)
    {
      fin_ >> data;
      skipEndl();

      return data;
    }


    int fetch(unsigned int& data)
    {
      fin_ >> data;
      skipEndl();

      return data;
    }


    int fetch(short& data)
    {
      fin_ >> data;
      skipEndl();

      return data;
    }


    int fetch(long& data)
    {
      fin_ >> data;
      skipEndl();

      return data;
    }
  };
}


struct MonitorDataHandler::pImpl
{
  MonitorModeManager::MonitorMode mode_;
  string file_;
  FileHandler* handler_;


  pImpl(string file, MonitorModeManager::MonitorMode mode)
    : mode_(mode), file_(file), handler_(NULL)
  {
    if (mode_ == MonitorModeManager::Record) {
      handler_ = new RecordHandler(file_.c_str());

    } else if (mode_ == MonitorModeManager::Play) {
      handler_ = new PlayHandler(file_.c_str());

    } else if (mode_ == MonitorModeManager::Simulation) {
      // !!!
    }
  }


  ~pImpl(void)
  {
    delete handler_;
  }


  int fetch(char* data, int size, const char* comment)
  {
    int data_size = handler_->fetchFirstLine(size, comment);
    if (data_size < 0) {
      return -1;
    }
    return handler_->fetch(data, data_size);
  }


  int fetch(string& data, const char* comment)
  {
    int size = data.size();
    if (handler_->fetchFirstLine(size, comment) < 0) {
      return -1;
    }
    return handler_->fetch(data);
  }


  int fetch(bool& data, const char* comment)
  {
    if (handler_->fetchFirstLine(1, comment) < 0) {
      return -1;
    }
    return handler_->fetch(data);
  }


  int fetch(int& data, const char* comment)
  {
    int n = digitsNumber(data);
    if (handler_->fetchFirstLine(n, comment) < 0) {
      return -1;
    }
    return handler_->fetch(data);
  }


  int fetch(unsigned int& data, const char* comment)
  {
    int n = digitsNumber(data);
    if (handler_->fetchFirstLine(n, comment) < 0) {
      return -1;
    }
    return handler_->fetch(data);
  }


  int fetch(short& data, const char* comment)
  {
    int n = digitsNumber(data);
    if (handler_->fetchFirstLine(n, comment) < 0) {
      return -1;
    }
    return handler_->fetch(data);
  }


  int fetch(long& data, const char* comment)
  {
    int n = digitsNumber(data);
    if (handler_->fetchFirstLine(n, comment) < 0) {
      return -1;
    }
    return handler_->fetch(data);
  }


  int digitsNumber(long data)
  {
    enum { BufferMax = 22 };
    char buffer[BufferMax];
    return snprintf(buffer, BufferMax, "%ld", data);
  }
};


MonitorDataHandler::MonitorDataHandler(string file,
                                       MonitorModeManager::MonitorMode mode)
  : pimpl(new pImpl(file, mode))
{
}


MonitorDataHandler::~MonitorDataHandler(void)
{
}


int MonitorDataHandler::fetch(char* data, int size, const char* comment)
{
  return pimpl->fetch(data, size, comment);
}


int MonitorDataHandler::fetch(string& data, const char* comment)
{
  return pimpl->fetch(data, comment);
}


int MonitorDataHandler::fetch(bool& data, const char* comment)
{
  return pimpl->fetch(data, comment);
}


int MonitorDataHandler::fetch(int& data, const char* comment)
{
  return pimpl->fetch(data, comment);
}


int MonitorDataHandler::fetch(short& data, const char* comment)
{
  return pimpl->fetch(data, comment);
}


int MonitorDataHandler::fetch(unsigned int& data, const char* comment)
{
  return pimpl->fetch(data, comment);
}


int MonitorDataHandler::fetch(long& data, const char* comment)
{
  return pimpl->fetch(data, comment);
}
