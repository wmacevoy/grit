/*!
  \file
  \brief ���j�^�Ή��� URG ����

  \author Satofumi KAMIMURA

  $Id: mUrgDevice.cpp 1841 2010-06-06 12:01:11Z satofumi $
*/

#include "mUrgDevice.h"
#include "../urg/RangeSensorParameter.h"
#include "UrgServer.h"
#include "../connection/SerialDevice.h"
#include "../connection/TcpipSocket.h"
#include "MonitorEventScheduler.h"
#include "MonitorDataHandler.h"
#include "LogNameHolder.h"
#include "DeviceIpManager.h"
#include "mConnection.h"

using namespace qrk;
using namespace std;


namespace
{
  class UrgDeviceForRecord : public UrgDevice
  {
  public:
    MonitorModeManager::MonitorMode mode_;
    MonitorDataHandler* handler_;


    UrgDeviceForRecord(void)
    : handler_(NULL)
    , mode_ (MonitorModeManager::Undefined)
    {
    }


    ~UrgDeviceForRecord(void)
    {
      delete handler_;
    }


    void captureReceived(void)
    {
      // �_�~�[�f�[�^���L�^���A�擾�^�C�~���O�𓯊���
      if ((mode_ == MonitorModeManager::Record) ||
          (mode_ == MonitorModeManager::Play)) {
        bool dummy = false;
        handler_->fetch(dummy);
      }
    }
  };
}


struct mUrgDevice::pImpl
{
  MonitorModeManager::MonitorMode mode_;
  UrgDeviceForRecord* urg_;
  Connection* monitor_con_;
  Connection* device_con_;
  MonitorDataHandler* handler_;


  pImpl(void)
  : urg_(NULL)
  , monitor_con_(NULL)
  , device_con_(NULL)
  , handler_(NULL)
  , mode_(MonitorModeManager::Undefined)
  {
  }


  ~pImpl(void)
  {
    delete urg_;

    if (monitor_con_) {
      delete monitor_con_;
    }
    if (device_con_) {
      delete device_con_;
    }
    delete handler_;
  }


  void initializeConnection(int argc, char *argv[])
  {
    MonitorModeManager::object()->setMode(argc, argv);
    mode_ = MonitorModeManager::object()->mode();
    urg_->mode_ = mode_;

    // mConnection �p�̃��O�Ǘ�
    if (mode_ != MonitorModeManager::Simulation) {
      device_con_ = new SerialDevice;
    } else {
      device_con_ = new TcpipSocket;
    }
    monitor_con_ = new mConnection(device_con_);
    urg_->setConnection(monitor_con_);

    // mUrgDevice �p�̃��O�Ǘ�
    string log_name = LogNameHolder::object()->name("mUrgDevice");
    handler_ = new MonitorDataHandler(log_name, mode_);

    // �f�[�^�擾�X���b�h�p�̃��O�Ǘ�
    log_name = LogNameHolder::object()->name("mUrgDevice_thread");
    urg_->handler_ = new MonitorDataHandler(log_name, mode_);
  }
};


mUrgDevice::mUrgDevice(int argc, char *argv[]) : pimpl(new pImpl)
{
  pimpl->urg_ = new UrgDeviceForRecord;
  pimpl->initializeConnection(argc, argv);
}


mUrgDevice::~mUrgDevice(void)
{
}


const char* mUrgDevice::what(void) const
{
  return pimpl->urg_->what();
}


bool mUrgDevice::connect(const char* device, long baudrate)
{
  MonitorModeManager::MonitorMode Simulation = MonitorModeManager::Simulation;
  if (MonitorModeManager::object()->mode() == Simulation) {
    // device ���� IP �|�[�g�擾���A�T�[�o���N������
    long port = DeviceIpManager::object()->createIpPort(device);
    if (port > 0) {
      // IP �|�[�g���擾�ς݂łȂ��ꍇ�̂݁A�T�[�o���N������
      // !!! URG �̌^�A�V���A�� ID �̏����A��Ŕ��f������ׂ�
      UrgServer* urg_server = new UrgServer;
      if (! urg_server->activate(port)) {
        // �T�[�o���N���ł��Ȃ���΁A�ڑ��𒆒f����
        return false;
      }
      MonitorEventScheduler::object()->registerDeviceServer(urg_server);
    }
  }
  return pimpl->urg_->connect(device, baudrate);
}


void mUrgDevice::setConnection(Connection* con)
{
  delete pimpl->monitor_con_;
  pimpl->monitor_con_ = new mConnection(con);
  pimpl->urg_->setConnection(pimpl->monitor_con_);
}


