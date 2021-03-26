#pragma once

#include "qtlottiedrawengine.h"

struct Lottie_Animation;

class QTLOTTIE_API QtLottieRLottieEngine : public QtLottieDrawEngine
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieRLottieEngine)

public:
    explicit QtLottieRLottieEngine(QObject *parent = nullptr);
    ~QtLottieRLottieEngine() override;

    void paint(QPainter *painter, const QSize &s) override;
    void render(const QSize &s) override;
    void release() override;

    QString name() const override;

    QUrl source() const override;
    bool setSource(const QUrl &value) override;

    int frameRate() const override;

    int duration() const override;

    QSize size() const override;

    int loops() const override;
    void setLoops(const int value) override;

    bool available() const override;

    bool playing() const override;

private:
    QUrl m_source = {};
    Lottie_Animation *m_animation = nullptr;
    QScopedArrayPointer<char> m_frameBuffer;
    quint64 m_currentFrame = 0;
    quint64 m_totalFrame = 0;
    quint64 m_width = 0;
    quint64 m_height = 0;
    bool m_hasFirstUpdate = false;
    double m_frameRate = 0;
    double m_duration = 0;
    int m_loops = 0;
    int m_loopTimes = 0;
    bool m_shouldStop = false;
};
