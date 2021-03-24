#pragma once

#include <QtWidgets/qwidget.h>

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;
};
