/*!
  \file
  \brief SDL �V�X�e���̏���

  \author Satofumi KAMIMURA

  $Id: SdlInit.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include <cstdlib>
#include "SdlInit.h"
#include <SDL/SDL.h>

using namespace qrk;


struct SdlInit::pImpl
{
  static bool initialized_;
};
bool SdlInit::pImpl::initialized_ = false;


SdlInit::SdlInit(void) : pimpl(new pImpl)
{
  if (pimpl->initialized_) {
    // ����ς݂Ȃ�΁A�߂�
    return;
  }

  if (SDL_Init(0) < 0) {
    // !!! ��O�𓊂���ׂ�
    return;
  }
  atexit(SDL_Quit);
  pimpl->initialized_ = true;
}


SdlInit::~SdlInit(void)
{
}
