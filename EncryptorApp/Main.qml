/************************************************
 * @file           : Main.qml
 * @author         : Ismail Amr
 * @brief          : GUI Application Main Window
 ************************************************/
import QtQuick
import QtQuick.Window
import EncryptorApp
import QtQuick.Dialogs
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    ToolBar {
            Button {
                text: qsTr("Choose Image...")
                onClicked: fileDialog.open()
            }
        }

    PassField {
        id: password
        anchors.centerIn: parent

    }

    Image {
        id: image
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
    }

    FileDialog {
        id: fileDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        onAccepted: image.source = selectedFile
    }
}
