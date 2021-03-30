import QtQuick 2.9
import QtQuick.Window 2.9
import QtQuick.Controls 2.9
import QtQuick.Layouts 1.9
import Qt.labs.platform 1.1
import wangwenx190.Utils 1.0

Window {
    id: window
    width: 800
    height: 600
    title: qsTr("QtLottie Quick Example")
    visible: true

    FileDialog {
        id: fileDialog
        nameFilters: [qsTr("Bodymovin files (*.json)"), qsTr("All files (*)")]
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
    }

    QtLottieItem {
        id: lottieItem
        anchors.fill: parent
        source: fileDialog.file
        onSourceSizeChanged: {
            window.width = lottieItem.sourceSize.width
            window.height = lottieItem.sourceSize.height
        }
    }

    RowLayout {
        anchors {
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
            bottom: parent.bottom
            bottomMargin: 10
        }
        height: 40
        spacing: 10

        Label {
            text: qsTr("Source")
            font.bold: true
        }

        TextField {
            id: inputBox
            placeholderText: qsTr("Files on the disk and embeded resources are all supported")
            text: fileDialog.file
            readOnly: true
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Browse")
            onClicked: fileDialog.open()
        }
    }
}
