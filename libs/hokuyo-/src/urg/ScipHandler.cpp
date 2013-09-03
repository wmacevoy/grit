/*!
  \file
  \brief SCIP �ʐM����

  \author Satofumi KAMIMURA

  $Id: ScipHandler.cpp 1977 2012-04-05 08:04:51Z satofumi $
*/

#include "ScipHandler.h"
#include "RangeSensorParameter.h"
#include "../connection/Connection.h"
#include "../connection/ConnectionUtils.h"
#include "../system/ticks.h"
#include "../system/delay.h"
#include "../system/DetectOS.h"
#include "../system/log_printf.h"
#include <cstring>
#include <cstdio>

#ifdef MSC
#define snprintf _snprintf
#endif

using namespace qrk;
using namespace std;


namespace
{
  typedef enum {
    ProcessContinue,
    ProcessBreak,
    ProcessNormal,
  } LoopProcess;

  void clearReceived(vector<long>& data, CaptureType& type,
                     int& line_count, int& timeout,
                     string& remain_string,
                     string& left_packet_data)
  {
    data.clear();
    type = TypeUnknown;
    line_count = 0;
    timeout = 0;
    remain_string.clear();
    left_packet_data.clear();
  }
}


struct ScipHandler::pImpl
{
  enum {
    TotalTimeout = 1000,        // [msec]
    ContinuousTimeout = 100,    // [msec]
    FirstTimeout = 1000,        // [msec]

    BufferSize = 64 + 1 + 1,    // �f�[�^�� + �`�F�b�N�T�� + ��s

    ResponseTimeout = -1,
    MismatchResponse = -2,
    SendFail = -3,
    ChecksumFail = -4,
    Scip11Response = -14,

    InvalidRange = -1,
  };

  typedef enum {
    LaserUnknown = 0,
    LaserOn,
    LaserOff,
  } LaserState;

  string error_message_;
  Connection* con_;
  LaserState laser_state_;
  bool mx_capturing_;

  bool isPreCommand_QT_;


  pImpl(void)
    : error_message_("no error."), con_(NULL), laser_state_(LaserUnknown),
      mx_capturing_(false), isPreCommand_QT_(false)
  {
  }


  bool connect(const char* device, long baudrate)
  {
    if (! con_->connect(device, baudrate)) {
      error_message_ = con_->what();
      return false;
    }

    long try_baudrates[] = { 115200, 19200, 38400, };
    size_t try_size = sizeof(try_baudrates) / sizeof(try_baudrates[0]);

    // �ڑ��������{�[���[�g��z��̐擪�Ɠ�ꊷ����
    for (size_t i = 1; i < try_size; ++i) {
      if (baudrate == try_baudrates[i]) {
        swap(try_baudrates[0], try_baudrates[i]);
        break;
      }
    }

    // �w��̃{�[���[�g�Őڑ����A�������Ԃ���邩�ǂ�������
    for (size_t i = 0; i < try_size; ++i) {

      // �z�X�g���̃{�[���[�g��ύX
      if (! con_->setBaudrate(try_baudrates[i])) {
        error_message_ = con_->what();
        return false;
      }

      // �O�񕪂̎�M�p�P�b�g��ǂݎ̂�
      con_->clear();

      // QT �̔��s
      int return_code = -1;
      char qt_expected_response[] = { 0, -1 };
      // return_code ���g���������߁AsetLaserOutput() ��p������ QT �𑗐M����
      if (response(return_code, "QT\n", qt_expected_response)) {
        laser_state_ = LaserOff;
        return changeBothBaudrate(baudrate);

      } else if (return_code == ResponseTimeout) {
        // �{�[���[�g������Ă��āA�ʐM�ł��Ȃ������Ƃ݂Ȃ�
        error_message_ = "baudrate is not detected.";
        continue;

      } else if (return_code == MismatchResponse) {
        // MD/MS �̉����Ƃ݂Ȃ��A��M�f�[�^��ǂݔ�΂�
	con_->clear();
        skip(con_, ContinuousTimeout);
        return changeBothBaudrate(baudrate);

      } else if (return_code == Scip11Response) {
        // SCIP1.1 �v���g�R���̏ꍇ�̂݁ASCIP2.0 �𑗐M����
        char scip20_expected_response[] = { 0, -1 };
        if (! response(return_code, "SCIP2.0\n", scip20_expected_response)) {
          error_message_ =
            "SCIP1.1 protocol is not supported. Please update URG firmware, or reconnect after a few seconds because sensor is booting.";
          return false;
        }
        laser_state_ = LaserOff;
        return changeBothBaudrate(baudrate);

      } else if (return_code == 0xE) {
        // TM ���[�h�Ƃ݂Ȃ��ATM2 �𔭍s����
        char tm2_expected_response[] = { 0, -1 };
        if (response(return_code, "TM2\n", tm2_expected_response)) {
          laser_state_ = LaserOff;
          return changeBothBaudrate(baudrate);
        }
      }
    }

    con_->disconnect();
    return false;
  }


