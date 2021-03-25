#include "qtlottiewidget.h"
#include "qtlottiehelper.h"
#include <QtGui/qpainter.h>
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>

QtLottieWidget::QtLottieWidget(QWidget *parent) : QWidget(parent)
{
    m_lottieHelper.reset(new QtLottieHelper(this));
}

QtLottieWidget::~QtLottieWidget() = default;

QString QtLottieWidget::source() const
{
    return m_source;
}

void QtLottieWidget::setSource(const QString &value)
{
    if (m_source != value) {
        m_source = value;
        if (!m_lottieHelper->start(m_source, QCoreApplication::applicationDirPath())) {
            qWarning() << "Failed to start playing.";
        }
        Q_EMIT sourceChanged();
    }
}

void QtLottieWidget::paintEvent(QPaintEvent *event)
{
    Q_ASSERT(!m_lottieHelper.isNull());
    if (m_lottieHelper.isNull()) {
        return;
    }
    QPainter painter(this);
    m_lottieHelper->paint(&painter);
    QWidget::paintEvent(event);
}
