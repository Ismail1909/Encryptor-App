/*****************************************************
 * @file           : fileBrowser.qml
 * @author         : Ismail Amr
 * @brief          : Input field for browsing file
 *****************************************************/
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

Item {

    id: root
    property alias path: path.text

    signal clicked

    TextField {
            id: path
            width: 250
            height: 20
            placeholderText: qsTr("Enter File")

    }

    Button {
            id: browser
            width: 80
            height: 20
            anchors.left: path.right
            anchors.leftMargin: 10
            text: "Browse..."
            onClicked: {
                fileDialog.open()
            }

           }

    FileDialog {
    id: fileDialog
    title: "Please choose a file"
    onAccepted: {
        var _path = selectedFile.toString();
        // remove prefixed "file:///"
        _path = _path.replace(/^(file:\/{3})/,"");
        path.text = _path
    }

    }



}
