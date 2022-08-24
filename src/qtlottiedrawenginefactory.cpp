/*
 * MIT License
 *
 * Copyright (C) 2021 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "qtlottiedrawenginefactory.h"
#include "qtlottiedrawengine_skottie.h"
#include "qtlottiedrawengine_rlottie.h"
#include <QtCore/qdebug.h>

static constexpr const char kPreferredEngine[] = "QTLOTTIE_PREFERRED_ENGINE";

QtLottieDrawEngine *QtLottieDrawEngineFactory::create(const Backend backend)
{
    switch (backend) {
    case Backend::Skottie:
        return new QtLottieSkottieEngine();
    case Backend::RLottie:
        return new QtLottieRLottieEngine();
    }
    return nullptr;
}

QtLottieDrawEngine *QtLottieDrawEngineFactory::create(const char *name)
{
    Q_ASSERT(name);
    if (!name) {
        return nullptr;
    }
    if (qstricmp(name, "skottie") == 0) {
        return create(Backend::Skottie);
    }
    if (qstricmp(name, "rlottie") == 0) {
        return create(Backend::RLottie);
    }
    qWarning() << "Unsupported lottie backend:" << name;
    return nullptr;
}

QtLottieDrawEngine *QtLottieDrawEngineFactory::create()
{
    const QString preferred = qEnvironmentVariable(kPreferredEngine, QStringLiteral("skottie"));
    if (const auto engine = create(qUtf8Printable(preferred))) {
        if (engine->available()) {
            return engine;
        }
        engine->release();
        qWarning() << preferred << "is not available.";
    }
    if (const auto engine = create("rlottie")) {
        if (engine->available()) {
            return engine;
        }
        engine->release();
        qWarning() << "rlottie is not available.";
    }
    // ### TODO: Use Qt Lottie (provided by TQtC) as the final fallback.
    return nullptr;
}
