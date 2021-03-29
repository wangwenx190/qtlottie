#pragma once

#include "qtlottie_global.h"

class QtLottieDrawEngine;

namespace QtLottieDrawEngineFactory
{

QTLOTTIE_API QtLottieDrawEngine *create(const char *name);

}
