/*!
  \file
  \brief COM �|�[�g�ꗗ�̎擾

  \author Satofumi KAMIMURA

  $Id: FindComPorts.cpp 1977 2012-04-05 08:04:51Z satofumi $
*/

#include "FindComPorts.h"
#include "../system/DetectOS.h"
#ifdef WINDOWS_OS
#include <windows.h>
#include <setupapi.h>
#include <deque>
#include <algorithm>

#if defined(MSC)
#pragma comment(lib, "setupapi.lib")
#endif

#else
#include <deque>
#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#endif

using namespace qrk;
using namespace std;


struct FindComPorts::pImpl
{
    vector<string> base_names_;
    vector<string> driver_names_;


    pImpl(void)
    {
        // Windows �̓��W�X�g����񂾂����� COM �ꗗ�̒T�����s��
        // ����āA�ȍ~�̐ݒ�� Linux, MacOS �݂̂Ŏg����
#if defined(LINUX_OS)
        base_names_.push_back("/dev/ttyUSB");
        base_names_.push_back("/dev/usb/ttyUSB");
#elif defined(MAC_OS)
        base_names_.push_back("/dev/tty.usbmodem");
#endif
    }


    void addBaseName(const char* base_name)
    {
        base_names_.insert(base_names_.begin(), base_name);
    }


    void addDriverName(const char* driver_name)
    {
        driver_names_.push_back(driver_name);
    }


    void orderByDriver(deque<string>& ports, deque<string>& drivers,
                       const char* driver_name,
                       bool all_ports)
    {
        if (ports.empty()) {
            return;
        }

        vector<string>::const_iterator it =
            std::find(driver_names_.begin(), driver_names_.end(), driver_name);
        if (it != driver_names_.end()) {
            ports.push_front(ports.back());
            ports.pop_back();

            drivers.push_front(drivers.back());
            drivers.pop_back();

        } else {
            if (! all_ports) {
                ports.pop_back();
                drivers.pop_back();
            }
        }
    }


    void addFoundPorts(vector<string>& found_ports,
                       const string& port)
    {
        for (vector<string>::iterator it = found_ports.begin();
             it != found_ports.end(); ++it) {

            // !!! �f�[�^�\���� map �ɂ��ׂ�����
            // !!! �����̃A���S���Y����p����ׂ�

            if (! port.compare(*it)) {
                return;
            }
        }
        found_ports.push_back(port);
    }
};


FindComPorts::FindComPorts(void) : pimpl(new pImpl)
{
}


FindComPorts::~FindComPorts(void)
{
}


void FindComPorts::clearBaseNames(void)
{
    pimpl->base_names_.clear();
}


void FindComPorts::addBaseName(const char* base_name)
{
    pimpl->addBaseName(base_name);
}


vector<string> FindComPorts::baseNames(void)
{
    return pimpl->base_names_;
}


void FindComPorts::addDriverName(const char* driver_name)
{
    pimpl->addDriverName(driver_name);
}


#ifdef WINDOWS_OS
// Windows �̏ꍇ
size_t FindComPorts::find(std::vector<std::string>& ports,
                          std::vector<std::string>& driver_names,
                          bool all_ports)
{
    // �f�o�C�X�}�l�[�W���̈ꗗ���� COM �f�o�C�X��T��
    deque<string> found_ports;
    deque<string> found_drivers;

    //4D36E978-E325-11CE-BFC1-08002BE10318
    GUID GUID_DEVINTERFACE_COM_DEVICE =
        {0x4D36E978L, 0xE325, 0x11CE,
         {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 }
        };

    HDEVINFO hdi = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COM_DEVICE, 0, 0,
                                       DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (hdi == INVALID_HANDLE_VALUE) {
        return 0;
    }

    SP_DEVINFO_DATA sDevInfo;
    sDevInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hdi, i, &sDevInfo); ++i){

        enum { BufferSize = 256 };
        char buffer[BufferSize + 1];
        DWORD dwRegType;
        DWORD dwSize;

        // �t�����h���[�l�[�����擾���� COM �ԍ������o��
        SetupDiGetDeviceRegistryPropertyA(hdi, &sDevInfo, SPDRP_FRIENDLYNAME,
                                          &dwRegType, (BYTE*)buffer, BufferSize,
                                          &dwSize);
        enum { ComNameLengthMax = 7 };
        size_t n = strlen(buffer);
        if (n < ComNameLengthMax) {
            // COM �����Z�߂����ꍇ�A�������Ȃ�
            // ��肪����ꍇ�́A�C������
            continue;
        }

        // (COMx) �̍Ō�̊��ʂ�T��
        for (int j = n - 1; j > 0; --j) {
            if (buffer[j] == ')') {
                buffer[j] = '\0';
                break;
            }
        }
        char* p = strstr(&buffer[n - ComNameLengthMax], "COM");
        if (! p) {
            continue;
        }
        found_ports.push_back(p);

        // �f�o�C�X�����擾���AIsUsbCom �̏����Ɉ�v������A�擪�ɔz�u����
        SetupDiGetDeviceRegistryPropertyA(hdi, &sDevInfo, SPDRP_DEVICEDESC,
                                          &dwRegType, (BYTE*)buffer, BufferSize,
                                          &dwSize);
        found_drivers.push_back(buffer);
        pimpl->orderByDriver(found_ports, found_drivers, buffer, all_ports);
    }
    SetupDiDestroyDeviceInfoList(hdi);

    ports.insert(ports.end(), found_ports.begin(), found_ports.end());
    driver_names.insert(driver_names.end(),
                        found_drivers.begin(), found_drivers.end());

    return ports.size();
}


#else

namespace
{
    void searchFiles(vector<string>& ports, const string& dir_name)
    {
        DIR* dp = opendir(dir_name.c_str());
        if (! dp) {
            return;
        }

        struct dirent* dir;
        while ((dir = readdir(dp))) {
            struct stat state;
            stat(dir->d_name, &state);
            if (S_ISDIR(state.st_mode)) {
                string file = dir_name + dir->d_name;
                ports.push_back(file);
            }
        }
    }
}


// Linux, Mac �̏ꍇ
size_t FindComPorts::find(std::vector<std::string>& ports,
                          std::vector<std::string>& driver_names,
                          bool all_ports)
{
    static_cast<void>(all_ports);
    static_cast<void>(driver_names);

    vector<string> found_ports;

    // �o�^�x�[�X�����ɁA�t�@�C�������Ȃ����̒T�����s��
    for (vector<string>::iterator it = pimpl->base_names_.begin();
         it != pimpl->base_names_.end(); ++it) {

        // �f�B���N�g�����ƃt�@�C�����Ƃ̐؂蕪��
        const char* path = it->c_str();
        const char* last_slash = strrchr(path, '/') + 1;

        string dir_name = it->substr(0, last_slash - path);
        vector<string> ports;
        searchFiles(ports, dir_name);

        size_t n = it->size();
        for (vector<string>::iterator fit = ports.begin();
             fit != ports.end(); ++fit) {
            if (! fit->compare(0, n, *it)) {
                // �}�b�`������A�o�^���s��
                pimpl->addFoundPorts(found_ports, *fit);
            }
        }
    }

    for (vector<string>::iterator it = found_ports.begin();
         it != found_ports.end(); ++it) {
        ports.push_back(*it);
        driver_names.push_back("");
    }

    return found_ports.size();
}
#endif


size_t FindComPorts::find(vector<string>& ports, bool all_ports)
{
    vector<string> driver_names_dummy;
    return find(ports, driver_names_dummy, all_ports);
}
