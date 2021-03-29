#include "qtlottiedrawengine_skottie.h"
#include <QtCore/qlibrary.h>
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>
#include <QtGui/qpainter.h>
#include <QtCore/qfile.h>

static const char _env_var_skottie_name[] = "QTLOTTIE_SKOTTIE_NAME";

static inline QString getSkottieLibraryName()
{
    return qEnvironmentVariable(_env_var_skottie_name, QStringLiteral("skottiewrapper"));
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
        if (skottieLib.isLoaded()) {
            qDebug() << "skottie library already loaded. Unloading ...";
            if (!unload()) {
                return false;
            }
        }
        skottieLib.setFileName(libName.isEmpty() ? getSkottieLibraryName() : libName);
        if (!skottieLib.load()) {
            qWarning() << "Failed to load skottie library:" << skottieLib.errorString();
            return false;
        }
        qDebug() << "skottie library loaded successfully from" << skottieLib.fileName();

        skottie_animation_from_file_pfn = reinterpret_cast<skottie_animation_from_file_ptr>(skottieLib.resolve("skottie_animation_from_file"));
        if (!skottie_animation_from_file_pfn) {
            qWarning() << "skottie_animation_from_file_pfn is null:" << skottieLib.errorString();
        }

        skottie_animation_from_data_pfn = reinterpret_cast<skottie_animation_from_data_ptr>(skottieLib.resolve("skottie_animation_from_data"));
        if (!skottie_animation_from_data_pfn) {
            qWarning() << "skottie_animation_from_data_pfn is null:" << skottieLib.errorString();
        }

        skottie_animation_get_size_pfn = reinterpret_cast<skottie_animation_get_size_ptr>(skottieLib.resolve("skottie_animation_get_size"));
        if (!skottie_animation_get_size_pfn) {
            qWarning() << "skottie_animation_get_size_pfn is null:" << skottieLib.errorString();
        }

        skottie_animation_get_duration_pfn = reinterpret_cast<skottie_animation_get_duration_ptr>(skottieLib.resolve("skottie_animation_get_duration"));
        if (!skottie_animation_get_duration_pfn) {
            qWarning() << "skottie_animation_get_duration_pfn is null:" << skottieLib.errorString();
        }

        skottie_animation_get_totalframe_pfn = reinterpret_cast<skottie_animation_get_totalframe_ptr>(skottieLib.resolve("skottie_animation_get_totalframe"));
        if (!skottie_animation_get_totalframe_pfn) {
            qWarning() << "skottie_animation_get_totalframe_pfn is null:" << skottieLib.errorString();
        }

        skottie_animation_get_framerate_pfn = reinterpret_cast<skottie_animation_get_framerate_ptr>(skottieLib.resolve("skottie_animation_get_framerate"));
        if (!skottie_animation_get_framerate_pfn) {
            qWarning() << "skottie_animation_get_framerate_pfn is null:" << skottieLib.errorString();
        }

        skottie_new_pixmap_pfn = reinterpret_cast<skottie_new_pixmap_ptr>(skottieLib.resolve("skottie_new_pixmap"));
        if (!skottie_new_pixmap_pfn) {
            qWarning() << "skottie_new_pixmap_pfn is null:" << skottieLib.errorString();
        }

        skottie_new_pixmap_wh_pfn = reinterpret_cast<skottie_new_pixmap_wh_ptr>(skottieLib.resolve("skottie_new_pixmap_wh"));
        if (!skottie_new_pixmap_wh_pfn) {
            qWarning() << "skottie_new_pixmap_wh_pfn is null:" << skottieLib.errorString();
        }

        skottie_get_pixmap_buffer_pfn = reinterpret_cast<skottie_get_pixmap_buffer_ptr>(skottieLib.resolve("skottie_get_pixmap_buffer"));
        if (!skottie_get_pixmap_buffer_pfn) {
            qWarning() << "skottie_get_pixmap_buffer_pfn is null:" << skottieLib.errorString();
        }

        skottie_delete_pixmap_pfn = reinterpret_cast<skottie_delete_pixmap_ptr>(skottieLib.resolve("skottie_delete_pixmap"));
        if (!skottie_delete_pixmap_pfn) {
            qWarning() << "skottie_delete_pixmap_pfn is null:" << skottieLib.errorString();
        }

        skottie_animation_render_pfn = reinterpret_cast<skottie_animation_render_ptr>(skottieLib.resolve("skottie_animation_render"));
        if (!skottie_animation_render_pfn) {
            qWarning() << "skottie_animation_render_pfn is null:" << skottieLib.errorString();
        }

        skottie_animation_render_scale_pfn = reinterpret_cast<skottie_animation_render_scale_ptr>(skottieLib.resolve("skottie_animation_render_scale"));
        if (!skottie_animation_render_scale_pfn) {
            qWarning() << "skottie_animation_render_scale_pfn is null:" << skottieLib.errorString();
        }

        skottie_animation_destroy_pfn = reinterpret_cast<skottie_animation_destroy_ptr>(skottieLib.resolve("skottie_animation_destroy"));
        if (!skottie_animation_destroy_pfn) {
            qWarning() << "skottie_animation_destroy_pfn is null:" << skottieLib.errorString();
        }

        return isLoaded();
    }

    [[nodiscard]] bool unload()
    {
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

        if (skottieLib.isLoaded()) {
            if (!skottieLib.unload()) {
                qWarning() << "Failed to unload skottie library:" << skottieLib.errorString();
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool isLoaded() const
    {
        return skottieLib.isLoaded() && skottie_animation_from_file_pfn
               && skottie_animation_from_data_pfn && skottie_animation_get_size_pfn
               && skottie_animation_get_duration_pfn && skottie_animation_get_totalframe_pfn
               && skottie_animation_get_framerate_pfn && skottie_new_pixmap_pfn
               && skottie_new_pixmap_wh_pfn && skottie_get_pixmap_buffer_pfn
               && skottie_delete_pixmap_pfn && skottie_animation_render_pfn
               && skottie_animation_render_scale_pfn && skottie_animation_destroy_pfn;
    }

private:
    QLibrary skottieLib;
};

Q_GLOBAL_STATIC(skottie_data, skottie)

QtLottieSkottieEngine::QtLottieSkottieEngine(QObject *parent) : QtLottieDrawEngine(parent)
{
    if (!skottie()->isLoaded()) {
        qWarning() << "skottie library not loaded.";
    }
}

QtLottieSkottieEngine::~QtLottieSkottieEngine()
{
    if (m_animation && skottie()->skottie_animation_destroy_pfn) {
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
    Q_ASSERT(skottie()->skottie_new_pixmap_wh_pfn);
    Q_ASSERT(skottie()->skottie_animation_render_scale_pfn);
    Q_ASSERT(skottie()->skottie_get_pixmap_buffer_pfn);
    Q_ASSERT(skottie()->skottie_delete_pixmap_pfn);
    if (!skottie()->skottie_new_pixmap_wh_pfn || !skottie()->skottie_animation_render_scale_pfn
        || !skottie()->skottie_get_pixmap_buffer_pfn || !skottie()->skottie_delete_pixmap_pfn) {
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
    const auto pixmap = skottie()->skottie_new_pixmap_wh_pfn(width, height, buffer.data());
    skottie()->skottie_animation_render_scale_pfn(m_animation, m_currentFrame, pixmap);
    const void *addr = skottie()->skottie_get_pixmap_buffer_pfn(pixmap);
    QImage image(width, height, QImage::Format_ARGB32);
    for (int i = 0; i != height; ++i) {
        const char *p = static_cast<const char *>(addr) + i * image.bytesPerLine();
        memcpy(image.scanLine(i), p, image.bytesPerLine());
    }
    skottie()->skottie_delete_pixmap_pfn(pixmap);
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->drawImage(QPoint{0, 0}, image);
    painter->restore();
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
                Q_EMIT playingChanged();
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
    Q_ASSERT(skottie()->skottie_animation_from_data_pfn);
    Q_ASSERT(skottie()->skottie_animation_get_size_pfn);
    Q_ASSERT(skottie()->skottie_animation_get_framerate_pfn);
    Q_ASSERT(skottie()->skottie_animation_get_duration_pfn);
    Q_ASSERT(skottie()->skottie_animation_get_totalframe_pfn);
    if (!skottie()->skottie_animation_from_data_pfn || !skottie()->skottie_animation_get_size_pfn
        || !skottie()->skottie_animation_get_framerate_pfn || !skottie()->skottie_animation_get_duration_pfn
        || !skottie()->skottie_animation_get_totalframe_pfn) {
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
    m_animation = skottie()->skottie_animation_from_data_pfn(const_cast<char *>(jsonBuffer.data()), jsonBuffer.length(), resDirPath.toUtf8().constData());
    if (!m_animation) {
        qWarning() << "Failed to create lottie animation.";
        return false;
    }
    m_source = value;
    skottie()->skottie_animation_get_size_pfn(m_animation, reinterpret_cast<size_t *>(&m_width), reinterpret_cast<size_t *>(&m_height));
    m_frameRate = skottie()->skottie_animation_get_framerate_pfn(m_animation);
    m_duration = skottie()->skottie_animation_get_duration_pfn(m_animation);
    m_totalFrame = skottie()->skottie_animation_get_totalframe_pfn(m_animation);
    // Clear previous status.
    m_currentFrame = 0;
    m_loopTimes = 0;
    m_shouldStop = false;
    Q_EMIT sourceChanged();
    Q_EMIT sizeChanged();
    Q_EMIT frameRateChanged();
    Q_EMIT durationChanged();
    Q_EMIT playingChanged();
    return true;
}

int QtLottieSkottieEngine::frameRate() const
{
    return qRound(m_frameRate);
}

int QtLottieSkottieEngine::duration() const
{
    return qRound(m_duration);
}

QSize QtLottieSkottieEngine::size() const
{
    return {static_cast<int>(m_width), static_cast<int>(m_height)};
}

int QtLottieSkottieEngine::loops() const
{
    return m_loops;
}

void QtLottieSkottieEngine::setLoops(const int value)
{
    if (m_loops != value) {
        m_loops = value;
        Q_EMIT loopsChanged();
        // Also clear previous status.
        m_loopTimes = 0;
        m_shouldStop = false;
        Q_EMIT playingChanged();
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

void QtLottieSkottieEngine::pause()
{

}

void QtLottieSkottieEngine::resume()
{

}
