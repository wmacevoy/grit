#ifndef QRK_MONITOR_DATA_HANDLER_H
#define QRK_MONITOR_DATA_HANDLER_H

/*!
  \file
  \brief ���j�^�f�[�^�Ǘ�

  \author Satofumi KAMIMURA

  $Id: MonitorDataHandler.h 783 2009-05-05 08:56:26Z satofumi $
*/

#include "MonitorModeManager.h"
#include "../system/DetectOS.h"
#include <string>

#ifdef MSC
#define snprintf _snprintf
#endif


namespace qrk
{

  /*!
    \brief ���j�^�f�[�^�Ǘ��N���X

    �f�[�^�t�H�[�}�b�g�́A�Q�s����\�������
    - �P�s��: �^�C���X�^���v, �f�[�^�o�C�g��
    - �Q�s��: �f�[�^

    �P�s�ڂɂ����āA# �ȍ~�̓R�����g�Ƃ݂Ȃ����
  */
  class MonitorDataHandler
  {
    MonitorDataHandler(void);
    MonitorDataHandler(const MonitorDataHandler& rhs);
    MonitorDataHandler& operator = (const MonitorDataHandler& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;

  public:
    explicit MonitorDataHandler(std::string file,
                                MonitorModeManager::MonitorMode mode);
    ~MonitorDataHandler(void);

    int fetch(char* data, int size, const char* comment = NULL);

    /*!
      \attention ��s���������镶����f�[�^�́A�����Ȃ�
    */
    int fetch(std::string& data, const char* comment = NULL);
    int fetch(bool& data, const char* comment = NULL);
    int fetch(int& data, const char* comment = NULL);
    int fetch(unsigned int& data, const char* comment = NULL);
    int fetch(short& data, const char* comment = NULL);
    int fetch(long& data, const char* comment = NULL);
  };
}

#endif /* !QRK_MONITOR_DATA_HANDLER_H */
