#pragma once

#include "qtlottie_global.h"
#include <QtQuick/qquickpainteditem.h>
#include <QtCore/qtimer.h>

class QtLottieDrawEngine;

class QTLOTTIE_API QtLottieItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieItem)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    QML_ELEMENT
#endif
    Q_PROPERTY(QString backend READ backend CONSTANT)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int frameRate READ frameRate NOTIFY frameRateChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QSize sourceSize READ sourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(int loops READ loops WRITE setLoops NOTIFY loopsChanged)

public:
    explicit QtLottieItem(QQuickItem *parent = nullptr);
    ~QtLottieItem() override;

    void paint(QPainter *painter) override;

    QString backend() const;

    QUrl source() const;
    void setSource(const QUrl &value);

    int frameRate() const;

    int duration() const;

    QSize sourceSize() const;

    int loops() const;
    void setLoops(const int value);

public Q_SLOTS:
    void dispose();
    void pause();
    void resume();

Q_SIGNALS:
    void sourceChanged();
    void frameRateChanged();
    void durationChanged();
    void sourceSizeChanged();
    void loopsChanged();

private:
    QUrl m_source = {};
    QtLottieDrawEngine *m_drawEngine = nullptr;
    QTimer m_timer;
};
