/*!
  \file
  \brief �V���A���ʐM�̎����� (Linux, Mac)

  \author Satofumi KAMIMURA

  $Id: SerialDevice_lin.cpp 1995 2012-10-30 02:28:34Z satofumi $
*/

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <string>


class RawSerialDevice
{
    enum {
        InvalidFd = -1,
    };

    std::string error_message_;
    int fd_;
    struct termios sio_;        //!< �ʐM�^�[�~�i������
    fd_set rfds_;               //!< �^�C���A�E�g����


    bool waitReceive(int timeout)
    {
        // �^�C���A�E�g�ݒ�
        FD_ZERO(&rfds_);
        FD_SET(fd_, &rfds_);

        struct timeval tv;
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        if (select(fd_ + 1, &rfds_, NULL, NULL,
                   (timeout < 0) ? NULL : &tv) <= 0) {
            /* �^�C���A�E�g���� */
            return false;
        }
        return true;
    }


public:
    RawSerialDevice(void) : error_message_("no error."), fd_(InvalidFd)
    {
    }


    const char* what(void)
    {
        return error_message_.c_str();
    }


    bool connect(const char* device, long baudrate)
    {
#ifndef MAC_OS
        enum { O_EXLOCK = 0x0 }; // Linux �ł͎g���Ȃ��̂Ń_�~�[���쐬���Ă���
#endif
        fd_ = open(device, O_RDWR | O_EXLOCK | O_NONBLOCK | O_NOCTTY);
        if (fd_ < 0) {
            std::string deviceStr(device);
            error_message_ = deviceStr + ": " + strerror(errno);
            return false;
        }
        int flags = fcntl(fd_, F_GETFL, 0);
        fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK);

        // �V���A���ʐM�̏���
        tcgetattr(fd_, &sio_);
        sio_.c_iflag = 0;
        sio_.c_oflag = 0;
        sio_.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
        sio_.c_cflag |= CS8 | CREAD | CLOCAL;
        sio_.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);

        sio_.c_cc[VMIN] = 0;
        sio_.c_cc[VTIME] = 0;

        // �{�[���[�g�̕ύX
        if (! setBaudrate(baudrate)) {
            return false;
        }
        return true;
    }


    void disconnect(void)
    {
        if (fd_ != InvalidFd) {
            close(fd_);
            fd_ = InvalidFd;
        }
    }


    bool isConnected(void)
    {
        return (fd_ == InvalidFd) ? false : true;
    }


    bool setBaudrate(long baudrate)
    {
        long baudrate_value = -1;
        enum { ErrorMessageSize = 256 };
        char error_message[ErrorMessageSize];

        switch (baudrate) {

        case 4800:
            baudrate_value = B4800;
            break;

        case 9600:
            baudrate_value = B9600;
            break;

        case 19200:
            baudrate_value = B19200;
            break;

        case 38400:
            baudrate_value = B38400;
            break;

        case 57600:
            baudrate_value = B57600;
            break;

        case 115200:
            baudrate_value = B115200;
            break;

        default:
            sprintf(error_message, "No handle baudrate value: %ld", baudrate);
            error_message_ = std::string(error_message);
            return false;
        }

        /* �{�[���[�g�ύX */
        cfsetospeed(&sio_, baudrate_value);
        cfsetispeed(&sio_, baudrate_value);
        tcsetattr(fd_, TCSANOW, &sio_);
        flush();

        return true;
    }


    int send(const char* data, int count)
    {
        if (! isConnected()) {
            error_message_ = "no connection.";
            return 0;
        }
        return write(fd_, data, count);
    }


    int receive(char buffer[], int count, int timeout)
    {
        int filled = 0;

        // �w��T�C�Y�̓ǂݏo�����s��
        int n = read(fd_, buffer, count);
        if (n < 0) {
            return n;
        }
        filled += n;

        // ��M���������Ă�����A�߂�
        if (filled >= count) {
            return filled;
        }

        // �^�C���A�E�g�t���œǂݏo�����s��
        while (filled < count) {
            if (! waitReceive(timeout)) {
                break;
            }

            int required_n = count - filled;
            n = read(fd_, &buffer[filled], required_n);
            if (n <= 0) {
                /* �ǂݏo���G���[�B���݂܂ł̎�M���e�Ŗ߂� */
                break;
            }
            filled += n;
        }
        return filled;
    }


    void flush(void)
    {
        tcdrain(fd_);
        tcflush(fd_, TCIOFLUSH);
    }
};
