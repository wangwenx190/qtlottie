/*
 * MIT License
 *
 * Copyright (C) 2021 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "qtlottiedrawengine_skottie.h"
#include <QtCore/qlibrary.h>
#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QtCore/qfile.h>
#include <QtCore/qmutex.h>
#include <QtGui/qscreen.h>
#include <QtGui/qguiapplication.h>

static constexpr const char kFileName[] = "QTLOTTIE_SKOTTIE_FILENAME";

[[nodiscard]] static inline QString getSkottieLibraryName()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    return qEnvironmentVariable(kFileName, QStringLiteral("skottiewrapper"));
#else
    const QByteArray ba = qgetenv(kFileName);
    return ba.isEmpty() ? QStringLiteral("skottiewrapper") : QString::fromUtf8(ba);
#endif
}

using skottie_animation_from_file_ptr = Skottie_Animation*(*)(const char *path, const char *resource);
using skottie_animation_from_data_ptr = Skottie_Animation*(*)(void *data, size_t data_size, const char *resource);
using skottie_animation_get_size_ptr = void(*)(const Skottie_Animation *animation, size_t *width, size_t *height);
using skottie_animation_get_duration_ptr = double(*)(const Skottie_Animation *animation);
using skottie_animation_get_totalframe_ptr = size_t(*)(const Skottie_Animation *animation);
using skottie_animation_get_framerate_ptr = double(*)(const Skottie_Animation *animation);
using skottie_new_pixmap_ptr = Skottie_Pixmap*(*)();
using skottie_new_pixmap_wh_ptr = Skottie_Pixmap*(*)(size_t width, size_t height, void *buffer);
using skottie_get_pixmap_buffer_ptr = const void*(*)(Skottie_Pixmap *pixmap);
using skottie_delete_pixmap_ptr = void(*)(Skottie_Pixmap *pixmap);
using skottie_animation_render_ptr = void(*)(Skottie_Animation *animation, size_t frame_num, Skottie_Pixmap *pixmap);
using skottie_animation_render_scale_ptr = void(*)(Skottie_Animation *animation, size_t frame_num, Skottie_Pixmap *pixmap);
using skottie_animation_destroy_ptr = void(*)(Skottie_Animation *animation);

class skottie_data
{
    Q_DISABLE_COPY_MOVE(skottie_data)

public:
    mutable QMutex mutex;

    skottie_animation_from_file_ptr skottie_animation_from_file_pfn = nullptr;
    skottie_animation_from_data_ptr skottie_animation_from_data_pfn = nullptr;
    skottie_animation_get_size_ptr skottie_animation_get_size_pfn = nullptr;
    skottie_animation_get_duration_ptr skottie_animation_get_duration_pfn = nullptr;
    skottie_animation_get_totalframe_ptr skottie_animation_get_totalframe_pfn = nullptr;
    skottie_animation_get_framerate_ptr skottie_animation_get_framerate_pfn = nullptr;
    skottie_new_pixmap_ptr skottie_new_pixmap_pfn = nullptr;
    skottie_new_pixmap_wh_ptr skottie_new_pixmap_wh_pfn = nullptr;
    skottie_get_pixmap_buffer_ptr skottie_get_pixmap_buffer_pfn = nullptr;
    skottie_delete_pixmap_ptr skottie_delete_pixmap_pfn = nullptr;
    skottie_animation_render_ptr skottie_animation_render_pfn = nullptr;
    skottie_animation_render_scale_ptr skottie_animation_render_scale_pfn = nullptr;
    skottie_animation_destroy_ptr skottie_animation_destroy_pfn = nullptr;

    explicit skottie_data()
    {
        const bool result = load();
        Q_UNUSED(result);
    }

    ~skottie_data()
    {
        const bool result = unload();
        Q_UNUSED(result);
    }

    [[nodiscard]] bool load(const QString &libName = {})
    {
        if (isLoaded()) {
            qDebug() << "The skottie library has already been loaded.";
            return true;
        }

        const QMutexLocker locker(&mutex);

        library.setFileName(libName.isEmpty() ? getSkottieLibraryName() : libName);
        if (!library.load()) {
            qWarning() << "Failed to load skottie library:" << library.errorString();
            return false;
        }
        qDebug() << "skottie library loaded successfully from" << library.fileName();

        #define RESOLVE(API) \
            API##_pfn = reinterpret_cast<API##_ptr>(library.resolve(#API)); \
            if (!API##_pfn) { \
                qWarning() << "Failed to resolve " #API; \
                return false; \
            }

        RESOLVE(skottie_animation_from_file)
        RESOLVE(skottie_animation_from_data)
        RESOLVE(skottie_animation_get_size)
        RESOLVE(skottie_animation_get_duration)
        RESOLVE(skottie_animation_get_totalframe)
        RESOLVE(skottie_animation_get_framerate)
        RESOLVE(skottie_new_pixmap)
        RESOLVE(skottie_new_pixmap_wh)
        RESOLVE(skottie_get_pixmap_buffer)
        RESOLVE(skottie_delete_pixmap)
        RESOLVE(skottie_animation_render)
        RESOLVE(skottie_animation_render_scale)
        RESOLVE(skottie_animation_destroy)

        #undef RESOLVE

        return true;
    }

    [[nodiscard]] bool unload()
    {
        if (!isLoaded()) {
            qDebug() << "The skottie library has already been unloaded.";
            return true;
        }

        const QMutexLocker locker(&mutex);

        skottie_animation_from_file_pfn = nullptr;
        skottie_animation_from_data_pfn = nullptr;
        skottie_animation_get_size_pfn = nullptr;
        skottie_animation_get_duration_pfn = nullptr;
        skottie_animation_get_totalframe_pfn = nullptr;
        skottie_animation_get_framerate_pfn = nullptr;
        skottie_new_pixmap_pfn = nullptr;
        skottie_new_pixmap_wh_pfn = nullptr;
        skottie_get_pixmap_buffer_pfn = nullptr;
        skottie_delete_pixmap_pfn = nullptr;
        skottie_animation_render_pfn = nullptr;
        skottie_animation_render_scale_pfn = nullptr;
        skottie_animation_destroy_pfn = nullptr;

        if (!library.unload()) {
            qWarning() << "Failed to unload skottie library:" << library.errorString();
            return false;
        }

        return true;
    }

    [[nodiscard]] bool isLoaded() const
    {
        const QMutexLocker locker(&mutex);

        return skottie_animation_from_file_pfn
               && skottie_animation_from_data_pfn && skottie_animation_get_size_pfn
               && skottie_animation_get_duration_pfn && skottie_animation_get_totalframe_pfn
               && skottie_animation_get_framerate_pfn && skottie_new_pixmap_pfn
               && skottie_new_pixmap_wh_pfn && skottie_get_pixmap_buffer_pfn
               && skottie_delete_pixmap_pfn && skottie_animation_render_pfn
               && skottie_animation_render_scale_pfn && skottie_animation_destroy_pfn;
    }

private:
    QLibrary library;
};

Q_GLOBAL_STATIC(skottie_data, skottie)

QtLottieSkottieEngine::QtLottieSkottieEngine(QObject *parent) : QtLottieDrawEngine(parent)
{
    if (skottie()->isLoaded()) {
        if (const auto screen = QGuiApplication::primaryScreen()) {
            m_devicePixelRatio = screen->devicePixelRatio();
        } else {
            m_devicePixelRatio = 1.0;
        }
    } else {
        qWarning() << "The skottie backend is not available due to can't load skottie library.";
    }
}

QtLottieSkottieEngine::~QtLottieSkottieEngine()
{
    if (!skottie()->isLoaded()) {
        return;
    }
    if (m_animation) {
        skottie()->skottie_animation_destroy_pfn(m_animation);
    }
}

void QtLottieSkottieEngine::paint(QPainter *painter, const QSize &s)
{
    Q_ASSERT(painter);
    if (!painter) {
        return;
    }
    Q_ASSERT(s.isValid());
    if (!s.isValid()) {
        qWarning() << s << "is not a valid size.";
        return;
    }
    if (!m_animation) {
        // lottie animation is not created, mostly due to setSource() not called.
        // Or the skottie library is not loaded. Safe to ignore.
        return;
    }
    if (!skottie()->isLoaded()) {
        qWarning() << Q_FUNC_INFO << "some necessary skottie functions are not available.";
        return;
    }
    if (m_shouldStop) {
        return;
    }
    if (!m_hasFirstUpdate) {
        return;
    }
    const int width = s.width();
    const int height = s.height();
    QScopedArrayPointer<char> buffer(new char[width * height * 4]);
    Skottie_Pixmap *pixmap = nullptr;
    skottie()->mutex.lock();
    if (s == size()) {
        pixmap = skottie()->skottie_new_pixmap_pfn();
        skottie()->skottie_animation_render_pfn(m_animation, m_currentFrame, pixmap);
    } else {
        pixmap = skottie()->skottie_new_pixmap_wh_pfn(width, height, buffer.data());
        skottie()->skottie_animation_render_scale_pfn(m_animation, m_currentFrame, pixmap);
    }
    const void *addr = skottie()->skottie_get_pixmap_buffer_pfn(pixmap);
    skottie()->mutex.unlock();
    QImage image(width, height, QImage::Format_ARGB32);
    for (int i = 0; i != height; ++i) {
        const char *p = static_cast<const char *>(addr) + i * image.bytesPerLine();
        std::memcpy(image.scanLine(i), p, image.bytesPerLine());
    }
    skottie()->mutex.lock();
    skottie()->skottie_delete_pixmap_pfn(pixmap);
    skottie()->mutex.unlock();
    painter->drawImage(QPoint{0, 0}, image);
}

void QtLottieSkottieEngine::render(const QSize &s)
{
    Q_UNUSED(s);
    if (!m_animation) {
        // lottie animation is not created, mostly due to setSource() not called.
        // Or the skottie library is not loaded. Safe to ignore.
        return;
    }
    if (m_shouldStop) {
        return;
    }
    if (m_currentFrame >= m_totalFrame) {
        m_currentFrame = 0;
        // negative number means infinite loops.
        if (m_loops > 0) {
            ++m_loopTimes;
            if (m_loopTimes >= m_loops) {
                m_loopTimes = 0;
                m_shouldStop = true;
                Q_EMIT playingChanged(false);
                return;
            }
        }
    } else {
        ++m_currentFrame;
    }
    m_hasFirstUpdate = true;
    Q_EMIT needsRepaint();
}

void QtLottieSkottieEngine::release()
{
    // TODO: ref count
    delete this;
}

QString QtLottieSkottieEngine::name() const
{
    return QStringLiteral("skottie");
}

QUrl QtLottieSkottieEngine::source() const
{
    return m_source;
}

bool QtLottieSkottieEngine::setSource(const QUrl &value)
{
    if (!skottie()->isLoaded()) {
        qWarning() << Q_FUNC_INFO << "some necessary skottie functions are not available.";
        return false;
    }
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        qWarning() << value << "is not a valid URL.";
        return false;
    }
    if (m_source == value) {
        // debug output?
        return false; // or true?
    }
    QString jsonFilePath = {};
    if (value.scheme() == QStringLiteral("qrc")) {
        jsonFilePath = value.toString();
        // QFile can't recognize url.
        jsonFilePath.replace(QStringLiteral("qrc:"), QStringLiteral(":"), Qt::CaseInsensitive);
        jsonFilePath.replace(QStringLiteral(":///"), QStringLiteral(":/"));
    } else {
        jsonFilePath = value.isLocalFile() ? value.toLocalFile() : value.url();
    }
    Q_ASSERT(QFile::exists(jsonFilePath));
    if (!QFile::exists(jsonFilePath)) {
        qWarning() << jsonFilePath << "doesn't exist.";
        return false;
    }
    QFile file(jsonFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Failed to open the JSON file:" << jsonFilePath;
        return false;
    }
    const QByteArray jsonBuffer = file.readAll();
    file.close();
    if (jsonBuffer.isEmpty()) {
        qWarning() << "File is empty:" << jsonFilePath;
        return false;
    }
    // TODO: support embeded resources.
    const QString resDirPath = QCoreApplication::applicationDirPath();
    skottie()->mutex.lock();
    m_animation = skottie()->skottie_animation_from_data_pfn(const_cast<char *>(jsonBuffer.data()), jsonBuffer.length(), resDirPath.toUtf8().constData());
    skottie()->mutex.unlock();
    if (!m_animation) {
        qWarning() << "Failed to create lottie animation.";
        return false;
    }
    m_source = value;
    skottie()->mutex.lock();
    skottie()->skottie_animation_get_size_pfn(m_animation, reinterpret_cast<size_t *>(&m_width), reinterpret_cast<size_t *>(&m_height));
    m_frameRate = qRound64(skottie()->skottie_animation_get_framerate_pfn(m_animation));
    m_duration = qRound64(skottie()->skottie_animation_get_duration_pfn(m_animation));
    m_totalFrame = skottie()->skottie_animation_get_totalframe_pfn(m_animation);
    skottie()->mutex.unlock();
    // Clear previous status.
    m_currentFrame = 0;
    m_loopTimes = 0;
    m_shouldStop = false;
    Q_EMIT sourceChanged(m_source);
    Q_EMIT sizeChanged(size());
    Q_EMIT frameRateChanged(m_frameRate);
    Q_EMIT durationChanged(m_duration);
    Q_EMIT playingChanged(true);
    return true;
}

qint64 QtLottieSkottieEngine::frameRate() const
{
    return m_frameRate;
}

qint64 QtLottieSkottieEngine::duration() const
{
    return m_duration;
}

QSize QtLottieSkottieEngine::size() const
{
    return {int(m_width), int(m_height)};
}

qint64 QtLottieSkottieEngine::loops() const
{
    return m_loops;
}

void QtLottieSkottieEngine::setLoops(const qint64 value)
{
    if (m_loops != value) {
        m_loops = value;
        Q_EMIT loopsChanged(m_loops);
        // Also clear previous status.
        m_loopTimes = 0;
        m_shouldStop = false;
        Q_EMIT playingChanged(true);
    }
}

bool QtLottieSkottieEngine::available() const
{
    return skottie()->isLoaded();
}

bool QtLottieSkottieEngine::playing() const
{
    return (m_animation && !m_shouldStop);
}

qreal QtLottieSkottieEngine::devicePixelRatio() const
{
    return m_devicePixelRatio;
}

void QtLottieSkottieEngine::setDevicePixelRatio(const qreal value)
{
    if (qFuzzyCompare(m_devicePixelRatio, value)) {
        return;
    }
    m_devicePixelRatio = value;
    Q_EMIT devicePixelRatioChanged(m_devicePixelRatio);
}

void QtLottieSkottieEngine::pause()
{
    if (m_animation && !m_shouldStop) {
        m_shouldStop = true;
        Q_EMIT playingChanged(false);
    }
}

void QtLottieSkottieEngine::resume()
{
    if (m_animation && m_shouldStop) {
        m_shouldStop = false;
        Q_EMIT playingChanged(true);
    }
}
