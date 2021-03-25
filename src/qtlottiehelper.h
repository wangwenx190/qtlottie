#pragma once

#include "qtlottie_global.h"
#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>

QT_BEGIN_NAMESPACE
QT_FORWARD_DECLARE_CLASS(QPainter)
QT_END_NAMESPACE

struct Lottie_Animation;

class QTLOTTIE_API QtLottieHelper : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieHelper)

public:
    explicit QtLottieHelper(QObject *parent = nullptr);
    ~QtLottieHelper() override;

    bool start(const QString &jsonFilePath, const QString &resourceFolderPath);
    void paint(QPainter *painter) const;
    bool reloadRLottie(const QString &fileName = {}) const;

private Q_SLOTS:
    void onTimerTicked();

private:
    bool checkParent() const;

private:
    Lottie_Animation *m_animation = nullptr;
    QTimer m_timer;
    QScopedArrayPointer<char> m_frameBuffer;
    quint64 m_currentFrame = 0;
    quint64 m_totalFrame = 0;
    quint64 m_width = 0;
    quint64 m_height = 0;
    bool m_hasFirstUpdate = false;
};
