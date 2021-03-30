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
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

public:
    explicit QtLottieWidget(QWidget *parent = nullptr);
    ~QtLottieWidget() override;

    void dispose();

    QSize minimumSizeHint() const override;

    QUrl source() const;
    void setSource(const QUrl &value);

Q_SIGNALS:
    void sourceChanged();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QUrl m_source = {};
    QtLottieDrawEngine *m_drawEngine = nullptr;
    QTimer m_timer;
};
