#include "qtlottiewidget.h"
#include "qtlottiedrawengine.h"
#include "qtlottiedrawenginefactory.h"
#include <QtGui/qpainter.h>
#include <QtCore/qdebug.h>

QtLottieWidget::QtLottieWidget(QWidget *parent) : QWidget(parent)
{
    // We prefer Skottie over RLottie.
    m_drawEngine = QtLottieDrawEngineFactory::create("skottie");
    if (!available()) {
        qWarning() << "Failed to initialize the skottie backend.";
        dispose();
        m_drawEngine = QtLottieDrawEngineFactory::create("rlottie");
        if (!available()) {
            qWarning() << "Failed to initialize the rlottie backend.";
            dispose();
            return;
        }
    }
    m_timer.setTimerType(Qt::PreciseTimer); // Is this necessary?
    connect(&m_timer, &QTimer::timeout, this, [this](){
        if (m_drawEngine->playing()) {
            m_drawEngine->render(size());
        }
    });
    connect(m_drawEngine, &QtLottieDrawEngine::needsRepaint, this, qOverload<>(&QtLottieWidget::update));
    connect(m_drawEngine, &QtLottieDrawEngine::frameRateChanged, this, [this](){
        if (m_timer.isActive()) {
            m_timer.stop();
        }
        m_timer.setInterval(1000 / m_drawEngine->frameRate());
        if (m_drawEngine->playing()) {
            m_timer.start();
        }
        Q_EMIT frameRateChanged();
    });
    connect(m_drawEngine, &QtLottieDrawEngine::playingChanged, this, [this](){
        if (m_drawEngine->playing()) {
            if (!m_timer.isActive()) {
                if (m_timer.interval() <= 0) {
                    m_timer.setInterval(1000 / m_drawEngine->frameRate());
                }
                m_timer.start();
            }
        } else {
            if (m_timer.isActive()) {
                m_timer.stop();
            }
        }
    });
    connect(m_drawEngine, &QtLottieDrawEngine::durationChanged, this, &QtLottieWidget::durationChanged);
    connect(m_drawEngine, &QtLottieDrawEngine::sizeChanged, this, &QtLottieWidget::sourceSizeChanged);
    connect(m_drawEngine, &QtLottieDrawEngine::loopsChanged, this, &QtLottieWidget::loopsChanged);
}

QtLottieWidget::~QtLottieWidget()
{
    dispose();
}

void QtLottieWidget::dispose()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    if (m_drawEngine) {
        m_drawEngine->release();
        m_drawEngine = nullptr;
    }
}

void QtLottieWidget::pause()
{
    if (available()) {
        m_drawEngine->pause();
    }
}

void QtLottieWidget::resume()
{
    if (available()) {
        m_drawEngine->resume();
    }
}

QSize QtLottieWidget::minimumSizeHint() const
{
    // Our lottie backend will fail to paint if the size of the widget is too small.
    // This size will be ignored if you set the size policy or minimum size explicitly.
    return {50, 50};
}

QString QtLottieWidget::backend() const
{
    return available() ? m_drawEngine->name() : QStringLiteral("Unknown");
}

QUrl QtLottieWidget::source() const
{
    return m_source;
}

void QtLottieWidget::setSource(const QUrl &value)
{
    if (!value.isValid()) {
        qWarning() << value << "is not a valid URL.";
        return;
    }
    if (m_source != value) {
        m_source = value;
        if (available()) {
            if (!m_drawEngine->setSource(value)) {
                qWarning() << "Failed to start playing.";
            }
        }
        Q_EMIT sourceChanged();
    }
}

int QtLottieWidget::frameRate() const
{
    // TODO: is the fallback value appropriate?
    return available() ? m_drawEngine->frameRate() : 30;
}

int QtLottieWidget::duration() const
{
    // TODO: is the fallback value appropriate?
    return available() ? m_drawEngine->duration() : 0;
}

QSize QtLottieWidget::sourceSize() const
{
    // TODO: is the fallback value appropriate?
    return available() ? m_drawEngine->size() : QSize{50, 50};
}

int QtLottieWidget::loops() const
{
    return available() ? m_drawEngine->loops() : 0;
}

void QtLottieWidget::setLoops(const int value)
{
    if (available()) {
        m_drawEngine->setLoops(value);
    }
}

bool QtLottieWidget::available() const
{
    return (m_drawEngine && m_drawEngine->available());
}

void QtLottieWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    if (available()) {
        m_drawEngine->paint(&painter, size());
    }
}
