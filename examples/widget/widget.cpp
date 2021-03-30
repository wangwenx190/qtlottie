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
