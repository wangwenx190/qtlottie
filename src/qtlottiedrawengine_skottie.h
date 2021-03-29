#pragma once

#include "qtlottiedrawengine.h"

using Skottie_Animation = void;
using Skottie_Pixmap = void;

class QTLOTTIE_API QtLottieSkottieEngine : public QtLottieDrawEngine
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieSkottieEngine)

public:
    explicit QtLottieSkottieEngine(QObject *parent = nullptr);
    ~QtLottieSkottieEngine() override;

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

public Q_SLOTS:
    void pause() override;
    void resume() override;

private:
    QUrl m_source = {};
    Skottie_Animation *m_animation = nullptr;
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