Connection* mUrgDevice::connection(void)
{
  return pimpl->urg_->connection();
}


void mUrgDevice::disconnect(void)
{
  pimpl->urg_->disconnect();
}


bool mUrgDevice::isConnected(void) const
{
  return pimpl->urg_->isConnected();
}


long mUrgDevice::minDistance(void) const
{
  return pimpl->urg_->minDistance();
}


long mUrgDevice::maxDistance(void) const
{
  return pimpl->urg_->maxDistance();
}


int mUrgDevice::maxScanLines(void) const
{
  return pimpl->urg_->maxScanLines();
}


void mUrgDevice::setRetryTimes(size_t times)
{
  return pimpl->urg_->setRetryTimes(times);
}


int mUrgDevice::scanMsec(void) const
{
  return pimpl->urg_->scanMsec();
}


void mUrgDevice::setCaptureMode(RangeCaptureMode mode)
{
  pimpl->urg_->setCaptureMode(mode);
}


RangeCaptureMode mUrgDevice::captureMode(void)
{
  return pimpl->urg_->captureMode();
}


void mUrgDevice::setCaptureRange(int begin_index, int end_index)
{
  pimpl->urg_->setCaptureRange(begin_index, end_index);
}


void mUrgDevice::setCaptureFrameInterval(size_t interval)
{
  pimpl->urg_->setCaptureFrameInterval(interval);
}


void mUrgDevice::setCaptureTimes(size_t times)
{
  pimpl->urg_->setCaptureTimes(times);
}


size_t mUrgDevice::remainCaptureTimes(void)
{
  return pimpl->urg_->remainCaptureTimes();
}


void mUrgDevice::setCaptureSkipLines(size_t skip_lines)
{
  pimpl->urg_->setCaptureSkipLines(skip_lines);
}


int mUrgDevice::capture(vector<long>& data, long* timestamp)
{
  // �_�~�[�f�[�^���L�^���A�擾�^�C�~���O�𓯊���
  if ((pimpl->mode_ == MonitorModeManager::Record) ||
      (pimpl->mode_ == MonitorModeManager::Play)) {
    bool dummy = false;
    pimpl->handler_->fetch(dummy);
  }
  return pimpl->urg_->capture(data, timestamp);
}


int mUrgDevice::captureWithIntensity(vector<long>& data,
                                   vector<long>& intensity_data,
                                   long* timestamp)
{
  // �_�~�[�f�[�^���L�^���A�擾�^�C�~���O�𓯊���
  if ((pimpl->mode_ == MonitorModeManager::Record) ||
      (pimpl->mode_ == MonitorModeManager::Play)) {
    bool dummy = false;
    pimpl->handler_->fetch(dummy);
  }
  return pimpl->urg_->captureWithIntensity(data, intensity_data, timestamp);
}


void mUrgDevice::stop(void)
{
  pimpl->urg_->stop();
}


bool mUrgDevice::setTimestamp(int ticks, int* response_msec,
                            int* force_delay_msec)
{
  if ((pimpl->mode_ == MonitorModeManager::Record) ||
      (pimpl->mode_ == MonitorModeManager::Play)) {
    pimpl->handler_->fetch(ticks);
  }

  int recorded = (force_delay_msec) ? *force_delay_msec : 0;
  if (pimpl->mode_ == MonitorModeManager::Play) {
    pimpl->handler_->fetch(recorded);
  }

  int delay_msec = 0;
  bool ret = pimpl->urg_->setTimestamp(ticks, &delay_msec, &recorded);
  if (response_msec) {
    *response_msec = delay_msec;
  }
  if (pimpl->mode_ == MonitorModeManager::Record) {
    pimpl->handler_->fetch(delay_msec);
  }
  return ret;
}


bool mUrgDevice::setLaserOutput(bool on)
{
  return pimpl->urg_->setLaserOutput(on);
}


double mUrgDevice::index2rad(const int index) const
{
  return pimpl->urg_->index2rad(index);
}


int mUrgDevice::rad2index(const double radian) const
{
  return pimpl->urg_->rad2index(radian);
}


void mUrgDevice::setParameter(const RangeSensorParameter& parameter)
{
  pimpl->urg_->setParameter(parameter);
}


RangeSensorParameter mUrgDevice::parameter(void) const
{
  return pimpl->urg_->parameter();
}


bool mUrgDevice::versionLines(vector<string>& lines)
{
  return pimpl->urg_->versionLines(lines);
}


bool mUrgDevice::loadParameter(void)
{
  return pimpl->urg_->loadParameter();
}


void mUrgDevice::reboot(void)
{
  pimpl->urg_->reboot();
}