  bool changeBothBaudrate(long baudrate)
  {
    // ��ɖڕW�Ώۂ̃{�[���[�g�l�Ȃ�΁A�����Ƃ݂Ȃ�
    // ���̊֐��́AScipHandler::connect() ���A����Ȍ�łȂ��ƌĂ΂�Ȃ�����
    if (con_->baudrate() == baudrate) {
      return true;
    }

    // URG ���̃{�[���[�g��ύX
    int pre_ticks = ticks();
    if (! changeBaudrate(baudrate)) {
      return false;
    }

    // �V���A���ʐM�̏ꍇ�A�{�[���[�g�ύX��A�P����҂K�v������
    int reply_msec = ticks() - pre_ticks;
    delay((reply_msec * 4 / 3) + 10);

    // �z�X�g���̃{�[���[�g��ύX
    return con_->setBaudrate(baudrate);
  }


  bool changeBaudrate(long baudrate)
  {
#if 0
    // Tcpip �ڑ��ɑΉ����邽�߁A�R�����g�A�E�g
    if (! ((baudrate == 19200) || (baudrate == 38400) ||
           (baudrate == 57600) || (baudrate == 115200))) {
      error_message_ = "Invalid baudrate value.";
      return false;
    }
#endif

    // SS �𑗐M���AURG ���̃{�[���[�g��ύX����
    char send_buffer[] = "SSxxxxxx\n";
    snprintf(send_buffer, 10, "SS%06ld\n", baudrate);
    int return_code = -1;
    // !!! ��ɐݒ�Ώۂ̃{�[���[�g�A�̏ꍇ�̖߂�l�� ss_expected... �ɒǉ�����
    char ss_expected_response[] = { 0, 0x3, 0x4, 0xf, -1 };
    if (! response(return_code, send_buffer, ss_expected_response)) {
      error_message_ = "Baudrate change fail.";
      return false;
    }

    return true;
  }


  bool loadParameter(RangeSensorParameter& parameters)
  {
    // PP �̑��M�ƃf�[�^�̎�M
    int return_code = -1;
    char pp_expected_response[] = { 0, -1 };
    vector<string> lines;
    if (! response(return_code, "PP\n", pp_expected_response, &lines)) {
      error_message_ = "PP fail.";
      return false;
    }

    // PP �������e�̊i�[
    if (lines.size() != 8) {
      error_message_ = "Invalid PP response.";
      return false;
    }

    // !!! �`�F�b�N�T���̕]�����s���ׂ�

    int modl_length =
      static_cast<int>(lines[RangeSensorParameter::MODL].size());
    // �ŏ��̃^�O�ƁA�`�F�b�N�T�����������������Ԃ�
    if (modl_length > (5 + 2)) {
      modl_length -= (5 + 2);
    }
    parameters.model = lines[RangeSensorParameter::MODL].substr(5, modl_length);

    parameters.distance_min = substr2int(lines[RangeSensorParameter::DMIN], 5);
    parameters.distance_max = substr2int(lines[RangeSensorParameter::DMAX], 5);
    parameters.area_total = substr2int(lines[RangeSensorParameter::ARES], 5);
    parameters.area_min = substr2int(lines[RangeSensorParameter::AMIN], 5);
    parameters.area_max = substr2int(lines[RangeSensorParameter::AMAX], 5);
    parameters.area_front = substr2int(lines[RangeSensorParameter::AFRT], 5);
    parameters.scan_rpm = substr2int(lines[RangeSensorParameter::SCAN], 5);

    return true;
  }


  int substr2int(const string& line, int from_n, int length = string::npos)
  {
    return atoi(line.substr(from_n, length).c_str());
  }


