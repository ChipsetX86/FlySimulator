import QtQuick 2.12
import QtQuick.Window 2.12
import QtQml 2.12
import QtQuick.Controls 2.12
import io.qt.backend 1.0

Window {
    id: mainWindow
    width: 600
    height: 600
    visible: true
    title: qsTr("Fly Simulator")

    TableView {
        id: mainTable
        anchors.fill: parent
        columnSpacing: 1
        rowSpacing: 1
        clip: false

        model: flySimulator.plotTableModel

        delegate: Rectangle {
            implicitWidth: (mainWindow.width - 100) / flySimulator.plotSize.width
            implicitHeight: (mainWindow.height - 100) / flySimulator.plotSize.height
            border.width: 2
            border.color: Qt.color("orange")

            Text {
                text: "Cell " + index
                anchors.centerIn: parent
            }

            Grid {
                padding: 5

                Repeater {
                    model: display
                    Row{
                        Image {
                            source: display[index].icon
                            width: 23
                            height: 23
                        }
                    }
                }

            }

            MouseArea {
                anchors.fill: parent
            }
        }
    }

    Button {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        id: buttonStop
        text: "Stop"
        onClicked: flySimulator.stopSimulation()
    }

    Component.onCompleted: flySimulator.startSimulation()
    //Component.onDestruction: flySimulator.stopSimulation()
}
