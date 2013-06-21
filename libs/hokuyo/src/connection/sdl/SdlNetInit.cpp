/*!
  \file
  \brief SDL_net �̏���

  \author Satofumi KAMIMURA

  $Id$
*/

#include "SdlNetInit.h"
#include "../../system/SdlInit.h"
#include <SDL/SDL_net.h>

using namespace qrk;


struct SdlNetInit::pImpl : private SdlInit
{
    static bool initialized;
};
bool SdlNetInit::pImpl::initialized = false;


SdlNetInit::SdlNetInit(void) : pimpl(new pImpl)
{
    if (pimpl->initialized) {
        return;
    }

    if (SDLNet_Init() < 0) {
        return;
    }
    atexit(SDLNet_Quit);
    pimpl->initialized = true;
}


SdlNetInit::~SdlNetInit(void)
{
}