  bool response(int& return_code, const char send_command[],
                char expected_response[],
                vector<string>* lines = NULL)
  {
    return_code = -1;
    if (! con_) {
      error_message_ = "no connection.";
      return false;
    }

    size_t send_size = strlen(send_command);
    int actual_send_size = con_->send(send_command, send_size);
    if (strncmp(send_command, "QT\n", send_size)) {
      isPreCommand_QT_ = false;
    }
    if (actual_send_size != static_cast<int>(send_size)) {
      return_code = SendFail;
      return false;
    }

    // �G�R�[�o�b�N�̎�M
    char buffer[BufferSize];
    int recv_size = readline(con_, buffer, BufferSize, FirstTimeout);
    if (recv_size < 0) {
      error_message_ = "response timeout.";
      return_code = ResponseTimeout;
      return false;
    }

    // �V���A���ڑ��Ń{�[���[�g�ύX����� 0x00 �́A����O�Ƃ���
    if (! ((recv_size == 1) && (buffer[0] == 0x00))) {
      if ((recv_size != static_cast<int>(send_size - 1)) ||
          (strncmp(buffer, send_command, recv_size))) {
        error_message_ = "mismatch response: " + string(buffer);
        return_code = MismatchResponse;
        return false;
      }
    }

    // �����̎�M
    // !!! ��L�̏����ƂȂ�ׂ����ʂɂ���
    // !!! SCIP1.1 �v���g�R���̉����́A������t��������� return_code �Ɋi�[����
    recv_size = readline(con_, buffer, BufferSize, ContinuousTimeout);
    if (recv_size < 0) {
      // !!! ���̏�����������
      error_message_ = "response timeout.";
      return_code = ResponseTimeout;
      return false;
    }
    if (recv_size == 3) {
      // �R�����Ȃ�΁ASCIP2.0 �Ƃ݂Ȃ��ă`�F�b�N�T�����m�F����
      // !!! �`�F�b�N�T���̊m�F
      if (! checkSum(buffer, recv_size - 1, buffer[recv_size - 1])) {
	return_code = ChecksumFail;
	return false;
      }
      buffer[2] = '\0';
      return_code = strtol(buffer, NULL, 16);

    } else if (recv_size == 1) {
      // �P�����Ȃ�΁ASCIP1.1 �Ƃ݂Ȃ��� 16�i�ϊ������l�ɕ��������ĕԂ�
      buffer[1] = '\0';
      return_code = -strtol(buffer, NULL, 16);
    }

    // �f�[�^�̈�̎�M
    // �P�s�ǂݏo���A��s�݂̂Ȃ�ΏI���Ƃ݂Ȃ�
    do {
      recv_size = readline(con_, buffer, BufferSize, ContinuousTimeout);
      if (lines && (recv_size > 0)) {
        lines->push_back(buffer);
      }
    } while (recv_size > 0);

    for (int i = 0; expected_response[i] != -1; ++i) {
      if (return_code == expected_response[i]) {
        return true;
      }
    }
    return false;
  }


  bool setLaserOutput(bool on, bool force)
  {
    if (((on == true) && (laser_state_ == LaserOn)) ||
        ((on == false) && (laser_state_ == LaserOff))) {
      if (! force) {
        // ���[�U�o�͂����݂̏�ԂƓ����Ȃ�Ζ߂�
        // �����ݒ�t���O�� true �̂Ƃ��͖߂炸�ɐݒ���s��
        return true;
      }
    }
    if ((!on) && isPreCommand_QT_) {
      return false;
    }

    if (on) {
      int return_code = -1;
      char expected_response[] = { 0, -1 };
      if (! response(return_code, "BM\n", expected_response)) {
        error_message_ = "BM fail.";
        return false;
      }
      laser_state_ = LaserOn;
      return true;

    } else {
      // "QT"
      if (! mx_capturing_) {
        // �������邽�߂� QT �ł́A������҂ׂ�
        int return_code = -1;
        char qt_expected_response[] = { 0, -1 };
        if (! response(return_code, "QT\n", qt_expected_response)) {
          return false;
        }
        laser_state_ = LaserOff;
        isPreCommand_QT_ = true;
        return true;

      } else {
        // MD �𒆒f���邽�߂� QT �ł́A������҂��Ă͂Ȃ�Ȃ�
        // �����́A��M�X���b�h���ŏ��������
        con_->send("QT\n", 3);
        isPreCommand_QT_ = true;
      }

      return true;
    }
  }


