#include <QtWidgets/qapplication.h>
#include "widget.h"

int main(int argc, char *argv[]) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_EnabledHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif
    QApplication application(argc, argv);
    Widget widget;
    widget.resize(800, 600);
    widget.show();
    return QApplication::exec();
}
