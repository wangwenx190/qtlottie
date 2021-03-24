#pragma once

#include "qtlottie_global.h"
#include <QtWidgets/qwidget.h>

class QtLottieHelper;

class QTLOTTIE_API QtLottieWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieWidget)

public:
    explicit QtLottieWidget(QWidget *parent = nullptr);
    ~QtLottieWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QScopedPointer<QtLottieHelper> m_lottieHelper;
};
