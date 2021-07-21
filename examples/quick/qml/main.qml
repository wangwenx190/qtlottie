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

import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.platform 1.0
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
