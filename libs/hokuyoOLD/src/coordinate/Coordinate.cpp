/*!
  \file
  \brief ���W�n

  \author Satofumi KAMIMURA

  $Id: Coordinate.cpp 1977 2012-04-05 08:04:51Z satofumi $
*/

#include "Coordinate.h"
#include "../system/MathUtils.h"
#include <map>
#include <cstdio>

using namespace qrk;
using namespace std;


namespace
{
  class Child
  {
  public:
    Coordinate* pointer_;
    Position<long> offset_;


    Child(void) : pointer_(NULL)
    {
    }


    Child(Coordinate* pointer, const Position<long>& offset)
      : pointer_(pointer), offset_(offset)
    {
    }
  };
  typedef map<Coordinate*, Child> Children;


  // Rotate.h �̊֐����g���悤�ɂ���
  Position<long> rotate(const Position<long>& point, const double radian)
  {
    long x = static_cast<long>
      (lrint((point.x * cos(radian)) - (point.y * sin(radian))));
    long y = static_cast<long>
      (lrint((point.x * sin(radian)) + (point.y * cos(radian))));

    return Position<long>(x, y, point.angle);
  }
}


struct Coordinate::pImpl
{
  Coordinate* parent_;
  Children children_;


  pImpl(Coordinate* parent)
    : parent_(parent)
  {
  }


  void eraseFromParent(Coordinate* coordinate)
  {
    parent_->pimpl->children_.erase(coordinate);
  }


  Position<long> positionOnChild(const Coordinate* child,
                                 const Position<long>& position)
  {
    Coordinate* parent = child->parent();
    Position<long> offset = parent->offset(child);

    // �q�̍��W�n�ł̈ʒu x, y ���A�q�� offset �̊p�x t ��ɉ�]������
    // �v�Z��� x, y �� t �� �q�� offset �� x, y, t �����ꂼ����Z����
    Position<long> converted_position = rotate(position, offset.to_rad());
    converted_position += offset;

    return converted_position;
  }


  Position<long> positionOnParent(const Coordinate* child,
                                  const Position<long>& position)
  {
    Coordinate* parent = child->parent();
    Position<long> offset = parent->offset(child);

    // �e�̍��W�n�ł̈ʒu x, y ���� offset �� x, y, t ����
    // �v�Z��� x, y �� offset �̊p�x -t ��ɉ�]������
    Position<long> converted_position = position - offset;
    return rotate(converted_position, -offset.to_rad());
  }


  Position<long> positionOnRoot(const Coordinate* coordinate,
                                  const Position<long>& position)
  {
    if (coordinate == root()) {
      return position;
    }

    return positionOnParent(coordinate,
                            positionOnRoot(coordinate->parent(), position));
  }
};


// root() ��p�̃R���X�g���N�^
Coordinate::Coordinate(Coordinate* parent) : pimpl(new pImpl(parent))
{
}


Coordinate::Coordinate(void) : pimpl(new pImpl(root()))
{
  // root �̎q�̍��W�n�Ƃ��ēo�^����
  setOriginTo(root(), Position<long>(0, 0, deg(0)));
}


Coordinate::~Coordinate(void)
{
  if (! pimpl->parent_) {
    return;
  }

  // �q�̍��W�n��e���W�n�Ɋ���U��
  // !!!

  // �e����o�^���폜
  pimpl->eraseFromParent(this);
}


Coordinate* Coordinate::root(void)
{
  static Coordinate root_coordinate(NULL);
  return &root_coordinate;
}


void Coordinate::setOriginTo(Coordinate* parent,
                             const Position<long>& position)
{
  pimpl->eraseFromParent(this);

  parent->pimpl->children_[this] = Child(this, position);
  pimpl->parent_ = parent;
}


Coordinate* Coordinate::parent(void) const
{
  return pimpl->parent_;
}


set<Coordinate*> Coordinate::children(void) const
{
  set<Coordinate*> children;
  for (Children::iterator it = pimpl->children_.begin();
       it != pimpl->children_.end(); ++it) {
    children.insert(it->first);
  }
  return children;
}


Position<long> Coordinate::offset(const Coordinate* child) const
{
  Children::iterator it = pimpl->children_.find(const_cast<Coordinate*>(child));
  if (it != pimpl->children_.end()) {
    return it->second.offset_;
  }

  // ��O�𓊂��邱�Ƃ���������
  // !!!
  fprintf(stderr, "Coordinte::offset(): no child.\n");

  Position<long> dummy;
  return dummy;
}


Position<long> Coordinate::pointPosition(const Coordinate* coordinate,
                                         const Position<long>& position) const
{
  // ���W�n�ɓ��L�ȏ��������s
  const_cast<Coordinate*>(this)->beforeEvaluate();
  if (coordinate) {
    const_cast<Coordinate*>(coordinate)->beforeEvaluate();
  }

  // coordinate �̃O���[�o�����W�n�ł̈ʒu�����߂�
  Position<long> root_position = position;
  Coordinate* root_coordinate = root();
  for (const Coordinate* p = coordinate;
       p != root_coordinate; p = p->parent()) {
    root_position = pimpl->positionOnChild(p, root_position);
    //fprintf(stderr, "{%ld,%ld,%d}, ", root_position.x, root_position.y, root_position.deg());
  }

  // ���̍��W�n�ł̈ʒu�����߂ĕԂ�
  return pimpl->positionOnRoot(this, root_position);
}
