
//includes for all 3rd party libraries used

#ifndef MEGA_LIBS_H
#define MEGA_LIBS_H

#include <Wire.h>

#ifdef USE_CUSTOM_RGB_MATRIX_LIBRARY
#include "src/customRGBMatrixPanel/customRGBmatrixPanel.h"
#else
#include "RGBmatrixPanel.h"
#endif

#include "src/Timer3/TimerThree.h"

#include "Coms.h"
#include "Coms_Serial.h"
#include "Graphics.h"
#include "Menus.h"
#include "Host.h"

#endif	// MEGA_LIBS_H
