#include "qtlottiewidget.h"
#include "qtlottiehelper.h"
#include <QtGui/qpainter.h>

QtLottieWidget::QtLottieWidget(QWidget *parent) : QWidget(parent)
{
    m_lottieHelper.reset(new QtLottieHelper(this));
    //m_lottieHelper->start(QStringLiteral(""), {});
}

QtLottieWidget::~QtLottieWidget() = default;

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
