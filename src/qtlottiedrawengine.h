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
    Q_PROPERTY(qreal frameRate READ frameRate NOTIFY frameRateChanged FINAL)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged FINAL)
    Q_PROPERTY(QSizeF size READ size NOTIFY sizeChanged FINAL)
    Q_PROPERTY(int loops READ loops WRITE setLoops NOTIFY loopsChanged FINAL)
    Q_PROPERTY(bool available READ available CONSTANT FINAL)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged FINAL)

public:
    explicit QtLottieDrawEngine(QObject *parent = nullptr) : QObject(parent) {}
    ~QtLottieDrawEngine() override = default;

    virtual void paint(QPainter *painter, const QSizeF &s) = 0;
    virtual void render(const QSizeF &s) = 0;
    virtual void release() = 0;

    virtual QString name() const = 0;

    virtual QUrl source() const = 0;
    virtual bool setSource(const QUrl &value) = 0;

    virtual qreal frameRate() const = 0;

    virtual qreal duration() const = 0;

    virtual QSizeF size() const = 0;

    virtual int loops() const = 0;
    virtual void setLoops(const int value) = 0;

    virtual bool available() const = 0;

    virtual bool playing() const = 0;

public Q_SLOTS:
    virtual void pause() = 0;
    virtual void resume() = 0;

Q_SIGNALS:
    void sourceChanged(const QUrl &);
    void frameRateChanged(qreal);
    void durationChanged(qreal);
    void sizeChanged(const QSizeF &);
    void loopsChanged(int);
    void playingChanged(bool);

    void needsRepaint();
};
