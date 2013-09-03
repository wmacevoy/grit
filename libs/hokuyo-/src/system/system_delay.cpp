/*!
  \file
  \brief �ҋ@�֐�

  \author Satofumi KAMIMURA

  $Id: system_delay.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include "system_delay.h"
#include "SdlInit.h"
#include <SDL/SDL.h>


namespace
{
  class DelayInit : private qrk::SdlInit
  {
  };
}


void qrk::system_delay(int msec)
{
  // ����p
  static DelayInit sdl_init;

  SDL_Delay(msec);
}
