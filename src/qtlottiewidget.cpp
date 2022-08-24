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

#include "qtlottiewidget.h"
#include "qtlottiedrawengine.h"
#include "qtlottiedrawenginefactory.h"
#include <QtGui/qpainter.h>
#include <QtCore/qdebug.h>

QtLottieWidget::QtLottieWidget(QWidget *parent) : QWidget(parent)
{
    m_drawEngine = QtLottieDrawEngineFactory::create();
    if (!available()) {
        qWarning() << "No backends available.";
        dispose();
        return;
    }
    // Use "Qt::PreciseTimer" to get the best appearance, however, it
    // will have a performance impact.
    m_timer.setTimerType(Qt::CoarseTimer);
    connect(&m_timer, &QTimer::timeout, this, [this](){
        if (m_drawEngine->playing()) {
            m_drawEngine->render(size());
        }
    });
    connect(m_drawEngine, &QtLottieDrawEngine::needsRepaint, this, qOverload<>(&QtLottieWidget::update));
    connect(m_drawEngine, &QtLottieDrawEngine::frameRateChanged, this, [this](qint64 arg){
        if (m_timer.isActive()) {
            m_timer.stop();
        }
        m_timer.setInterval(qRound(qreal(1000) / qreal(arg)));
        if (m_drawEngine->playing()) {
            m_timer.start();
        }
        Q_EMIT frameRateChanged(arg);
    });
    connect(m_drawEngine, &QtLottieDrawEngine::playingChanged, this, [this](bool arg){
        if (arg) {
            if (!m_timer.isActive()) {
                if (m_timer.interval() <= 0) {
                    m_timer.setInterval(qRound(qreal(1000) / qreal(m_drawEngine->frameRate())));
                }
                m_timer.start();
            }
        } else {
            if (m_timer.isActive()) {
                m_timer.stop();
            }
        }
    });
    connect(m_drawEngine, &QtLottieDrawEngine::durationChanged, this, &QtLottieWidget::durationChanged);
    connect(m_drawEngine, &QtLottieDrawEngine::sizeChanged, this, &QtLottieWidget::sourceSizeChanged);
    connect(m_drawEngine, &QtLottieDrawEngine::loopsChanged, this, &QtLottieWidget::loopsChanged);
    connect(m_drawEngine, &QtLottieDrawEngine::devicePixelRatioChanged, this, &QtLottieWidget::devicePixelRatioChanged);
}

QtLottieWidget::~QtLottieWidget()
{
    dispose();
}

void QtLottieWidget::dispose()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    if (m_drawEngine) {
        m_drawEngine->release();
        m_drawEngine = nullptr;
    }
}

void QtLottieWidget::pause()
{
    if (available()) {
        m_drawEngine->pause();
    }
}

void QtLottieWidget::resume()
{
    if (available()) {
        m_drawEngine->resume();
    }
}

QSize QtLottieWidget::minimumSizeHint() const
{
    // Our lottie backend will fail to paint if the size of the widget is too small.
    // This size will be ignored if you set the size policy or minimum size explicitly.
    return {50, 50};
}

QString QtLottieWidget::backend() const
{
    return available() ? m_drawEngine->name() : QStringLiteral("Unknown");
}

QUrl QtLottieWidget::source() const
{
    return m_source;
}

void QtLottieWidget::setSource(const QUrl &value)
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
        Q_EMIT sourceChanged(m_source);
    }
}

qint64 QtLottieWidget::frameRate() const
{
    // TODO: is the fallback value appropriate?
    return available() ? m_drawEngine->frameRate() : 30;
}

qint64 QtLottieWidget::duration() const
{
    // TODO: is the fallback value appropriate?
    return available() ? m_drawEngine->duration() : 0;
}

QSize QtLottieWidget::sourceSize() const
{
    // TODO: is the fallback value appropriate?
    return available() ? m_drawEngine->size() : QSize{50, 50};
}

qint64 QtLottieWidget::loops() const
{
    return available() ? m_drawEngine->loops() : 0;
}

void QtLottieWidget::setLoops(const qint64 value)
{
    if (available()) {
        m_drawEngine->setLoops(value);
    }
}

bool QtLottieWidget::available() const
{
    return (m_drawEngine && m_drawEngine->available());
}

qreal QtLottieWidget::devicePixelRatio() const
{
    return available() ? m_drawEngine->devicePixelRatio() : qreal(1);
}

void QtLottieWidget::setDevicePixelRatio(const qreal value)
{
    if (!available()) {
        return;
    }
    m_drawEngine->setDevicePixelRatio(value);
}

void QtLottieWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    if (available()) {
        m_drawEngine->paint(&painter, size());
    }
}
