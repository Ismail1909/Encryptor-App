/*****************************************************
 * @file           : passField.qml
 * @author         : Ismail Amr
 * @brief          : Custom Input Field for password
 *****************************************************/
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Password Item Component
Item {
    id: root

    width: 200; height: 100

    property bool accepted

    /* Function to Validate user input password against password requirements */
    function passwordValidation(passIn)
    {
        /* The Regular Expression used for Validation:
        At least 8 Characters long, 1 upper and lower case letter, 1 number and 1 special character */
        var regExp = /^(?=.*\d)(?=.*[a-z])(?=.*[A-Z]).{8,}$/;
        if(regExp.test(passIn))
        {
            /* If the password is valid */

            /* Hide Helperinfo */
            helperinfo.opacity = 0;

            /* Check if the password >= 12 to be strong */
            if(password.length >= 12)
            {
                /* Display Green color on all item components */
                label.color = "green";
                rectOkay.color = "green";
                rectStrong.color = "green";
                helper.text = "Strong";
            }
            else
            {
                /* Change text colors and fill 2 bars of the password strength indicator with Orange */
                label.color = "orange";
                rectOkay.color = "orange";
                rectStrong.color = "grey";
                helper.text = "! Okay";
            }
        }
        else
        {
            /* If the password is not valid */

            /*Display Helperinfo */
            helperinfo.opacity = 1;

            /* Change text colors and fill 1 bar of the password strength indicator with Red */
            label.color = "red";
            rectOkay.color = "grey"
            rectStrong.color = "grey"
            helper.text = "! Weak"

        }
    }

    ColumnLayout{
        spacing: 2

        /* Password field label */
        Text {
            id: label
            color: "grey"
            text: "Password"
            font.pixelSize: 10
            leftPadding: 5
            opacity: password.hovered ? 1 : 0.5
        }

        /* Password input field */
        TextField{
            id: password
            validator: RegularExpressionValidator { regularExpression: /^(?=.*\d)(?=.*[a-z])(?=.*[A-Z]).{8,}$/ }
            placeholderText: qsTr("Enter Password")
            hoverEnabled: true
            leftPadding : 5
            echoMode: TextInput.Password
            opacity: password.hovered ? 1 : 0.5

            background: Rectangle {
                    implicitWidth: 154
                    implicitHeight: 20
                    opacity: password.hovered ? 1 : 0.5
                    border.color: password.hovered ? label.color : "transparent"
                }
            /* Password field text editing handler */
            onTextEdited:
            {
                root.accepted = false
                if(length > 0)
                {
                    /* If there is input */

                    /* Increase Focus */
                    rectParent.opacity= 1

                    /* Validate input */
                    passwordValidation(text)
                }
                else
                {
                    /* If Empty */

                    /* Decrease Focus */
                    rectParent.opacity= 0.5
                    label.color = "grey"
                    rectOkay.color = "grey"
                    rectStrong.color = "grey"
                    helper.text = ""
                }
            }
            onAccepted: root.accepted = true
            onAcceptableInputChanged: root.accepted = false



        }

        /* Password Strength Indicator */
        RowLayout
        {
            id: rectParent
            opacity: password.hovered ? 1 : 0.2
            spacing: 2
            Rectangle {
            id: rectWeak
            height: 2
            width: 50
            color: label.color
            }
            Rectangle {
            id: rectOkay
            height: 2
            width: 50
            color: "grey"
            }
            Rectangle {
            id: rectStrong
            height: 2
            width: 50
            color: "grey"
            }

        }

        /* Password Strength Status */
        Text {
            id: helper
            color: label.color
            text: ""
            font.pixelSize: 10
            leftPadding: 5
        }

        /* Password Validation Rules */
        Text {
            id: helperinfo
            color: label.color
            opacity: 1
            text: "At least 8 Characters long, 1 upper and\nlower case letter, 1 number and\n1 special character"
            font.pixelSize: 10
            leftPadding: 5
        }

    }


}
