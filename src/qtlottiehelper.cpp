#include "qtlottiehelper.h"
#include <QtGui/qpainter.h>
#include <QtCore/qlibrary.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qvariant.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>

using lottie_animation_destroy_ptr = void(*)(Lottie_Animation *animation);
using lottie_animation_from_data_ptr = Lottie_Animation *(*)(const char *data, const char *key, const char *resource_path);
using lottie_animation_get_framerate_ptr = double(*)(const Lottie_Animation *animation);
using lottie_animation_get_totalframe_ptr = size_t(*)(const Lottie_Animation *animation);
using lottie_animation_render_ptr = void(*)(Lottie_Animation *animation, size_t frame_num, uint32_t *buffer, size_t width, size_t height, size_t bytes_per_line);

struct rlottie_data {
    lottie_animation_destroy_ptr lottie_animation_destroy_pfn = nullptr;
    lottie_animation_from_data_ptr lottie_animation_from_data_pfn = nullptr;
    lottie_animation_get_framerate_ptr lottie_animation_get_framerate_pfn = nullptr;
    lottie_animation_get_totalframe_ptr lottie_animation_get_totalframe_pfn = nullptr;
    lottie_animation_render_ptr lottie_animation_render_pfn = nullptr;

    rlottie_data()
    {
        QLibrary library(QStringLiteral("rlottie"));
        lottie_animation_destroy_pfn = reinterpret_cast<lottie_animation_destroy_ptr>(library.resolve("lottie_animation_destroy"));
        lottie_animation_from_data_pfn = reinterpret_cast<lottie_animation_from_data_ptr>(library.resolve("lottie_animation_from_data"));
        lottie_animation_get_framerate_pfn = reinterpret_cast<lottie_animation_get_framerate_ptr>(library.resolve("lottie_animation_get_framerate"));
        lottie_animation_get_totalframe_pfn = reinterpret_cast<lottie_animation_get_totalframe_ptr>(library.resolve("lottie_animation_get_totalframe"));
        lottie_animation_render_pfn = reinterpret_cast<lottie_animation_render_ptr>(library.resolve("lottie_animation_render"));
    }

    bool isLoaded() const {
        return lottie_animation_destroy_pfn && lottie_animation_from_data_pfn && lottie_animation_get_framerate_pfn && lottie_animation_get_totalframe_pfn && lottie_animation_render_pfn;
    }
};

Q_GLOBAL_STATIC(rlottie_data, rlottie)

QtLottieHelper::QtLottieHelper(QObject *parent) : QObject(parent)
{
    Q_ASSERT(rlottie()->isLoaded());
    if (!rlottie()->isLoaded()) {
        qCritical() << "rlottie not loaded.";
        return;
    }
    m_window = QtLottieHelper::parent();
    Q_ASSERT(m_window);
    if (!m_window) {
        qWarning() << "Parent must be a QWidget or QQuickItem.";
        return;
    }
    Q_ASSERT(m_window->isWidgetType() || m_window->inherits("QQuickItem"));
    if (!m_window->isWidgetType() && !m_window->inherits("QQuickItem")) {
        qWarning() << m_window << "is not a QWidget or QQuickItem.";
        return;
    }
    connect(&m_timer, &QTimer::timeout, this, &QtLottieHelper::onTimerTicked);
}

QtLottieHelper::~QtLottieHelper()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    if (m_animation && rlottie()->lottie_animation_destroy_pfn) {
        rlottie()->lottie_animation_destroy_pfn(m_animation);
    }
}

bool QtLottieHelper::start(const QString &jsonFilePath, const QString &resource)
{
    Q_ASSERT(rlottie()->lottie_animation_from_data_pfn);
    Q_ASSERT(rlottie()->lottie_animation_get_framerate_pfn);
    Q_ASSERT(rlottie()->lottie_animation_get_totalframe_pfn);
    if (!rlottie()->lottie_animation_from_data_pfn || !rlottie()->lottie_animation_get_framerate_pfn || !rlottie()->lottie_animation_get_totalframe_pfn) {
        qWarning() << "rlottie is not loaded.";
        return false;
    }
    Q_ASSERT(QFile::exists(jsonFilePath));
    if (!QFile::exists(jsonFilePath)) {
        qWarning() << "json file doesn't exist:" << jsonFilePath;
        return false;
    }
    QFile file(jsonFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Failed to open the json file:" << jsonFilePath;
        return false;
    }
    const QByteArray jsonBuffer = file.readAll();
    file.close();
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    // TODO: check is there a rlottie API to do this.
    const QJsonObject jsonObj = QJsonDocument::fromJson(jsonBuffer).object();
    m_width = jsonObj.value(QStringLiteral("w")).toInt(50);
    m_height = jsonObj.value(QStringLiteral("h")).toInt(50);
    m_frameBuffer.reset(new char[m_width * m_height * 32 / 8]);
    m_animation = rlottie()->lottie_animation_from_data_pfn(jsonBuffer.constData(), jsonBuffer.constData(), resource.toUtf8().data());
    if (!m_animation) {
        qWarning() << "Failed to create lottie animation.";
        return false;
    }
    const double frameRate = rlottie()->lottie_animation_get_framerate_pfn(m_animation);
    m_totalFrame = rlottie()->lottie_animation_get_totalframe_pfn(m_animation);
    m_timer.setInterval(qRound(1000 / frameRate));
    m_timer.start();
    return true;
}

void QtLottieHelper::paint(QPainter *painter) const
{
    Q_ASSERT(painter);
    if (!painter) {
        return;
    }
    if (!m_animation) {
        // start() is not called, just ignore this paintEvent.
        return;
    }
    if (!m_hasFirstUpdate) {
        return;
    }
    painter->save();
    QImage img(m_width, m_height, QImage::Format_ARGB32);
    for (int y = 0; y < img.height(); ++y) {
        char *p = m_frameBuffer.data() + y * img.bytesPerLine();
        memcpy(img.scanLine(y),	p, img.bytesPerLine());
    }
    QSize size = {};
    if (m_window->isWidgetType()) {
        size = m_window->property("size").toSize();
    } else {
        const qreal w = m_window->property("width").toReal();
        const qreal h = m_window->property("height").toReal();
        size = {qRound(w), qRound(h)};
    }
    // TODO: let the user be able to set the scale mode.
    painter->drawImage(QPoint{0, 0}, img.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    painter->restore();
}

void QtLottieHelper::onTimerTicked()
{
    Q_ASSERT(rlottie()->lottie_animation_render_pfn);
    if (!rlottie()->lottie_animation_render_pfn) {
        qWarning() << "rlottie not loaded.";
        return;
    }
    // We assert here because we can't continue executing this callback function if the animation is not created.
    Q_ASSERT(m_animation);
    if (!m_animation) {
        return;
    }
    rlottie()->lottie_animation_render_pfn(m_animation, m_currentFrame, reinterpret_cast<uint32_t *>(m_frameBuffer.data()), m_width, m_height, m_width * 32 / 8);
    if (m_totalFrame == m_currentFrame) {
        m_currentFrame = 0;
    } else {
        ++m_currentFrame;
    }
    m_hasFirstUpdate = true;
    QMetaObject::invokeMethod(m_window, m_window->isWidgetType() ? "update" : "update2");
}
