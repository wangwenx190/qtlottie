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

#pragma once

#include "qtlottie_global.h"
#include <QtQuick/qquickpainteditem.h>
#include <QtCore/qtimer.h>

class QtLottieDrawEngine;

class QTLOTTIE_API QtLottieItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieItem)
#ifdef QML_NAMED_ELEMENT
    QML_NAMED_ELEMENT(LottieItem)
#endif
    Q_PROPERTY(QString backend READ backend CONSTANT FINAL)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged FINAL)
    Q_PROPERTY(qint64 frameRate READ frameRate NOTIFY frameRateChanged FINAL)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged FINAL)
    Q_PROPERTY(QSizeF sourceSize READ sourceSize NOTIFY sourceSizeChanged FINAL)
    Q_PROPERTY(qint64 loops READ loops WRITE setLoops NOTIFY loopsChanged FINAL)
    Q_PROPERTY(bool available READ available CONSTANT FINAL)
    Q_PROPERTY(qreal devicePixelRatio READ devicePixelRatio WRITE setDevicePixelRatio NOTIFY devicePixelRatioChanged FINAL)

public:
    explicit QtLottieItem(QQuickItem *parent = nullptr);
    ~QtLottieItem() override;

    void paint(QPainter *painter) override;

    QString backend() const;

    QUrl source() const;
    void setSource(const QUrl &value);

    qint64 frameRate() const;

    qint64 duration() const;

    QSizeF sourceSize() const;

    qint64 loops() const;
    void setLoops(const qint64 value);

    bool available() const;

    qreal devicePixelRatio() const;
    void setDevicePixelRatio(const qreal value);

public Q_SLOTS:
    void dispose();
    void pause();
    void resume();

Q_SIGNALS:
    void sourceChanged(const QUrl &);
    void frameRateChanged(qint64);
    void durationChanged(qint64);
    void sourceSizeChanged(const QSizeF &);
    void loopsChanged(qint64);
    void devicePixelRatioChanged(qreal);

private:
    QUrl m_source = {};
    QtLottieDrawEngine *m_drawEngine = nullptr;
    QTimer m_timer;
};
