/************************************************
 * @file           : Main.qml
 * @author         : Ismail Amr
 * @brief          : GUI Application Main Window
 ************************************************/
import QtQuick
import QtQuick.Window
import EncryptorApp
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs


ApplicationWindow {
    visible: true
    title: qsTr("File Cipher")
    width: 400
    height: 300

    FileBrowser{
        id:browser
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 40

    }
    Text{
       id:fileinfo
       anchors.top: browser.bottom
       anchors.topMargin: 20
       anchors.left: parent.left
       anchors.leftMargin: 8
       text:{

           browser.path ? "File Extension: " + FileManager.getExtension(browser.path) + " File Size: " + FileManager.getSize(browser.path) + " Bytes" : " "
        }
    }

    PassField {
        id: password
        anchors.top: fileinfo.bottom
        anchors.topMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: 8

     }

    Button {
        id: encrypt
        width: 80
        height: 20
        anchors.top: password.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 8
        text: "Encrypt"
        enabled: (browser.path && password.accepted && FileManager.getExtension(browser.path) !== "aes") ? true : false
        onClicked: {
            FileManager.readFile(browser.path)

            FileManager.writeTempFile(FileManager.readFile(browser.path))
            save.enabled = true;
        }

       }
    Button {
        id: decrypt
        width: 80
        height: 20
        anchors.top: password.bottom
        anchors.topMargin: 10
        anchors.left: encrypt.right
        anchors.leftMargin: 10
        enabled: (browser.path && password.accepted && FileManager.getExtension(browser.path) === "aes")  ? true : false
        text: "Decrypt"

       }

    Button {
        id: save
        width: 80
        height: 20
        anchors.top: password.bottom
        anchors.topMargin: 10
        anchors.left: decrypt.right
        anchors.leftMargin: 10
        enabled: false
        text: "Save"
        onClicked:{
            saveDialog.open()
        }

       }

    FileDialog {
        id: saveDialog
        fileMode: FileDialog.SaveFile
        title: "Please choose a file"
        onAccepted: {
        var _path = selectedFile.toString();
        // remove prefixed "file:///"
        _path = _path.replace(/^(file:\/{3})/,"");
        FileManager.saveFile(_path);

    }
    }

}
