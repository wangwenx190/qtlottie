import QtQuick 2.15
import QtQuick.Window 2.15
import wangwenx190.Utils 1.0

Window {
    width: 800
    height: 600
    title: qsTr("Hello")
    visible: true

    QtLottieItem {
        anchors.fill: parent
        source: "qrc:///lottie/43721-keywords.json"
    }
}
