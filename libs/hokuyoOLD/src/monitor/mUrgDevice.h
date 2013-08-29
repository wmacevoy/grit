#ifndef QRK_M_URG_DEVICE_H
#define QRK_M_URG_DEVICE_H

/*!
  \file
  \brief ���j�^�Ή��� URG ����

  \author Satofumi KAMIMURA

  $Id: mUrgDevice.h 1841 2010-06-06 12:01:11Z satofumi $
*/

#include "../urg/UrgDevice.h"


namespace qrk
{
  /*!
    \brief ���j�^�Ή��� URG ����
  */
  class mUrgDevice : public RangeSensor
  {
    mUrgDevice(void);
    mUrgDevice(const mUrgDevice& rhs);
    mUrgDevice& operator = (const mUrgDevice& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;

  public:
    /*!
      \brief �I�v�V�����p�p�����[�^
    */
    enum {
      DefaultBaudrate = 115200, //!< �ڑ����̃f�t�H���g�{�[���[�g
      Infinity = 0,             //!< ������̃f�[�^�擾
    };


    /*!
      \brief �R���X�g���N�^

      ���샂�[�h���w�肷��

      - Record ���[�h (-r �w��)
        - �擾�f�[�^�����O�t�@�C���ɏ����o��

      - Play ���[�h (-p �w��)
        - ���O�t�@�C���̃f�[�^���擾�f�[�^�Ƃ��Ĉ���

      �g�p��
      \code
// �v���O�������s���̈�� -r, -p ���w�肷�邱�Ƃœ��삪�ς��B
//
// % ./a.exe -r    �f�[�^���擾���A�L�^����
// % ./a.exe -p    �L�^�ς݂̃f�[�^���Đ�����
//
// �������A�L�^���ƍĐ����ł͓����v���O�������g�����ƁB

#include <mUrgDevice.h>

using namespace qrk;
using namespace std;


int main(int argc, char *argv[])
{
  mUrgDevice urg(argc, argv);

  if (! urg.connect("COM4")) {
    ...
  }

  long timestamp = 0;
  vector<long> data;
  urg.capture(data, &timestamp);

  ...
} \endcode

      \see UrgDevice
    */
    explicit mUrgDevice(int argc, char *argv[]);
    ~mUrgDevice(void);

    const char* what(void) const;

    bool connect(const char* device, long baudrate = DefaultBaudrate);
    void setConnection(Connection* con);
    Connection* connection(void);
    void disconnect(void);
    bool isConnected(void) const;

    long minDistance(void) const;
    long maxDistance(void) const;
    int maxScanLines(void) const;

    void setRetryTimes(size_t times);

    int scanMsec(void) const;

    void setCaptureMode(RangeCaptureMode mode);
    RangeCaptureMode captureMode(void);

    void setCaptureRange(int begin_index, int end_index);
    void setCaptureFrameInterval(size_t interval);
    void setCaptureTimes(size_t times);
    size_t remainCaptureTimes(void);
    void setCaptureSkipLines(size_t skip_lines);

    int capture(std::vector<long>& data, long* timestamp = NULL);
    int captureWithIntensity(std::vector<long>& data,
                             std::vector<long>& intensity_data,
                             long* timestamp = NULL);
    void stop(void);

    bool setTimestamp(int ticks = 0, int* response_msec = NULL,
                      int* force_delay_msec = NULL);

    bool setLaserOutput(bool on);

    double index2rad(const int index) const;
    int rad2index(const double radian) const;

    void setParameter(const RangeSensorParameter& parameter);
    RangeSensorParameter parameter(void) const;
    bool loadParameter(void);

    bool versionLines(std::vector<std::string>& lines);

    void reboot(void);
  };
}

#endif /* !QRK_M_URG_DEVICE_H */
