import QtQuick 2.15
import QtQml 2.15
import QtQuick.Controls 2.15
import io.qt.backend 1.0

ApplicationWindow {
    id: mainWindow
    width: 600
    height: 600
    visible: true
    title: qsTr("Fly Simulator")

    Component {
        id: simulation

        Item {
            id: items

            TableView {
                id: mainTable

                anchors.fill: parent
                columnSpacing: 1
                rowSpacing: 1
                clip: false

                model: flySimulator

                delegate: Rectangle {
                    implicitWidth: (stackView.width - 50) / 5
                    implicitHeight: (stackView.height - 50) / 5
                    border.width: 2
                    border.color: Qt.color("orange")

                    Grid {
                        padding: 5

                        Repeater {
                            model: display
                            Column {
                                Text {
                                    text: "s:" + Number(display[index].meanSpeedCellsInSec).toFixed(1).toString()
                                    visible: flySimulator.isStoped
                                }
                                Text {
                                    text: "age: " + Number(display[index].ageSec).toString();
                                    visible: flySimulator.isStoped
                                }
                                Image {
                                    source: display[index].icon
                                    width: 20
                                    height: 20
                                }
                            }
                        }

                    }
                }
            }

            Button {
                id: buttonStop
                text: "Stop"
                enabled: true
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
                onClicked: {
                    console.log("Click stop");
                    flySimulator.stopSimulation();
                    mainTable.forceLayout()

                }
            }

        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: Component {
            id: page
            Column {
                padding: 5
                spacing: 5

                Text {
                    text: "Width"
                }

                SpinBox {
                    id: widthSimulationTable
                    value: 5
                    stepSize: 1
                    anchors.left: parent.left
                    anchors.right: parent.right
                }

                Text {
                    text: "Height"
                }

                SpinBox {
                    id: heightSimulationTable
                    value: 5
                    stepSize: 1
                    anchors.left: parent.left
                    anchors.right: parent.right
                }

                Text {
                    text: "Time fly planing, sec"
                }

                SpinBox {
                    id: timeFlyPlaningimulation
                    value: 5
                    stepSize: 1
                    anchors.left: parent.left
                    anchors.right: parent.right
                }

                Button {
                    id: buttonStart
                    text: "Start"
                    onClicked: {
                        console.log("Click start");
                        flySimulator.flightPlanningTimeSec =
                                timeFlyPlaningimulation.value
                        flySimulator.plotSize =
                                Qt.size(widthSimulationTable.value,
                                        heightSimulationTable.value)
                        flySimulator.startSimulation()

                        stackView.push(simulation)
                    }

                    anchors.left: parent.left
                    anchors.right: parent.right
                }
            }
        }
    }

}
