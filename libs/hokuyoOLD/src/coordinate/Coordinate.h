#ifndef QRC_COORDINATE_H
#define QRC_COORDINATE_H

/*!
  \file
  \brief ���W�n

  \author Satofumi KAMIMURA

  $Id: Coordinate.h 1601 2010-01-04 15:09:50Z satofumi $
*/

#include "../geometry/Position.h"
#include <set>
#include <memory>


namespace qrk
{
  /*!
    \brief ���W�n
  */
  class Coordinate
  {
  public:
    Coordinate(void);
    virtual ~Coordinate(void);


    /*!
      \brief �O���[�o�����W�n�̎擾
    */
    static Coordinate* root(void);


    /*!
      \brief ���W�n�̌��_��ݒ�

      �w�肵�����W�n�Ɍ��_�ʒu������t����B

      \param[in,out] parent �e�Ƃ��Đݒ肷����W�n
      \param[in] position ���_�̐e���W�n�ł̈ʒu
    */
    void setOriginTo(Coordinate* parent, const Position<long>& position);


    /*!
      \brief �e���W�n�̎擾

      \retval �e���W�n
    */
    Coordinate* parent(void) const;


    /*!
      \brief �q���W�n�̎擾

      \retval �q���W�n�̃|�C���^�ꗗ
    */
    std::set<Coordinate*> children(void) const;


    /*!
      \brief �q���W�n�̌��_�ʒu���擾

      \param[in] child �q���W�n
      \retval �q���W�n�̌��_�ʒu

      \todo �]�͂���΁A�C�ӂ̍��W�n��^���ď����ł���悤�ɕύX����
    */
    Position<long> offset(const Coordinate* child) const;


    /*!
      \brief �w�肵���ʒu�����W�n�̂ǂ��ɑ������邩���擾

      \param[in] position �ʒu
      \param[in] coordinate �ʒu�����݂�����W�n
      \retval ���g�̍��W�n�ł̈ʒu
    */
    Position<long> pointPosition(const Coordinate* coordinate,
                                 const Position<long>& position) const;

  protected:
    /*!
      \brief ���W�n�̕]���O�ɌĂ΂�郁�\�b�h
    */
    virtual void beforeEvaluate(void)
    {
    }

  private:
    Coordinate(Coordinate* parent);
    Coordinate(const Coordinate& rhs);
    Coordinate& operator = (const Coordinate& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;
  };
}

#endif /*! QRC_COORDINATE_H */
