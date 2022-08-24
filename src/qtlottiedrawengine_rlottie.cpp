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

#include "qtlottiedrawengine_rlottie.h"
#include <QtGui/qpainter.h>
#include <QtCore/qlibrary.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qvariant.h>
#include <QtCore/qmutex.h>
#include <QtGui/qscreen.h>
#include <QtGui/qguiapplication.h>

static constexpr const char kFileName[] = "QTLOTTIE_RLOTTIE_FILENAME";

[[nodiscard]] static inline QString getRLottieLibraryName()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    return qEnvironmentVariable(kFileName, QStringLiteral("rlottie"));
#else
    const QByteArray ba = qgetenv(kFileName);
    return ba.isEmpty() ? QStringLiteral("rlottie") : QString::fromUtf8(ba);
#endif
}

using lottie_init_ptr = void(*)();
using lottie_shutdown_ptr = void(*)();
using lottie_animation_destroy_ptr = void(*)(Lottie_Animation *animation);
using lottie_animation_from_data_ptr = Lottie_Animation *(*)(const char *data, const char *key, const char *resource_path);
using lottie_animation_get_framerate_ptr = double(*)(const Lottie_Animation *animation);
using lottie_animation_get_totalframe_ptr = size_t(*)(const Lottie_Animation *animation);
using lottie_animation_render_ptr = void(*)(Lottie_Animation *animation, size_t frame_num, uint32_t *buffer, size_t width, size_t height, size_t bytes_per_line);
using lottie_animation_get_size_ptr = void(*)(const Lottie_Animation *animation, size_t *width, size_t *height);
using lottie_animation_get_duration_ptr = double(*)(const Lottie_Animation *animation);

class rlottie_data
{
    Q_DISABLE_COPY_MOVE(rlottie_data)

public:
    mutable QMutex mutex;

    lottie_init_ptr lottie_init_pfn = nullptr;
    lottie_shutdown_ptr lottie_shutdown_pfn = nullptr;
    lottie_animation_destroy_ptr lottie_animation_destroy_pfn = nullptr;
    lottie_animation_from_data_ptr lottie_animation_from_data_pfn = nullptr;
    lottie_animation_get_framerate_ptr lottie_animation_get_framerate_pfn = nullptr;
    lottie_animation_get_totalframe_ptr lottie_animation_get_totalframe_pfn = nullptr;
    lottie_animation_render_ptr lottie_animation_render_pfn = nullptr;
    lottie_animation_get_size_ptr lottie_animation_get_size_pfn = nullptr;
    lottie_animation_get_duration_ptr lottie_animation_get_duration_pfn = nullptr;

    explicit rlottie_data()
    {
        const bool result = load();
        Q_UNUSED(result);
    }

    ~rlottie_data()
    {
        const bool result = unload();
        Q_UNUSED(result);
    }

    [[nodiscard]] bool load(const QString &libName = {})
    {
        if (isLoaded()) {
            qDebug() << "rlottie library has already been loaded.";
            return true;
        }

        const QMutexLocker locker(&mutex);

        library.setFileName(libName.isEmpty() ? getRLottieLibraryName() : libName);
        if (!library.load()) {
            qWarning() << "Failed to load rlottie library:" << library.errorString();
            return false;
        }
        qDebug() << "rlottie library loaded successfully from" << library.fileName();

        #define RESOLVE(API) \
            API##_pfn = reinterpret_cast<API##_ptr>(library.resolve(#API)); \
            if (!API##_pfn) { \
                qWarning() << "Failed to resolve " #API; \
                return false; \
            }

        RESOLVE(lottie_init)
        RESOLVE(lottie_shutdown)
        RESOLVE(lottie_animation_destroy)
        RESOLVE(lottie_animation_from_data)
        RESOLVE(lottie_animation_get_framerate)
        RESOLVE(lottie_animation_get_totalframe)
        RESOLVE(lottie_animation_render)
        RESOLVE(lottie_animation_get_size)
        RESOLVE(lottie_animation_get_duration)

        #undef RESOLVE

        return true;
    }

    [[nodiscard]] bool unload()
    {
        if (!isLoaded()) {
            qDebug() << "rlottie library has already been unloaded.";
            return true;
        }

        const QMutexLocker locker(&mutex);

        lottie_init_pfn = nullptr;
        lottie_shutdown_pfn = nullptr;
        lottie_animation_destroy_pfn = nullptr;
        lottie_animation_from_data_pfn = nullptr;
        lottie_animation_get_framerate_pfn = nullptr;
        lottie_animation_get_totalframe_pfn = nullptr;
        lottie_animation_render_pfn = nullptr;
        lottie_animation_get_size_pfn = nullptr;
        lottie_animation_get_duration_pfn = nullptr;

        if (!library.unload()) {
            qWarning() << "Failed to unload rlottie library:" << library.errorString();
            return false;
        }

        return true;
    }

    [[nodiscard]] bool isLoaded() const
    {
        const QMutexLocker locker(&mutex);

        return lottie_init_pfn && lottie_shutdown_pfn && lottie_animation_destroy_pfn
                && lottie_animation_from_data_pfn && lottie_animation_get_framerate_pfn
                && lottie_animation_get_totalframe_pfn && lottie_animation_render_pfn
                && lottie_animation_get_size_pfn && lottie_animation_get_duration_pfn;
    }

private:
    QLibrary library;
};

Q_GLOBAL_STATIC(rlottie_data, rlottie)

