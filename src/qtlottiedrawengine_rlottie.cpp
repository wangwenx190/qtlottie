#include "qtlottiedrawengine_rlottie.h"
#include <QtGui/qpainter.h>
#include <QtCore/qlibrary.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qvariant.h>
#include <QtCore/qcoreapplication.h>

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
using lottie_animation_get_duration_ptr = double(*)(const Lottie_Animation *animation);

class rlottie_data
{
    Q_DISABLE_COPY_MOVE(rlottie_data)

public:
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
        if (rlottieLib.isLoaded()) {
            qDebug() << "rlottie library already loaded. Unloading ...";
            if (!unload()) {
                return false;
            }
        }
        rlottieLib.setFileName(libName.isEmpty() ? getRLottieLibraryName() : libName);
        if (!rlottieLib.load()) {
            qWarning() << "Failed to load rlottie library:" << rlottieLib.errorString();
            return false;
        }
        qDebug() << "rlottie library loaded successfully from" << rlottieLib.fileName();

        lottie_animation_destroy_pfn = reinterpret_cast<lottie_animation_destroy_ptr>(rlottieLib.resolve("lottie_animation_destroy"));
        if (!lottie_animation_destroy_pfn) {
            qWarning() << "lottie_animation_destroy_pfn is null:" << rlottieLib.errorString();
        }

        lottie_animation_from_data_pfn = reinterpret_cast<lottie_animation_from_data_ptr>(rlottieLib.resolve("lottie_animation_from_data"));
        if (!lottie_animation_from_data_pfn) {
            qWarning() << "lottie_animation_from_data_pfn is null:" << rlottieLib.errorString();
        }

        lottie_animation_get_framerate_pfn = reinterpret_cast<lottie_animation_get_framerate_ptr>(rlottieLib.resolve("lottie_animation_get_framerate"));
        if (!lottie_animation_get_framerate_pfn) {
            qWarning() << "lottie_animation_get_framerate_pfn is null:" << rlottieLib.errorString();
        }

        lottie_animation_get_totalframe_pfn = reinterpret_cast<lottie_animation_get_totalframe_ptr>(rlottieLib.resolve("lottie_animation_get_totalframe"));
        if (!lottie_animation_get_totalframe_pfn) {
            qWarning() << "lottie_animation_get_totalframe_pfn is null:" << rlottieLib.errorString();
        }

        lottie_animation_render_pfn = reinterpret_cast<lottie_animation_render_ptr>(rlottieLib.resolve("lottie_animation_render"));
        if (!lottie_animation_render_pfn) {
            qWarning() << "lottie_animation_render_pfn is null:" << rlottieLib.errorString();
        }

        lottie_animation_get_size_pfn = reinterpret_cast<lottie_animation_get_size_ptr>(rlottieLib.resolve("lottie_animation_get_size"));
        if (!lottie_animation_get_size_pfn) {
            qWarning() << "lottie_animation_get_size_pfn is null:" << rlottieLib.errorString();
        }

        lottie_animation_get_duration_pfn = reinterpret_cast<lottie_animation_get_duration_ptr>(rlottieLib.resolve("lottie_animation_get_duration"));
        if (!lottie_animation_get_duration_pfn) {
            qWarning() << "lottie_animation_get_duration_pfn is null:" << rlottieLib.errorString();
        }

