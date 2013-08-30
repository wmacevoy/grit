/*!
  \file
  \brief シリアル用の補助関数

  \author Satofumi KAMIMURA

  $Id$
*/

#include "../include/c/urg_serial_utils.h"
#include "../include/c/urg_detect_os.h"


#if defined(URG_WINDOWS_OS)
#include "urg_serial_utils_windows.c"
#pragma comment(lib, "Setupapi.lib")
#else
#include "urg_serial_utils_linux.c"
#endif
