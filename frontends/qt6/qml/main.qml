import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    width: 1200
    height: 800
    title: "WizardMerge - Intelligent Merge Conflict Resolution"

    // Properties exposed from C++
    property string backendUrl: ""
    property bool standalone: false
    property string initialFile: ""

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: "WizardMerge"
                font.pixelSize: 18
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            Label {
                text: standalone ? "Standalone Mode" : "Client Mode"
                font.pixelSize: 12
            }

            ToolButton {
                text: "Open File"
                onClicked: fileDialog.open()
            }

            ToolButton {
                text: "Settings"
                onClicked: settingsDialog.open()
            }
        }
    }

    // Main content area
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        // Left panel - Base version
        Rectangle {
            SplitView.preferredWidth: parent.width / 3
            color: "#f5f5f5"
            border.color: "#cccccc"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                Label {
                    text: "Base Version"
                    font.bold: true
                    font.pixelSize: 14
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    TextArea {
                        id: baseText
                        readOnly: true
                        font.family: "monospace"
                        font.pixelSize: 12
                        wrapMode: TextEdit.NoWrap
                        placeholderText: "Base version will appear here..."
                    }
                }
            }
        }

        // Middle panel - Ours version
        Rectangle {
            SplitView.preferredWidth: parent.width / 3
            color: "#e8f5e9"
            border.color: "#4caf50"
            border.width: 2

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                Label {
                    text: "Ours (Current Branch)"
                    font.bold: true
                    font.pixelSize: 14
                    color: "#2e7d32"
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    TextArea {
                        id: oursText
                        readOnly: true
                        font.family: "monospace"
                        font.pixelSize: 12
                        wrapMode: TextEdit.NoWrap
                        placeholderText: "Our version will appear here..."
                    }
                }
            }
        }

        // Right panel - Theirs version
        Rectangle {
            SplitView.preferredWidth: parent.width / 3
            color: "#e3f2fd"
            border.color: "#2196f3"
            border.width: 2

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                Label {
                    text: "Theirs (Incoming Branch)"
                    font.bold: true
                    font.pixelSize: 14
                    color: "#1565c0"
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    TextArea {
                        id: theirsText
                        readOnly: true
                        font.family: "monospace"
                        font.pixelSize: 12
                        wrapMode: TextEdit.NoWrap
                        placeholderText: "Their version will appear here..."
                    }
                }
            }
        }
    }

    // Status bar
    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: "Backend: " + backendUrl
                font.pixelSize: 10
            }

            Item { Layout.fillWidth: true }

            Label {
                text: initialFile !== "" ? "File: " + initialFile : "No file loaded"
                font.pixelSize: 10
            }

            Label {
                text: "Ready"
                font.pixelSize: 10
                font.bold: true
            }
        }
    }

    // File dialog (placeholder)
    Dialog {
        id: fileDialog
        title: "Open File"
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        Label {
            text: "File selection not yet implemented.\nUse command line: wizardmerge-qt6 <file>"
        }
    }

    // Settings dialog (placeholder)
    Dialog {
        id: settingsDialog
        title: "Settings"
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        ColumnLayout {
            spacing: 10

            Label {
                text: "Backend URL:"
            }

            TextField {
                Layout.fillWidth: true
                text: backendUrl
                placeholderText: "http://localhost:8080"
            }

            CheckBox {
                text: "Standalone Mode"
                checked: standalone
            }
        }
    }

    // Component initialization
    Component.onCompleted: {
        console.log("WizardMerge Qt6 UI initialized")
        console.log("Backend URL:", backendUrl)
        console.log("Standalone:", standalone)
        console.log("Initial File:", initialFile)
    }
}
