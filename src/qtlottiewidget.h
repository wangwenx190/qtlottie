#pragma once

#include "qtlottie_global.h"
#include <QtWidgets/qwidget.h>
#include <QtCore/qurl.h>
#include <QtCore/qtimer.h>

class QtLottieDrawEngine;

class QTLOTTIE_API QtLottieWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieWidget)
    Q_PROPERTY(QString backend READ backend CONSTANT)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int frameRate READ frameRate NOTIFY frameRateChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QSize sourceSize READ sourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(int loops READ loops WRITE setLoops NOTIFY loopsChanged)

public:
    explicit QtLottieWidget(QWidget *parent = nullptr);
    ~QtLottieWidget() override;

    QSize minimumSizeHint() const override;

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

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QUrl m_source = {};
    QtLottieDrawEngine *m_drawEngine = nullptr;
    QTimer m_timer;
};
