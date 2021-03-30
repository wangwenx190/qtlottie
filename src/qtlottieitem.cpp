#include "qtlottieitem.h"
#include "qtlottiedrawengine.h"
#include "qtlottiedrawenginefactory.h"
#include <QtCore/qdebug.h>

QtLottieItem::QtLottieItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    // We prefer Skottie over RLottie.
    m_drawEngine = QtLottieDrawEngineFactory::create("skottie");
    if (!m_drawEngine || !m_drawEngine->available()) {
        qWarning() << "Failed to initialize the skottie backend.";
        dispose();
        m_drawEngine = QtLottieDrawEngineFactory::create("rlottie");
        if (!m_drawEngine || !m_drawEngine->available()) {
            qWarning() << "Failed to initialize the rlottie backend.";
            dispose();
            return;
        }
    }
    m_timer.setTimerType(Qt::PreciseTimer); // Is this necesary?
    connect(&m_timer, &QTimer::timeout, this, [this](){
        if (m_drawEngine->playing()) {
            m_drawEngine->render({qRound(width()), qRound(height())});
        }
    });
    connect(m_drawEngine, &QtLottieDrawEngine::needsRepaint, this, [this](){
        update();
    });
    connect(m_drawEngine, &QtLottieDrawEngine::frameRateChanged, this, [this](){
        if (m_timer.isActive()) {
            m_timer.stop();
        }
        m_timer.setInterval(1000 / m_drawEngine->frameRate());
        if (m_drawEngine->playing()) {
            m_timer.start();
        }
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
}

QtLottieItem::~QtLottieItem()
{
    dispose();
}

void QtLottieItem::paint(QPainter *painter)
{
    Q_ASSERT(painter);
    if (!painter) {
        return;
    }
    if (m_drawEngine) {
        m_drawEngine->paint(painter, {qRound(width()), qRound(height())});
    }
}

void QtLottieItem::dispose()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    if (m_drawEngine) {
        m_drawEngine->release();
        m_drawEngine = nullptr;
    }
}

QUrl QtLottieItem::source() const
{
    return m_source;
}

void QtLottieItem::setSource(const QUrl &value)
{
    if (!value.isValid()) {
        qWarning() << value << "is not a valid URL.";
        return;
    }
    if (m_source != value) {
        m_source = value;
        if (m_drawEngine) {
            if (!m_drawEngine->setSource(value)) {
                qWarning() << "Failed to start playing.";
            }
        }
        Q_EMIT sourceChanged();
    }
}
