import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls 2.4

Page {
    id: pagePredict
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
                        color: "white"
                        border.color: "gray"
                        border.width: 1
                        ListView{
                            id:listViewLabels
                            anchors.fill: parent
                            spacing: 2

                            delegate:
                                Rectangle{
                                    x:1
                                    width: parent.width-2
                                    height: 40
                                    border.color: "gray"
                                    Row {
                                        anchors.fill: parent
                                          Text {
                                              text: qsTr("ID")+": " + modelData.ID
                                              font.pixelSize: 14
                                              width:parent.width/2
                                              height: parent.height
                                              verticalAlignment: Text.AlignVCenter
                                              MouseArea{
                                                  hoverEnabled: true
                                                  anchors.fill: parent
                                                  onEntered: {
                                                      ToolTip.text = "1111";
                                                      if(ToolTip.text.length != 0)
                                                          ToolTip.visible = true;
                                                      else
                                                          ToolTip.visible = false;
                                          }
                                                  onExited: {
                                                       ToolTip.visible = false;
                                                  }

                                              }
                                          }
                                          Text {
                                              id:labelText
                                              text: qsTr("Label")+": " + modelData.label
                                              font.pixelSize: 14
                                              width:parent.width/2
                                              height: parent.height
                                              verticalAlignment: Text.AlignVCenter
                                              MouseArea{
                                                  hoverEnabled: true
                                                  anchors.fill: parent
                                                  onEntered: {
                                                      ToolTip.text = labelText.text;

                                                      if(ToolTip.text.length != 0)
                                                          ToolTip.visible = true;
                                                      else
                                                          ToolTip.visible = false;
                                          }
                                                  onExited: {
                                                       ToolTip.visible = false;
                                }
                                }
                                }
                                }
                                }
                            model: []//ReviewController.getImagePredictInfo();
                            //model:[{ID:"1",label:"0"},{ID:"1",label:"0"}]
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
                                    ReviewController.previousImage();
                                }
                            }
                            TextInput{
                                id: currentIndexLabel
                                width: parent.width*3/2/5
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
                                width: parent.width*3/2/5
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
                                    ReviewController.nextImage();
                                    ReviewController.startPredict();
                                }
                            }

                        }
                    }

                    Rectangle{

                        width: parent.width-parent.padding*2
                        height: parent.height-rectIndexControll.height-parent.padding*2-2*parent.spacing

                        border.color: "gray"
                        AOIInfoDisplayGrid{
                            padding: 20
                            id:infoDisplayGrid
                            anchors.fill: parent
                        }
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


                    Switch{
                        id:autoPredicit
                        width: parent.width
                        height: 60
                        text: qsTr("auto predict")
                        onClicked: {
                            if(checked)
                                ReviewController.startPredict();
                        }
                    }
                    Switch{
                        text: qsTr("auto next")
                        width: parent.width
                        height: 60
                        onClicked: {
                            if(checked)
                            {
                                nextTimer.start();
                            }
                            else{
                                nextTimer.stop();
                            }
                        }
                    }


                    Label{
                        text: qsTr("current Image progress")
                        width: parent.width
                        height: 60
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    ProgressBar{
                        id:progressBarCurrentImagePredict
                        x:30
                        width: parent.width-2*30
                        height: 60
                        from:0
                        to:100
                        value: 30
                        background:
                            Rectangle {
                                  width: parent.width
                                  height: 6
                                  color: "#e6e6e6"
                                  radius: 3
                              }

                        contentItem:
                            Item{
                            width: parent.width
                            height: 6
                                Rectangle {
                                    width: progressBarCurrentImagePredict.visualPosition * parent.width
                                    height: 6
                                    radius: 2
                                    color: "#7cf7be"
                                }
                            }

                    }


                    Label{
                        id:currentUnpredict
                        text: "0"
                        width: parent.width
                        height: 60
                        font.pixelSize: 60
                        font.bold: true
                        font.family: "Heiti"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    Label{
                        text: qsTr("current unpredict")
                        width: parent.width
                        height: 60
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Label{
                        text: qsTr("Total Image progress")
                        width: parent.width
                        height: 60
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    ProgressBar{
                        id:progressBarTotalImagePredict
                        x:30
                        width: parent.width-2*30
                        height: 60
                        from:0
                        to:100
                        value: 80
                        background:
                            Rectangle {
                                  width: parent.width
                                  height: 6
                                  color: "#e6e6e6"
                                  radius: 3
                              }

                        contentItem:
                            Item{
                            width: parent.width
                            height: 6
                                Rectangle {
                                width: progressBarTotalImagePredict.visualPosition * parent.width
                                    height: 6
                                    radius: 2
                                    color: "#37cbec"
                                }
                            }
                    }
                    Button{
                        id:buttonOpenImage
                        text:"打开"
                        width: parent.width/2
                        height: 30
                        x:parent.width/4
                        background: Rectangle{
                            anchors.fill: parent
                            color: "orange"

                        }
                        onClicked: {
//                            fileDialog.mission="loadPic";
//                            fileDialog.open()
                            folderDialog.mission="loadPicFolder"
                            folderDialog.open()
                        }
                    }
                }
            }
        }

    }

    Timer{
        id:nextTimer
        interval: 1000
        repeat: true
        onTriggered: {
            ReviewController.nextImage();
            ReviewController.startPredict();
        }
    }

    Component.onCompleted: {
        //testCanvas.loadImage("./resource/firstImage.png")
        var path = "file:///"+"C:/Users/ABC3D/Desktop/0.jpg";
        //testCanvas.loadImage(path);
//        testCanvas.addMark("person",[[0.1,0.1],[0.7,0.1],[0.7,0.7],[0.1,0.7]],3,Qt.rgba(1,0,0,1),Qt.rgba(1,1,0,0.2))
        //testCanvas.myPaint();

        ReviewController.signalImageInfoUpdated.connect(updateInfo);
        ReviewController.updateImagePredicitInfo.connect(updateIDAndLabel);
        ReviewController.updateImagePredicitInfo.connect(function drawPolygon(){ updateImage(1) });
        updateInfo();
        //infoDisplayGrid.updateAOITitleValue([{role:"Time",title:"Time",value:"2020-07-20 00:00:00"}]);
        //infoDisplayGrid.updateAOITitleValue(ReviewController.getGridInfo());
        updateImage(1);

        testCanvas.myPaint();
        }

    function updateIDAndLabel(){

        listViewLabels.model = ReviewController.getImagePredictInfo();
    }

    function updateImage(draw_polygon){



        var result_current_json = ReviewController.getImagePredictInfo();


        testCanvas.clearMarkList();

        //console.log(result_current_json);

        testCanvas.loadImage(ReviewController.getCurrentImagePath());
        testCanvas.initDrawImageRateParamWithOriginSize()
        console.log("draw_polygon",result_current_json);
        if(draw_polygon === 1)
        {
            var wr=testCanvas.drawImageRateParam["dwr"],hr=testCanvas.drawImageRateParam["dhr"];
            for(var i=0; i<result_current_json.length; ++i)
            {
                var x1 = result_current_json[i]["x1"]*wr;
                var x2 = result_current_json[i]["x2"]*wr;
                var y1 = result_current_json[i]["y1"]*hr;
                var y2 = result_current_json[i]["y2"]*hr;
                var type =  result_current_json[i]["label"];
                var polygon = [[x1,y1],[x2,y1],[x2,y2],[x1,y2]];
                console.log("polygon",polygon);
                testCanvas.addMark(type,polygon,1,Qt.rgba(1,0,0,1),Qt.rgba(1,1,0,0.2));
    //            print(i,result_current_json[i]["label"],result_current_json[i]["x1"]);
    }

            //testCanvas.addMark("person",[[0.1,0.1],[0.2,0.1],[0.2,0.2],[0.1,0.2]],3,Qt.rgba(1,0,0,1),Qt.rgba(1,1,0,0.2));
            //testCanvas.addMark("bird",[[0.1,0.1],[0.7,0.1],[0.7,0.7],[0.1,0.7]],3,Qt.rgba(1,0,0,1),Qt.rgba(1,1,0,0.2));
        }
        testCanvas.myPaint();


//        if(draw_polygon === 1)
//            ReviewController.nextImage();
    }

    function updateInfo(){
//        currentIndexLabel.text = "";
//        totalLabel.text = "";
        var currenIndexAndTotal = ReviewController.getCurrentIndexAndTotal();
        currentIndexLabel.text=String(currenIndexAndTotal[0]);
        totalLabel.text="/"+currenIndexAndTotal[1];

        currentUnpredict.text = currenIndexAndTotal[1] - currenIndexAndTotal[0] > 0?currenIndexAndTotal[1] - currenIndexAndTotal[0] : 0 ;
        progressBarTotalImagePredict.value=currenIndexAndTotal[0]/currenIndexAndTotal[1]*100

        updateImage(1);

        var trans={"Time":"时间","imagepath":"图片地址"}
        //repeaterAOIInformation.model = [];
        var model=ReviewController.getGridInfo();
        for(var i=0; i<model.length;++i){
            if(trans[model[i]["role"]]){
                model[i]["title"]=trans[model[i]["role"]];
            }
            else{
                delete model[i];
            }
        }

        infoDisplayGrid.updateAOITitleValue(model);


    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