  bool testChecksum(const char* buffer, int line_size,
                    vector<long>& data, CaptureType& type,
                    int line_count, int timeout,
                    string& remain_string, string& left_packet_data)
  {
    if (! checkSum(buffer, line_size - 1, buffer[line_size - 1])) {
      log_printf("checksum error: %s\n", buffer);
      // return InvalidData;
      // !!! URG �̃p�P�b�g�G���[���Ȃ��Ȃ�����A���̎����ɖ߂�

      // !!! ���݂��邾���̃p�P�b�g��ǂݔ�΂�
      error_message_ = "invalid packet.";
      clearReceived(data, type, line_count, timeout,
                    remain_string, left_packet_data);
      return false;
    }

    return true;
  }


  LoopProcess handleEchoback(const char* buffer, CaptureSettings& settings,
                             CaptureType& type, vector<long>& data,
                             int line_count, int timeout,
                             string& remain_string,
                             string& left_packet_data)
  {
    string line = buffer;
    if ((! line.compare(0, 2, "GD")) || (! line.compare(0, 2, "GS"))) {
      if (! parseGdEchoback(settings, line)) {
        return ProcessBreak;
      }
      type = (line[1] == 'D') ? GD : GS;

    } else if ((! line.compare(0, 2, "MD")) ||
               (! line.compare(0, 2, "MS"))) {
      if (! parseMdEchoback(settings, line)) {
        return ProcessBreak;
      }
      type = (line[1] == 'D') ? MD : MS;
      laser_state_ = LaserOn;

    } else if (! line.compare(0, 2, "ME")) {
      if (! parseMeEchoback(settings, line)) {
        return ProcessBreak;
      }
      type = ME;
      laser_state_ = LaserOn;

    } else if (! line.compare(0, 2, "QT")) {
      settings.remain_times = 0;
      laser_state_ = LaserOff;
      mx_capturing_ = false;
      return ProcessNormal;

    } else {
      //return InvalidData;
      // !!! URG ������ȃp�P�b�g��Ԃ��悤�ɂȂ�����A���̎����ɖ߂�

      clearReceived(data, type, line_count, timeout,
                    remain_string, left_packet_data);
      //fprintf(stderr, "invalid data: %s\n", buffer);
      return ProcessContinue;
    }

    data.reserve(settings.capture_last + 1);
    return ProcessNormal;
  }


  void handleReturnCode(char* buffer, CaptureSettings& settings, int timeout,
                        CaptureType& type, int* total_times)
  {
    // !!! ������ 2 + 1 ���̃`�F�b�N�����ׂ�
    buffer[2] = '\0';
    settings.error_code = atoi(buffer);

    if (settings.error_code == 10) {
      // ���[�U���������o
      laser_state_ = pImpl::LaserOff;
    }

    // "0B" ���Ԃ��ꂽ�ꍇ�A�Z���T�ƃz�X�g�̉���������Ă���\��������̂�
    // ����������ǂݎ̂Ă�
    if (! strncmp(buffer, "0B", 2)) {
      skip(con_, TotalTimeout, timeout);
    }

    // !!! "00P" �Ƃ̔�r�����ׂ�
    if ((settings.error_code == 0) &&
        ((type == MD) || (type == MS) || (type == ME))) {
      if (total_times) {
        *total_times = settings.remain_times;
      }
      type = Mx_Reply;
    }
  }


