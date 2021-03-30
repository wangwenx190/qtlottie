#include "widget.h"
#include <qtlottiewidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qfiledialog.h>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle(tr("QtLottie Widget Example"));
    resize(800, 600);
    const auto lottieWidget = new QtLottieWidget(this);
    const auto lottieLayout = new QVBoxLayout(this);
    lottieLayout->setContentsMargins(0, 0, 0, 0);
    lottieLayout->setSpacing(0);
    lottieLayout->addWidget(lottieWidget);
    setLayout(lottieLayout);
    const auto label = new QLabel(this);
    label->setText(tr("Source"));
    const auto lineEdit = new QLineEdit(this);
    lineEdit->setReadOnly(true);
    lineEdit->setPlaceholderText(tr("Files on the disk and embeded resources are all supported"));
    const auto browseBtn = new QPushButton(this);
    browseBtn->setText(tr("Browse"));
    const auto bottomPanelLayout = new QHBoxLayout();
    bottomPanelLayout->addWidget(label);
    bottomPanelLayout->addWidget(lineEdit);
    bottomPanelLayout->addWidget(browseBtn);
    const auto layout2 = new QVBoxLayout(lottieWidget);
    layout2->addStretch();
    layout2->addLayout(bottomPanelLayout);
    lottieWidget->setLayout(layout2);
    connect(browseBtn, &QPushButton::clicked, this, [this, lineEdit, lottieWidget](){
        const QUrl url = QFileDialog::getOpenFileUrl(this, tr("Select a Bodymovin file"), {}, tr("Bodymovin files (*.json);;All files (*)"));
        if (url.isValid()) {
            lineEdit->setText(url.toString());
            lottieWidget->setSource(url);
        }
    });
    connect(lottieWidget, &QtLottieWidget::sourceSizeChanged, this, [this, lottieWidget](){
        resize(lottieWidget->sourceSize());
    });
}

Widget::~Widget() = default;