QtLottieRLottieEngine::QtLottieRLottieEngine(QObject *parent) : QtLottieDrawEngine(parent)
{
    if (rlottie()->isLoaded()) {
        if (const auto screen = QGuiApplication::primaryScreen()) {
            m_devicePixelRatio = screen->devicePixelRatio();
        } else {
            m_devicePixelRatio = 1.0;
        }
        const QMutexLocker locker(&rlottie()->mutex);
        rlottie()->lottie_init_pfn();
    } else {
        qWarning() << "The rlottie backend is not available due to can't load rlottie library.";
    }
}

QtLottieRLottieEngine::~QtLottieRLottieEngine()
{
    if (!rlottie()->isLoaded()) {
        return;
    }
    const QMutexLocker locker(&rlottie()->mutex);
    if (m_animation) {
        rlottie()->lottie_animation_destroy_pfn(m_animation);
    }
    rlottie()->lottie_shutdown_pfn();
}

bool QtLottieRLottieEngine::setSource(const QUrl &value)
{
    if (!rlottie()->isLoaded()) {
        qWarning() << Q_FUNC_INFO << "some necessary rlottie functions are not available.";
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
    rlottie()->mutex.lock();
    m_animation = rlottie()->lottie_animation_from_data_pfn(jsonBuffer.constData(), jsonBuffer.constData(), resDirPath.toUtf8().constData());
    rlottie()->mutex.unlock();
    if (!m_animation) {
        qWarning() << "Failed to create lottie animation.";
        return false;
    }
    m_source = value;
    rlottie()->mutex.lock();
    rlottie()->lottie_animation_get_size_pfn(m_animation, reinterpret_cast<size_t *>(&m_width), reinterpret_cast<size_t *>(&m_height));
    m_frameRate = qRound64(rlottie()->lottie_animation_get_framerate_pfn(m_animation));
    m_duration = qRound64(rlottie()->lottie_animation_get_duration_pfn(m_animation));
    m_totalFrame = rlottie()->lottie_animation_get_totalframe_pfn(m_animation);
    rlottie()->mutex.unlock();
    m_frameBuffer.reset(new char[m_width * m_height * 32 / 8]);
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

qint64 QtLottieRLottieEngine::frameRate() const
{
    return m_frameRate;
}

qint64 QtLottieRLottieEngine::duration() const
{
    return m_duration;
}

QSize QtLottieRLottieEngine::size() const
{
    return {int(m_width), int(m_height)};
}

qint64 QtLottieRLottieEngine::loops() const
{
    return m_loops;
}

void QtLottieRLottieEngine::setLoops(const qint64 value)
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

bool QtLottieRLottieEngine::available() const
{
    return rlottie()->isLoaded();
}

bool QtLottieRLottieEngine::playing() const
{
    return (m_animation && !m_shouldStop);
}

qreal QtLottieRLottieEngine::devicePixelRatio() const
{
    return m_devicePixelRatio;
}

void QtLottieRLottieEngine::setDevicePixelRatio(const qreal value)
{
    if (qFuzzyCompare(m_devicePixelRatio, value)) {
        return;
    }
    m_devicePixelRatio = value;
    Q_EMIT devicePixelRatioChanged(m_devicePixelRatio);
}

void QtLottieRLottieEngine::pause()
{
    if (m_animation && !m_shouldStop) {
        m_shouldStop = true;
        Q_EMIT playingChanged(false);
    }
}

void QtLottieRLottieEngine::resume()
{
    if (m_animation && m_shouldStop) {
        m_shouldStop = false;
        Q_EMIT playingChanged(true);
    }
}

void QtLottieRLottieEngine::paint(QPainter *painter, const QSize &s)
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
        // Or the rlottie library is not loaded. Safe to ignore.
        return;
    }
    if (m_shouldStop) {
        return;
    }
    if (!m_hasFirstUpdate) {
        return;
    }
    QImage image(m_width, m_height, QImage::Format_ARGB32);
    for (int i = 0; i != image.height(); ++i) {
        char *p = m_frameBuffer.data() + i * image.bytesPerLine();
        std::memcpy(image.scanLine(i), p, image.bytesPerLine());
    }
    // TODO: let the user be able to set the scale mode.
    // "Qt::SmoothTransformation" is a must otherwise the scaled image will become fuzzy.
    painter->drawImage(QPoint{0, 0}, (s == size()) ? image : image.scaled(s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void QtLottieRLottieEngine::render(const QSize &s)
{
    Q_UNUSED(s);
    if (!m_animation) {
        // lottie animation is not created, mostly due to setSource() not called.
        // Or the rlottie library is not loaded. Safe to ignore.
        return;
    }
    if (!rlottie()->isLoaded()) {
        qWarning() << Q_FUNC_INFO << "some necessary rlottie functions are not available.";
        return;
    }
    if (m_shouldStop) {
        return;
    }
    rlottie()->mutex.lock();
    rlottie()->lottie_animation_render_pfn(m_animation, m_currentFrame, reinterpret_cast<uint32_t *>(m_frameBuffer.data()), m_width, m_height, m_width * 32 / 8);
    rlottie()->mutex.unlock();
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

void QtLottieRLottieEngine::release()
{
    // TODO: ref count
    delete this;
}

QString QtLottieRLottieEngine::name() const
{
    return QStringLiteral("rlottie");
}

QUrl QtLottieRLottieEngine::source() const
{
    return m_source;
}
