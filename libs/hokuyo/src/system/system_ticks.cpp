/*!
  \file
  \brief �^�C���X�^���v�擾�֐�

  \author Satofumi KAMIMURA

  $Id: system_ticks.cpp 1728 2010-02-26 21:14:05Z satofumi $

  \todo �Đ����x�̕ύX�ɑΏ����邱��
*/

#include "SdlInit.h"
#include "system_ticks.h"
#include <SDL/SDL.h>

namespace
{
  class TicksInit : private qrk::SdlInit
  {
  };
}


long qrk::system_ticks(void)
{
  // ����p
  static TicksInit sdl_init;

  return SDL_GetTicks();
}
