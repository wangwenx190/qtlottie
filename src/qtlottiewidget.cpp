#include "qtlottiewidget.h"
#include "qtlottiedrawengine.h"
#include "qtlottiedrawenginefactory.h"
#include <QtGui/qpainter.h>
#include <QtCore/qdebug.h>

QtLottieWidget::QtLottieWidget(QWidget *parent) : QWidget(parent)
{
    m_drawEngine = QtLottieDrawEngineFactory::create(QStringLiteral("rlottie"));
    if (!m_drawEngine) {
        qWarning() << "Failed to create the draw engine.";
        return;
    }
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
    });
    connect(m_drawEngine, &QtLottieDrawEngine::playingChanged, this, [this](){
        if (m_drawEngine->playing()) {
            if (!m_timer.isActive() && (m_timer.interval() > 0)) {
                m_timer.start();
            }
        } else {
            if (m_timer.isActive()) {
                m_timer.stop();
            }
        }
    });
}

QtLottieWidget::~QtLottieWidget()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    if (m_drawEngine) {
        m_drawEngine->release();
        m_drawEngine = nullptr;
    }
}

QSize QtLottieWidget::minimumSizeHint() const
{
    // The draw engine will fail to paint if the size of the widget is too small.
    return {50, 50};
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
        if (m_drawEngine) {
            if (!m_drawEngine->setSource(value)) {
                qWarning() << "Failed to start playing.";
            }
        }
        Q_EMIT sourceChanged();
    }
}

void QtLottieWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (m_drawEngine) {
        m_drawEngine->paint(&painter, size());
    }
    QWidget::paintEvent(event);
}