  CaptureType receiveCaptureData(vector<long>& data,
                                 CaptureSettings& settings, long* timestamp,
                                 int* remain_times, int* total_times)
  {
    int line_count = 0;
    data.clear();

    string remain_string;

    string left_packet_data;
    char buffer[BufferSize];

    error_message_ = "no response.";

    CaptureType type = TypeUnknown;
    int timeout = FirstTimeout;
    int line_size = 0;
    while ((line_size = readline(con_, buffer, BufferSize, timeout)) > 0) {
      //fprintf(stderr, "%d: % 3d: %s\n", ticks(), line_count, buffer);

      // �`�F�b�N�T���̊m�F
      if (line_count != 0) {
        // �G�R�[�o�b�N�ɂ̓`�F�b�N�T�������񂪂Ȃ��̂ŁA����
        if (! testChecksum(buffer, line_size, data, type, line_count, timeout,
                           remain_string, left_packet_data)) {
          continue;
        }
      }

      if (line_count == 0) {
        // �G�R�[�o�b�N
        LoopProcess loop_process =
          handleEchoback(buffer, settings, type, data, line_count, timeout,
                         remain_string, left_packet_data);
        if (loop_process == ProcessContinue) {
          continue;

        } else if (loop_process == ProcessBreak) {
          break;
        }

      } else if (line_count == 1) {
        // �����R�[�h
        handleReturnCode(buffer, settings, timeout, type, total_times);

      } else if (line_count == 2) {
        // �^�C���X�^���v
        if (timestamp) {
          *timestamp = decode(buffer, 4);
        }
      } else {
        if (line_count == 3) {
          // ��M�f�[�^���Ȃ��擪����̗̈���A�_�~�[�f�[�^�Ŗ��߂�
          for (int i = 0; i < settings.capture_first; ++i) {
            data.push_back(InvalidRange);
            if (type == ME) {
              // ME ��M�̂Ƃ��́A���x�f�[�^�������߂�
              data.push_back(InvalidRange);
            }
          }
        }
        // �����f�[�^�̊i�[
        left_packet_data =
          addLengthData(data, string(buffer), left_packet_data,
                        settings.data_byte, settings.skip_lines);
      }
      ++line_count;
      timeout = ContinuousTimeout;
    }

    // !!! type �������f�[�^�擾�̂Ƃ��́A����Ɏ�M�������������A���m�F���ׂ�

    // ME �� "�܂Ƃ߂鐔" �ݒ�ȏ�̃f�[�^���Ԃ����o�O�ɑΏ�
    size_t expected_n = settings.capture_last * ((type == ME) ? 2 : 1);
    if (expected_n < data.size()) {
      data.erase(data.begin() + expected_n, data.end());
    }

    if (remain_times) {
      *remain_times = settings.remain_times;
    }
    return type;
  }


  bool parseGdEchoback(CaptureSettings& settings, const string& line)
  {
    if (line.size() != 12) {
      error_message_ = "Invalid Gx packet has arrived.";
      return false;
    }

    settings.capture_first = substr2int(line, 2, 4);
    settings.capture_last = substr2int(line, 6, 4) + 1;
    int skip_lines = substr2int(line, 10, 2);
    settings.skip_lines = (skip_lines == 0) ? 1 : skip_lines;
    settings.data_byte = (line[1] == 'D') ? 3 : 2;

    return true;
  }


  bool parseMdEchoback(CaptureSettings& settings, const string& line)
  {
    if (line.size() != 15) {
      error_message_ = "Invalid Mx packet has arrived.";
      return false;
    }

    settings.capture_first = substr2int(line, 2, 4);
    settings.capture_last = substr2int(line, 6, 4) + 1;
    int skip_lines = substr2int(line, 10, 2);
    settings.skip_lines = (skip_lines == 0) ? 1 : skip_lines;
    settings.skip_frames = substr2int(line, 12, 1);
    settings.remain_times = substr2int(line, 13, 2);
    settings.data_byte = (line[1] == 'D') ? 3 : 2;

    if (settings.remain_times == 1) {
      // �Ō�̃f�[�^�擾�ŁA���[�U�����������ɂ���
      // �{���́A���̃f�[�^�擾��Ƀ��[�U�͏�������Ă���
      // 1 �Ŕ��肷��ƁA�擾�񐔂� 1 �̂Ƃ��ɂ�����ɓ��삷�邽��
      mx_capturing_ = false;

    } else {
      if (settings.remain_times > 0) {
        mx_capturing_ = true;
      } else if (settings.remain_times == 0) {
        settings.remain_times = 100;
      }
    }

    return true;
  }


  bool parseMeEchoback(CaptureSettings& settings, const string& line)
  {
    if (line.size() != 15) {
      error_message_ = "Invalid ME packet has arrived.";
      return false;
    }

    settings.capture_first = substr2int(line, 2, 4);
    settings.capture_last = substr2int(line, 6, 4) + 1;
    int skip_lines = substr2int(line, 10, 2);
    settings.skip_lines = (skip_lines == 0) ? 1 : skip_lines;
    settings.skip_frames = substr2int(line, 12, 1);
    settings.remain_times = substr2int(line, 13, 2);
    settings.data_byte = 3;

    if (settings.remain_times == 1) {
      mx_capturing_ = false;

    } else {
      mx_capturing_ = true;
    }

    return true;
  }


  string addLengthData(vector<long>& data,
                       const string& line,
                       const string& left_packet_data,
                       const size_t data_byte, const int skip_lines = 1)
  {
    if (line.empty()) {
      // ��s�̏ꍇ�A�߂�
      return left_packet_data;
    }

    // �[���B����ɏ������镪
    string left_byte = left_packet_data;

    size_t data_size = (left_byte.size() + (line.size() - 1)) / data_byte;
    size_t n = data_size * data_byte - left_byte.size();
    for (size_t i = 0; i < n; ++i) {
      left_byte.push_back(line[i]);
      if (left_byte.size() >= data_byte) {
        // �f�[�^�������ɕϊ����āA�i�[
        long length = decode(&left_byte[0], data_byte);
        for (int j = 0; j < skip_lines; ++j) {
          data.push_back(length);
        }
        left_byte.clear();
      }
    }
    left_byte += line.substr(n, (line.size() - n) - 1);

    return left_byte;
  }
};


