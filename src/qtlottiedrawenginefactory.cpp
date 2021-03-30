#include "qtlottiedrawenginefactory.h"
#include "qtlottiedrawengine_skottie.h"
#include "qtlottiedrawengine_rlottie.h"
#include <QtCore/qdebug.h>

QtLottieDrawEngine *QtLottieDrawEngineFactory::create(const char *name)
{
    Q_ASSERT(name);
    if (!name) {
        return nullptr;
    }
    if (_stricmp(name, "skottie") == 0) {
        qDebug() << "Trying the skottie backend ...";
        return new QtLottieSkottieEngine();
    } else if (_stricmp(name, "rlottie") == 0) {
        qDebug() << "Trying the rlottie backend ...";
        return new QtLottieRLottieEngine();
    } else {
        qWarning() << "Unsupported lottie backend:" << name;
    }
    return nullptr;
}
