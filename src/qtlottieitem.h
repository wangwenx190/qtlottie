#pragma once

#include "qtlottie_global.h"
#include <QtQuick/qquickpainteditem.h>

class QtLottieHelper;

class QTLOTTIE_API QtLottieItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QtLottieItem)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    QML_ELEMENT
#endif
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

public:
    explicit QtLottieItem(QQuickItem *parent = nullptr);
    ~QtLottieItem() override;

    void paint(QPainter *painter) override;
    Q_INVOKABLE void update2();

    QUrl source() const;
    void setSource(const QUrl &value);

Q_SIGNALS:
    void sourceChanged();

private:
    QScopedPointer<QtLottieHelper> m_lottieHelper;
    QUrl m_source = {};
};
