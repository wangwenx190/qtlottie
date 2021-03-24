#include "qtlottiewidget.h"
#include "qtlottiehelper.h"
#include <QtGui/qpainter.h>
#include <QtCore/qdebug.h>

QtLottieWidget::QtLottieWidget(QWidget *parent) : QWidget(parent)
{
    m_lottieHelper.reset(new QtLottieHelper(this));
}

QtLottieWidget::~QtLottieWidget() = default;

QString QtLottieWidget::filePath() const
{
    return m_filePath;
}

void QtLottieWidget::setFilePath(const QString &value)
{
    if (m_filePath != value) {
        m_filePath = value;
        if (!m_lottieHelper->start(m_filePath)) {
            qWarning() << "Failed to start playing.";
        }
        Q_EMIT filePathChanged();
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
