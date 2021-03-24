#pragma once

#include "qtlottie_global.h"
#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>

QT_BEGIN_NAMESPACE
QT_FORWARD_DECLARE_CLASS(QPainter)
QT_END_NAMESPACE

using Lottie_Animation = struct Lottie_Animation_S;

class QTLOTTIE_API QtLottieHelper : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieHelper)

public:
    explicit QtLottieHelper(QObject *parent = nullptr);
    ~QtLottieHelper() override;

    bool start(const QString &jsonFilePath, const QString resource);
    void paint(QPainter *painter) const;

private Q_SLOTS:
    void onTimerTicked();

private:
    Lottie_Animation *m_animation = nullptr;
    QTimer m_timer;
    QScopedArrayPointer<char> m_frameBuffer;
    int m_currentFrame = 0;
    int m_totalFrame = 0;
    int m_width = 0;
    int m_height = 0;
    bool m_hasFirstUpdate = false;
    QObject *m_window = nullptr;
};
