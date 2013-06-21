#ifndef QRK_SOCKET_SET_H
#define QRK_SOCKET_SET_H

/*!
  \file
  \brief �ʐM�\�P�b�g�Ǘ�

  \author Satofumi KAMIMURA

  $Id: SocketSet.h 1836 2010-05-26 11:23:08Z satofumi $
*/

#include <SDL/SDL_net.h>
#include <memory>

namespace qrk
{
    class TcpipCtrl;


    //! �ʐM�\�P�b�g�Ǘ�
    class SocketSet
    {
    public:
        /*!
          \brief �R���X�g���N�^

          \param size [i] �����Ԃł̃\�P�b�g���e��
        */
        explicit SocketSet(size_t size = 1);
        ~SocketSet(void);


        /*!
          \brief �\�P�b�g�̓o�^

          \param socket [i] �o�^����\�P�b�g

          \retval true �o�^�ɐ���
          \retval false �o�^�Ɏ��s
        */
        bool add(TCPsocket socket);


        /*!
          \brief �\�P�b�g�̍폜

          \param socket [i] �폜����\�P�b�g
        */
        void del(TCPsocket socket);


        /*!
          \brief ��M�f�[�^�����邩�̊m�F

          \param timeout [i] �^�C���A�E�g���� [msec]

          \return ��M�f�[�^�̂���\�P�b�g��
        */
        size_t check(int timeout);

    private:
        SocketSet(const SocketSet& rhs);
        SocketSet& operator = (const SocketSet& rhs);

        struct pImpl;
        const std::auto_ptr<pImpl> pimpl;
    };
}

#endif /* !QRK_SOCKET_SET_H */
