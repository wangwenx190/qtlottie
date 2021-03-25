#pragma once

#include "qtlottie_global.h"
#include <QtWidgets/qwidget.h>

class QtLottieHelper;

class QTLOTTIE_API QtLottieWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieWidget)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

public:
    explicit QtLottieWidget(QWidget *parent = nullptr);
    ~QtLottieWidget() override;

    QString source() const;
    void setSource(const QString &value);

Q_SIGNALS:
    void sourceChanged();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QScopedPointer<QtLottieHelper> m_lottieHelper;
    QString m_source = {};
};
