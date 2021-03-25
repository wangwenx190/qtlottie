#include "qtlottiehelper.h"
#include <QtGui/qpainter.h>
#include <QtCore/qlibrary.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qvariant.h>

static const char _env_var_rlottie_name[] = "QTLOTTIE_RLOTTIE_NAME";

static inline QString getRLottieLibraryName()
{
    return qEnvironmentVariable(_env_var_rlottie_name, QStringLiteral("rlottie"));
}

using lottie_animation_destroy_ptr = void(*)(Lottie_Animation *animation);
using lottie_animation_from_data_ptr = Lottie_Animation *(*)(const char *data, const char *key, const char *resource_path);
using lottie_animation_get_framerate_ptr = double(*)(const Lottie_Animation *animation);
using lottie_animation_get_totalframe_ptr = size_t(*)(const Lottie_Animation *animation);
using lottie_animation_render_ptr = void(*)(Lottie_Animation *animation, size_t frame_num, uint32_t *buffer, size_t width, size_t height, size_t bytes_per_line);
using lottie_animation_get_size_ptr = void(*)(const Lottie_Animation *animation, size_t *width, size_t *height);

struct rlottie_data
{
    lottie_animation_destroy_ptr lottie_animation_destroy_pfn = nullptr;
    lottie_animation_from_data_ptr lottie_animation_from_data_pfn = nullptr;
    lottie_animation_get_framerate_ptr lottie_animation_get_framerate_pfn = nullptr;
    lottie_animation_get_totalframe_ptr lottie_animation_get_totalframe_pfn = nullptr;
    lottie_animation_render_ptr lottie_animation_render_pfn = nullptr;
    lottie_animation_get_size_ptr lottie_animation_get_size_pfn = nullptr;

    rlottie_data()
    {
        load();
    }

    void load(const QString &libName = {})
    {
        QLibrary library(libName.isEmpty() ? getRLottieLibraryName() : libName);
        lottie_animation_destroy_pfn = reinterpret_cast<lottie_animation_destroy_ptr>(library.resolve("lottie_animation_destroy"));
        lottie_animation_from_data_pfn = reinterpret_cast<lottie_animation_from_data_ptr>(library.resolve("lottie_animation_from_data"));
        lottie_animation_get_framerate_pfn = reinterpret_cast<lottie_animation_get_framerate_ptr>(library.resolve("lottie_animation_get_framerate"));
        lottie_animation_get_totalframe_pfn = reinterpret_cast<lottie_animation_get_totalframe_ptr>(library.resolve("lottie_animation_get_totalframe"));
        lottie_animation_render_pfn = reinterpret_cast<lottie_animation_render_ptr>(library.resolve("lottie_animation_render"));
        lottie_animation_get_size_pfn = reinterpret_cast<lottie_animation_get_size_ptr>(library.resolve("lottie_animation_get_size"));
    }

    void unload()
    {
        lottie_animation_destroy_pfn = nullptr;
        lottie_animation_from_data_pfn = nullptr;
        lottie_animation_get_framerate_pfn = nullptr;
        lottie_animation_get_totalframe_pfn = nullptr;
        lottie_animation_render_pfn = nullptr;
        lottie_animation_get_size_pfn = nullptr;
    }

    void reload(const QString &libName = {})
    {
        unload();
        load(libName);
    }

    bool isLoaded() const
    {
        return lottie_animation_destroy_pfn && lottie_animation_from_data_pfn
                && lottie_animation_get_framerate_pfn && lottie_animation_get_totalframe_pfn
                && lottie_animation_render_pfn && lottie_animation_get_size_pfn;
    }
};

Q_GLOBAL_STATIC(rlottie_data, rlottie)

