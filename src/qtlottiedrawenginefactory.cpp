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

QtLottieDrawEngine *QtLottieDrawEngineFactory::create(const char *name)
{
    Q_ASSERT(name);
    if (!name) {
        return nullptr;
    }
    if (qstricmp(name, "skottie") == 0) {
        qDebug() << "Trying the skottie backend ...";
        return new QtLottieSkottieEngine();
    } else if (qstricmp(name, "rlottie") == 0) {
        qDebug() << "Trying the rlottie backend ...";
        return new QtLottieRLottieEngine();
    } else {
        qWarning() << "Unsupported lottie backend:" << name;
    }
    return nullptr;
}
