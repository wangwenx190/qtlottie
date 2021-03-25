#include "qtlottieitem.h"
#include "qtlottiehelper.h"
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>

QtLottieItem::QtLottieItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    m_lottieHelper.reset(new QtLottieHelper(this));
}

QtLottieItem::~QtLottieItem() = default;

void QtLottieItem::paint(QPainter *painter)
{
    Q_ASSERT(!m_lottieHelper.isNull());
    if (m_lottieHelper.isNull()) {
        return;
    }
    Q_ASSERT(painter);
    if (!painter) {
        return;
    }
    m_lottieHelper->paint(painter);
}

void QtLottieItem::update2()
{
    update();
}

QUrl QtLottieItem::source() const
{
    return m_source;
}

void QtLottieItem::setSource(const QUrl &value)
{
    if (!value.isValid()) {
        qWarning() << value << "is not valid.";
        return;
    }
    if (m_source != value) {
        m_source = value;
        QString path = {};
        if (value.scheme() == QStringLiteral("qrc")) {
            path = m_source.toString();
            // QFile can't recognize url.
            path.replace(QStringLiteral("qrc:"), QStringLiteral(":"), Qt::CaseInsensitive);
            path.replace(QStringLiteral(":///"), QStringLiteral(":/"));
        } else {
            path = m_source.isLocalFile() ? m_source.toLocalFile() : m_source.url();
        }
        if (!m_lottieHelper->start(path, QCoreApplication::applicationDirPath())) {
            qWarning() << "Failed to start playing.";
        }
        Q_EMIT sourceChanged();
    }
}