QtLottieHelper::QtLottieHelper(QObject *parent) : QObject(parent)
{
    if (!rlottie()->isLoaded()) {
        qWarning() << "rlottie not loaded.";
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

bool QtLottieHelper::start(const QString &jsonFilePath, const QString &resourceFolderPath)
{
    Q_ASSERT(rlottie()->lottie_animation_from_data_pfn);
    Q_ASSERT(rlottie()->lottie_animation_get_framerate_pfn);
    Q_ASSERT(rlottie()->lottie_animation_get_totalframe_pfn);
    Q_ASSERT(rlottie()->lottie_animation_get_size_pfn);
    if (!rlottie()->lottie_animation_from_data_pfn || !rlottie()->lottie_animation_get_framerate_pfn
            || !rlottie()->lottie_animation_get_totalframe_pfn || !rlottie()->lottie_animation_get_size_pfn) {
        qWarning() << "rlottie is not loaded.";
        return false;
    }
    Q_ASSERT(!jsonFilePath.isEmpty());
    Q_ASSERT(!resourceFolderPath.isEmpty());
    if (jsonFilePath.isEmpty() || resourceFolderPath.isEmpty()) {
        qWarning() << "The path of the JSON file and the resource folder can't be empty.";
        return false;
    }
    Q_ASSERT(QFile::exists(jsonFilePath));
    if (!QFile::exists(jsonFilePath)) {
        qWarning() << "JSON file doesn't exist:" << jsonFilePath;
        return false;
    }
    // TODO: check whether "resourceFolderPath" exists or not.
    QFile file(jsonFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Failed to open the JSON file:" << jsonFilePath;
        return false;
    }
    const QByteArray jsonBuffer = file.readAll();
    file.close();
    if (jsonBuffer.isEmpty()) {
        qWarning() << "Empty JSON content.";
        return false;
    }
    m_animation = rlottie()->lottie_animation_from_data_pfn(jsonBuffer.constData(), jsonBuffer.constData(), resourceFolderPath.toUtf8().constData());
    if (!m_animation) {
        qWarning() << "Failed to create lottie animation.";
        return false;
    }
    rlottie()->lottie_animation_get_size_pfn(m_animation, &m_width, &m_height);
    m_frameBuffer.reset(new char[m_width * m_height * 32 / 8]);
    const double frameRate = rlottie()->lottie_animation_get_framerate_pfn(m_animation);
    m_totalFrame = rlottie()->lottie_animation_get_totalframe_pfn(m_animation);
    if (m_timer.isActive()) {
        m_timer.stop();
    }
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
    if (!checkParent()) {
        return;
    }
    if (!m_animation) {
        // start() is not called, just ignore this paintEvent.
        return;
    }
    if (!m_hasFirstUpdate) {
        return;
    }
    QImage image(m_width, m_height, QImage::Format_ARGB32);
    for (int i = 0; i != image.height(); ++i) {
        char *p = m_frameBuffer.data() + i * image.bytesPerLine();
        memcpy(image.scanLine(i), p, image.bytesPerLine());
    }
    QSize size = {};
    const QObject *p = parent();
    if (p->isWidgetType()) {
        size = p->property("size").toSize();
    } else {
        // "size" is not a property of QQuickItem so we can't get it. But "width" and "height" are.
        const qreal w = p->property("width").toReal();
        const qreal h = p->property("height").toReal();
        size = {qRound(w), qRound(h)};
    }
    Q_ASSERT(size.isValid());
    if (!size.isValid()) {
        qWarning() << size << "is not a valid size.";
        return;
    }
    const bool needScale = (size != QSize{static_cast<int>(m_width), static_cast<int>(m_height)});
    // TODO: let the user be able to set the scale mode.
    // "Qt::SmoothTransformation" is a must otherwise the scaled image will become fuzzy.
    painter->drawImage(QPoint{0, 0}, needScale ? image.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation) : image);
}

bool QtLottieHelper::reloadRLottie(const QString &fileName) const
{
    rlottie()->reload(fileName);
    return rlottie()->isLoaded();
}

void QtLottieHelper::onTimerTicked()
{
    Q_ASSERT(rlottie()->lottie_animation_render_pfn);
    if (!rlottie()->lottie_animation_render_pfn) {
        qWarning() << "rlottie not loaded.";
        return;
    }
    // We assert here because we can't continue executing this function if the animation is not created.
    Q_ASSERT(m_animation);
    if (!m_animation) {
        return;
    }
    if (!checkParent()) {
        return;
    }
    rlottie()->lottie_animation_render_pfn(m_animation, m_currentFrame, reinterpret_cast<uint32_t *>(m_frameBuffer.data()), m_width, m_height, m_width * 32 / 8);
    if (m_currentFrame >= m_totalFrame) {
        m_currentFrame = 0;
    } else {
        ++m_currentFrame;
    }
    m_hasFirstUpdate = true;
    QObject *p = parent();
    // QQuickPaintedItem doesn't have a slot called "update".
    // It's "update()" function is not marked as slot, so we can't invoke it.
    QMetaObject::invokeMethod(p, p->isWidgetType() ? "update" : "update2");
}

bool QtLottieHelper::checkParent() const
{
    const QObject *p = parent();
    // We assert here because the whole class will not be usable if we don't have a parent.
    Q_ASSERT(p);
    if (!p) {
        qWarning() << "QtLottieHelper must have a valid parent.";
        return false;
    }
    const bool ret = p->isWidgetType() || p->inherits("QQuickItem");
    if (!ret) {
        qWarning() << "QtLottieHelper's parent must be a QWidget or QQuickItem.";
    }
    return ret;
}
