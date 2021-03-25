#include "widget.h"
#include <qtlottiewidget.h>
#include <QtWidgets/qlayout.h>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    const auto lottieWidget = new QtLottieWidget(this);
    const auto l = new QVBoxLayout(this);
    l->addWidget(lottieWidget);
    setLayout(l);
    lottieWidget->setSource(QStringLiteral(":/lottie/46472-lurking-cat.json"));
}

Widget::~Widget() = default;
