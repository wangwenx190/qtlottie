#include "qtlottiedrawenginefactory.h"
#include "qtlottiedrawengine_rlottie.h"

QtLottieDrawEngine *QtLottieDrawEngineFactory::create(const QString &name)
{
    Q_ASSERT(!name.isEmpty());
    if (name.isEmpty()) {
        return nullptr;
    }
    const QString loweredName = name.toLower();
    if (loweredName == QStringLiteral("rlottie")) {
        return new QtLottieRLottieEngine();
    }
    if (loweredName == QStringLiteral("skottie")) {
        return nullptr;
    }
    return nullptr;
}
