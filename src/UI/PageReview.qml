import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0


Page {
    id: pageReview
    width: 960
    height:800
    Rectangle{
        anchors.fill: parent
        border.color: "gray"
        border.width: 1
    }

    Row{
        id:rowMain
        anchors.fill: parent
        anchors.topMargin: 1

        Column{
            id:colContent
            width: parent.width-colOperate.width
            height: parent.height
            Row{
                id:rowImageAndLabel
                width: parent.width
                height: parent.height*2/3
                Column{
                    id:colLabels
                    width: 200
                    height: parent.height
                    Label{
                        id:labelTitle
                        text:qsTr("predict result")
                        width: parent.width
                        height: 60
                        font.pixelSize: 20
                        font.bold: true
                        leftPadding: 20
                        topPadding: 20
                        bottomPadding: 0
                        background: Rectangle {
                            color: "#b1b3b5"
                        }

                    }
                    Rectangle{
                        id:rectListViewLabels
                        width: parent.width
                        height: parent.height-labelTitle.height
                        color: "lightyellow"
                        border.color: "black"
                        border.width: 2
                        ListView{
                            id:listViewLabels
                            anchors.fill: parent
                            spacing: 2

                            delegate:
                                Rectangle{
                                    x:2
                                    width: parent.width-4
                                    height: 40
                                    border.color: "gray"
                                    Row {
                                        anchors.fill: parent
                                          Text {
                                              text: "ID: " + ID
                                              font.pixelSize: 14
                                              width:parent.width/2
                                              height: parent.height
                                              verticalAlignment: Text.AlignVCenter
                                          }
                                          Text {
                                              text: "Label: $" + label
                                              font.pixelSize: 14
                                              width:parent.width/2
                                              height: parent.height
                                              verticalAlignment: Text.AlignVCenter
                                          }
                                }
                                }
                            model:ListModel{
                                id:listModelLabels
                                ListElement {
                                    ID: "0"
                                    label: 4
                                }
                                ListElement {
                                    ID: "1"
                                    label: 2
                                }
                                ListElement {
                                    ID: "2"
                                    label: 3
                                }
                            }
                        }

                    }
                }
                Rectangle {
                    id: rectImage
                    width: parent.width-listViewLabels.width
                    height: parent.height
                    CustomedImageCanvas{
                        id:testCanvas
                        type: 1
                        scollHide: false
                        buttonAndSliderHide:false
                        anchors.fill:parent;
                    }
                }
            }
            Rectangle {
                id: rectControlAndInfo
                width: parent.width
                height: parent.height/3


                Column {
                    anchors.fill: parent
                    padding: 20
                    Rectangle {
                        id:rectIndexControll
                        width: parent.width-parent.padding*2
                        height: 40
                        Row{
                        id: rowIndexControll
                        anchors.fill: parent
                            Button {
                                id: prevButton
                                width: parent.width/5
                                height: parent.height
                                text: qsTr("Last")
                                onClicked: {
                                    //ReviewController.previousImage()
                                }
                            }
                            TextInput{
                                id: currentIndexLabel
                                width: parent.width/5
                                height: parent.height
                                font.family: "Microsoft YaHei UI"
                                font.pointSize: 16
                                color: "#ee2c90"
                                text: "0"
                                horizontalAlignment: Text.AlignRight
                                selectByMouse: true
                                validator: IntValidator{
                                    bottom: -1

                                }
                                onAccepted:function (){
                                    //ReviewController.setPartCurrentIndex(parseInt(currentIndexLabel.text)-1,true);
                                }
                            }
                            Label{
                                id: totalLabel
                                width: parent.width/5
                                height: parent.height
                                font.family: "Microsoft YaHei UI"
                                font.pointSize: 16
                                text:"/"
                            }

                            Button {
                                id: nextButton
                                width: parent.width/5
                                height: parent.height
                                text: qsTr("Next")

                                onClicked: {
                                    //ReviewController.nextImage()
                                }
                            }

                        }
                    }


                    AOIInfoDisplayGrid{
                        id:infoDisplayGrid
                        width: parent.width-parent.padding*2
                        height: parent.height-rectIndexControll.height-parent.padding*2-2*parent.spacing
                    }


                }
            }

        }


        Column {

            id: colOperate
            width: 200
            height: parent.height
            Label {
                id: labelOperateTitle
                text: qsTr("Operations")
                font.pixelSize: 20
                font.bold: true
                width: parent.width
                height: 60
                leftPadding: 20
                topPadding: 20
                bottomPadding: 0
                background: Rectangle {
                    color: "#b1b3b5"
                }
            }

            Rectangle {
                id: rectOperateButtons
                width: parent.width
                height: parent.height-labelOperateTitle.height
                color: "white"
                border.color: "gray"

                Column{
                    anchors.fill: parent
                    spacing: 10
                    padding: 10
                    Repeater{
                        id:repeaterReviewFilters
                        model: 2
                        ComboBox{
                            x:10
                            width: 160
                            height: 40
                            displayText: qsTr("current:")+currentText
                            model: ["0"]
                        }
                    }


                }

            }
        }

    }
    Component.onCompleted: {
        //testCanvas.loadImage("file://./rest.jpg")
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
