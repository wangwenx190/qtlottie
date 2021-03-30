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

#include "qtlottieitem.h"
#include "qtlottiedrawengine.h"
#include "qtlottiedrawenginefactory.h"
#include <QtCore/qdebug.h>

QtLottieItem::QtLottieItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    // We prefer Skottie over RLottie.
    m_drawEngine = QtLottieDrawEngineFactory::create("skottie");
    if (!available()) {
        qWarning() << "Failed to initialize the skottie backend.";
        dispose();
        m_drawEngine = QtLottieDrawEngineFactory::create("rlottie");
        if (!available()) {
            qWarning() << "Failed to initialize the rlottie backend.";
            dispose();
            return;
        }
    }
    m_timer.setTimerType(Qt::PreciseTimer); // Is this necesary?
    connect(&m_timer, &QTimer::timeout, this, [this](){
        if (m_drawEngine->playing()) {
            m_drawEngine->render({qRound(width()), qRound(height())});
        }
    });
    connect(m_drawEngine, &QtLottieDrawEngine::needsRepaint, this, [this](){
        update();
    });
    connect(m_drawEngine, &QtLottieDrawEngine::frameRateChanged, this, [this](){
        if (m_timer.isActive()) {
            m_timer.stop();
        }
        m_timer.setInterval(1000 / m_drawEngine->frameRate());
        if (m_drawEngine->playing()) {
            m_timer.start();
        }
        Q_EMIT frameRateChanged();
    });
    connect(m_drawEngine, &QtLottieDrawEngine::playingChanged, this, [this](){
        if (m_drawEngine->playing()) {
            if (!m_timer.isActive()) {
                if (m_timer.interval() <= 0) {
                    m_timer.setInterval(1000 / m_drawEngine->frameRate());
                }
                m_timer.start();
            }
        } else {
            if (m_timer.isActive()) {
                m_timer.stop();
            }
        }
    });
    connect(m_drawEngine, &QtLottieDrawEngine::durationChanged, this, &QtLottieItem::durationChanged);
    connect(m_drawEngine, &QtLottieDrawEngine::sizeChanged, this, &QtLottieItem::sourceSizeChanged);
    connect(m_drawEngine, &QtLottieDrawEngine::loopsChanged, this, &QtLottieItem::loopsChanged);
}

QtLottieItem::~QtLottieItem()
{
    dispose();
}

void QtLottieItem::paint(QPainter *painter)
{
    Q_ASSERT(painter);
    if (!painter) {
        return;
    }
    if (available()) {
        m_drawEngine->paint(painter, {qRound(width()), qRound(height())});
    }
}

void QtLottieItem::dispose()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    if (m_drawEngine) {
        m_drawEngine->release();
        m_drawEngine = nullptr;
    }
}

void QtLottieItem::pause()
{
    if (available()) {
        m_drawEngine->pause();
    }
}

void QtLottieItem::resume()
{
    if (available()) {
        m_drawEngine->resume();
    }
}

QString QtLottieItem::backend() const
{
    return available() ? m_drawEngine->name() : QStringLiteral("Unknown");
}

QUrl QtLottieItem::source() const
{
    return m_source;
}

void QtLottieItem::setSource(const QUrl &value)
{
    if (!value.isValid()) {
        qWarning() << value << "is not a valid URL.";
        return;
    }
    if (m_source != value) {
        m_source = value;
        if (available()) {
            if (!m_drawEngine->setSource(value)) {
                qWarning() << "Failed to start playing.";
            }
        }
        Q_EMIT sourceChanged();
    }
}

int QtLottieItem::frameRate() const
{
    // TODO: is the fallback value appropriate?
    return available() ? m_drawEngine->frameRate() : 30;
}

int QtLottieItem::duration() const
{
    // TODO: is the fallback value appropriate?
    return available() ? m_drawEngine->duration() : 0;
}

QSize QtLottieItem::sourceSize() const
{
    // TODO: is the fallback value appropriate?
    return available() ? m_drawEngine->size() : QSize{50, 50};
}

int QtLottieItem::loops() const
{
    return available() ? m_drawEngine->loops() : 0;
}

void QtLottieItem::setLoops(const int value)
{
    if (available()) {
        m_drawEngine->setLoops(value);
    }
}

bool QtLottieItem::available() const
{
    return (m_drawEngine && m_drawEngine->available());
}
