/*
 * MIT License
 *
 * Copyright (C) 2022 by wangwenx190 (Yuhang Zhao)
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

#pragma once

#include "qtlottie_global.h"
#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qsize.h>

QT_BEGIN_NAMESPACE
class QPainter;
QT_END_NAMESPACE

class QTLOTTIE_API QtLottieDrawEngine : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieDrawEngine)
    Q_PROPERTY(QString name READ name CONSTANT FINAL)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged FINAL)
    Q_PROPERTY(qint64 frameRate READ frameRate NOTIFY frameRateChanged FINAL)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged FINAL)
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged FINAL)
    Q_PROPERTY(qint64 loops READ loops WRITE setLoops NOTIFY loopsChanged FINAL)
    Q_PROPERTY(bool available READ available CONSTANT FINAL)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged FINAL)
    Q_PROPERTY(qreal devicePixelRatio READ devicePixelRatio WRITE setDevicePixelRatio NOTIFY devicePixelRatioChanged FINAL)

public:
    explicit QtLottieDrawEngine(QObject *parent = nullptr) : QObject(parent) {}
    ~QtLottieDrawEngine() override = default;

    virtual void paint(QPainter *painter, const QSize &s) = 0;
    virtual void render(const QSize &s) = 0;
    virtual void release() = 0;

    virtual QString name() const = 0;

    virtual QUrl source() const = 0;
    virtual bool setSource(const QUrl &value) = 0;

    virtual qint64 frameRate() const = 0;

    virtual qint64 duration() const = 0;

    virtual QSize size() const = 0;

    virtual qint64 loops() const = 0;
    virtual void setLoops(const qint64 value) = 0;

    virtual bool available() const = 0;

    virtual bool playing() const = 0;

    virtual qreal devicePixelRatio() const = 0;
    virtual void setDevicePixelRatio(const qreal value) = 0;

public Q_SLOTS:
    virtual void pause() = 0;
    virtual void resume() = 0;

Q_SIGNALS:
    void sourceChanged(const QUrl &);
    void frameRateChanged(qint64);
    void durationChanged(qint64);
    void sizeChanged(const QSize &);
    void loopsChanged(qint64);
    void playingChanged(bool);
    void devicePixelRatioChanged(qreal);

    void needsRepaint();
};