ScipHandler::ScipHandler(void) : pimpl(new pImpl)
{
}


ScipHandler::~ScipHandler(void)
{
}


const char* ScipHandler::what(void) const
{
  return pimpl->error_message_.c_str();
}


long ScipHandler::decode(const char* data, size_t size)
{
  const char* p = data;
  const char* last_p = p + size;

  int value = 0;
  while (p < last_p) {
    value <<= 6;
    value &= ~0x3f;
    value |= *p++ - 0x30;
  }
  return value;
}


bool ScipHandler::checkSum(const char* buffer, int size, char actual_sum)
{
  const char* p = buffer;
  const char* last_p = p + size;

  char expected_sum = 0x00;
  while (p < last_p) {
    expected_sum += *p++;
  }
  expected_sum = (expected_sum & 0x3f) + 0x30;

  return (expected_sum == actual_sum) ? true : false;
}


void ScipHandler::setConnection(Connection* con)
{
  pimpl->con_ = con;
}


Connection* ScipHandler::connection(void)
{
  return pimpl->con_;
}


bool ScipHandler::connect(const char* device, long baudrate)
{
  return pimpl->connect(device, baudrate);
}


int ScipHandler::send(const char data[], int size)
{
  if (size >= 2) {
    // �R�}���h���M��̎�M�O���ƁA���[�U�_�����̔��肪�ł��Ȃ����߁A
    // �����Ń��[�U�_�����̏�Ԃ𔻒肷��
    if ((! strncmp("MD", data, 2)) || (! strncmp("MS", data, 2)) ||
        (! strncmp("ME", data, 2))) {
      pimpl->laser_state_ = pImpl::LaserOn;
      pimpl->mx_capturing_ = true;
      pimpl->isPreCommand_QT_ = false;
    }
  }
  return pimpl->con_->send(data, size);
}


int ScipHandler::recv(char data[], int size, int timeout)
{
  return pimpl->con_->receive(data, size, timeout);
}


bool ScipHandler::loadParameter(RangeSensorParameter& parameters)
{
  return pimpl->loadParameter(parameters);
}


bool ScipHandler::versionLines(vector<string>& lines)
{
  int return_code = -1;
  char expected_response[] = { 0, -1 };
  if (! pimpl->response(return_code, "VV\n", expected_response, &lines)) {
    return false;
  }
  return true;
}


bool ScipHandler::setRawTimestampMode(bool on)
{
  char send_command[] = "TMx\n";
  send_command[2] = (on) ? '0' : '2';

  // TM0 or TM2 �̑��M
  int return_code = -1;
  char expected_response[] = { 0, -1 };
  if (! pimpl->response(return_code, send_command, expected_response)) {
    pimpl->error_message_ = (on) ? "TM0 fail." : "TM2 fail.";
    return false;
  }

  // TM1, TM2 �̉���������Ȃ�΁A���[�U�͏������Ă���͂�
  pimpl->laser_state_ = pImpl::LaserOff;

  return true;
}


bool ScipHandler::rawTimestamp(int* timestamp)
{
  // TM1 �̒l��Ԃ�
  int return_code = -1;
  char expected_response[] = { 0, -1 };
  vector<string> lines;
  if (! pimpl->response(return_code, "TM1\n", expected_response, &lines)) {
    pimpl->error_message_ = "TM1 fail.";
    return false;
  }

  if ((lines.size() != 1) || (lines[0].size() != 5)) {
    pimpl->error_message_ = "response mismatch.";
    return false;
  }

  *timestamp = decode(lines[0].c_str(), 4);
  return true;
}


bool ScipHandler::setLaserOutput(bool on, bool force)
{
  return pimpl->setLaserOutput(on, force);
}


CaptureType ScipHandler::receiveCaptureData(vector<long>& data,
                                            CaptureSettings& settings,
                                            long* timestamp, int* remain_times,
                                            int* total_times)
{
  return pimpl->receiveCaptureData(data, settings,
                                   timestamp, remain_times, total_times);
}