        return isLoaded();
    }

    [[nodiscard]] bool unload()
    {
        lottie_animation_destroy_pfn = nullptr;
        lottie_animation_from_data_pfn = nullptr;
        lottie_animation_get_framerate_pfn = nullptr;
        lottie_animation_get_totalframe_pfn = nullptr;
        lottie_animation_render_pfn = nullptr;
        lottie_animation_get_size_pfn = nullptr;
        lottie_animation_get_duration_pfn = nullptr;

        if (rlottieLib.isLoaded()) {
            if (!rlottieLib.unload()) {
                qWarning() << "Failed to unload rlottie library:" << rlottieLib.errorString();
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool isLoaded() const
    {
        return rlottieLib.isLoaded() && lottie_animation_destroy_pfn
                && lottie_animation_from_data_pfn && lottie_animation_get_framerate_pfn
                && lottie_animation_get_totalframe_pfn && lottie_animation_render_pfn
                && lottie_animation_get_size_pfn && lottie_animation_get_duration_pfn;
    }

private:
    QLibrary rlottieLib;
};

Q_GLOBAL_STATIC(rlottie_data, rlottie)

QtLottieRLottieEngine::QtLottieRLottieEngine(QObject *parent) : QtLottieDrawEngine(parent)
{
    if (!rlottie()->isLoaded()) {
        qWarning() << "rlottie not loaded.";
    }
}

QtLottieRLottieEngine::~QtLottieRLottieEngine()
{
    if (m_animation && rlottie()->lottie_animation_destroy_pfn) {
        rlottie()->lottie_animation_destroy_pfn(m_animation);
    }
}

bool QtLottieRLottieEngine::setSource(const QUrl &value)
{
    Q_ASSERT(rlottie()->lottie_animation_from_data_pfn);
    Q_ASSERT(rlottie()->lottie_animation_get_framerate_pfn);
    Q_ASSERT(rlottie()->lottie_animation_get_totalframe_pfn);
    Q_ASSERT(rlottie()->lottie_animation_get_size_pfn);
    Q_ASSERT(rlottie()->lottie_animation_get_duration_pfn);
    if (!rlottie()->lottie_animation_from_data_pfn || !rlottie()->lottie_animation_get_framerate_pfn
            || !rlottie()->lottie_animation_get_totalframe_pfn || !rlottie()->lottie_animation_get_size_pfn
            || !rlottie()->lottie_animation_get_duration_pfn) {
        qWarning() << "rlottie is not loaded.";
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
    // TODO: support qrc resources.
    const QString resDirPath = QCoreApplication::applicationDirPath();
    m_animation = rlottie()->lottie_animation_from_data_pfn(jsonBuffer.constData(), jsonBuffer.constData(), resDirPath.toUtf8().constData());
    if (!m_animation) {
        qWarning() << "Failed to create lottie animation.";
        return false;
    }
    m_source = value;
    Q_EMIT sourceChanged();
    rlottie()->lottie_animation_get_size_pfn(m_animation, &m_width, &m_height);
    Q_EMIT sizeChanged();
    m_frameRate = qRound(rlottie()->lottie_animation_get_framerate_pfn(m_animation));
    Q_EMIT frameRateChanged();
    m_duration = qRound(rlottie()->lottie_animation_get_duration_pfn(m_animation));
    Q_EMIT durationChanged();
    m_totalFrame = rlottie()->lottie_animation_get_totalframe_pfn(m_animation);
    m_frameBuffer.reset(new char[m_width * m_height * 32 / 8]);
    return true;
}

int QtLottieRLottieEngine::frameRate() const
{
    return m_frameRate;
}

int QtLottieRLottieEngine::duration() const
{
    return m_duration;
}

QSize QtLottieRLottieEngine::size() const
{
    return {static_cast<int>(m_width), static_cast<int>(m_height)};
}

int QtLottieRLottieEngine::loops() const
{
    return -1;
}

void QtLottieRLottieEngine::setLoops(const int value)
{
    Q_UNUSED(value);
}

bool QtLottieRLottieEngine::available() const
{
    return rlottie()->isLoaded();
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
    const bool needScale = (s != size());
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    // TODO: let the user be able to set the scale mode.
    // "Qt::SmoothTransformation" is a must otherwise the scaled image will become fuzzy.
    painter->drawImage(QPoint{0, 0}, needScale ? image.scaled(s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation) : image);
    painter->restore();
}

void QtLottieRLottieEngine::render(const QSize &s)
{
    Q_UNUSED(s);
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
    rlottie()->lottie_animation_render_pfn(m_animation, m_currentFrame, reinterpret_cast<uint32_t *>(m_frameBuffer.data()), m_width, m_height, m_width * 32 / 8);
    if (m_currentFrame >= m_totalFrame) {
        m_currentFrame = 0;
    } else {
        ++m_currentFrame;
    }
    m_hasFirstUpdate = true;
    Q_EMIT needRepaint();
}

void QtLottieRLottieEngine::release()
{
    // ref count
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
