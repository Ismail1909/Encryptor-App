/************************************************
 * @file           : Main.qml
 * @author         : Ismail Amr
 * @brief          : GUI Application Main Window
 ************************************************/
import QtQuick
import QtQuick.Window
import EncryptorApp

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    PassField {
        id: password
        anchors.centerIn: parent

    }
}
