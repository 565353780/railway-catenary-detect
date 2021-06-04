import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls 2.4



Page {

//    property var background_color_0: Qt.rgba(0,0,0,0)//"#132463"
//    property var background_color_1: "#5472de"
//    property var background_color_2: "#94a6ea"
//    property var text_color: "#FFFFFF"
//    property var border_color: Qt.rgba(21/255,66/255,93/255,1)//"#3BBEE3"
//    property int default_border_width: 2;
    property var button_color: "#7e4d96"
    property var button_hover_color: "#364a91"
    property  var label_list;
    property var colorList: ["yellow","lightblue","green","organge","purple","pink"];
    property int leftShrinkStatus: 0;
    property int rightShrinkStatus: 0;
    RectBackground0 {}

    id: pagePredict
    width: 960
    height:800
    focus: true
    onWidthChanged: {
        updateInfo();
    }

    Keys.onPressed: {
        if(event.key === Qt.Key_Z){
             testCanvas.switchEditMode(true);
        }
        if(event.key === Qt.Key_X)
        {
            testCanvas.switchEditMode(false);
        }
    }


    Row{
        id:rowMain
        anchors.fill: parent
        anchors.topMargin: 1

        Rectangle {
            id: operateFunctionalArea
            visible: true
            width: 200
            height: parent.height
            border.width: default_border_width
            border.color: border_color
            color: background_color_0

            Rectangle {
                id: operateArea
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 1
                anchors.topMargin: 1
                color: background_color_0
                width: parent.width
                height: 140

                //visible: parent.width == 1 ? true : false

                Rectangle {
                    id: operationslabel
                    anchors.topMargin: 1
                    width: parent.width - 1 - operateArea.anchors.leftMargin
                    height: 60
                    color: background_color_1
                    Text {
                        id: labelOperateTitle
                        anchors.fill: parent
                        text: qsTr("Operations")
                        font.pixelSize: 20
                        font.bold: true
                        color: text_color
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        }
                }

                Switch{
                    id:autoPredicit
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    anchors.topMargin: operationslabel.height + operationslabel.anchors.topMargin + 20
                    width: parent.width - anchors.leftMargin
                    height: 50
                    text: qsTr("auto predict")
                    onClicked: {
                        if(checked)
                        {
                            var target = true;
                            ReviewController_Predict.setPredictController(target);
                            ReviewController_Predict.startPredict();
                            nextTimer.start();
                        }
                        else
                        {
                            target = false;
                            ReviewController_Predict.setPredictController(target);
                            nextTimer.stop();
                        }
                    }
                    contentItem: Text {
                        text: autoPredicit.text
                        font: autoPredicit.font
                        color: text_color
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: 65
                    }

                }

                Switch{
                    id: autoNextSwitch
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    anchors.topMargin: autoPredicit.height + autoPredicit.anchors.topMargin
                    text: qsTr("auto next")
                    width: parent.width - anchors.leftMargin
                    height: 50
                    onClicked: {
                        if(checked)
                        {
                            nextTimer.start();
                        }
                        else{
                            nextTimer.stop();
                        }
                    }
                    contentItem: Text {
                        text: autoNextSwitch.text
                        font: autoNextSwitch.font
                        color: text_color
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: 65
                    }
                }

            }

            Rectangle {
                anchors.top: operateArea.bottom
                anchors.left: parent.left
                anchors.leftMargin: 1
                anchors.topMargin: operateArea.height + operateArea.anchors.topMargin
                color: background_color_0
                width: parent.width - anchors.leftMargin
                height: parent.height - operateArea.height

                Rectangle {
                    id: processArea
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 1
                    width: parent.width - parent.anchors.leftMargin
                    height: 60
                    color: background_color_1
                    Text {
                        anchors.fill: parent
                        text: qsTr("Process")
                        font.pixelSize: 20
                        font.bold: true
                        color: text_color
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Column {
                    id: colOperate
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 15
                    anchors.topMargin: processArea.height + processArea.anchors.topMargin + 30
                    width: parent.width - anchors.leftMargin
                    height: parent.height
                    spacing: 10

                    Rectangle {
                        width: parent.width - parent.anchors.leftMargin
                        height: 100
                        border.width: default_border_width
                        border.color: border_color
                        color: background_color_0
                        Column {
                            width: parent.width
                            height: parent.height
                            spacing: 10
                            Label{
                                text: qsTr("current Image progress")
                                width: parent.width
                                height: 40
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                color: text_color
                                background: Rectangle {
                                    color: background_color_2
                                }
                            }

                            ProgressBar{
                                id:progressBarCurrentImagePredict
                                anchors.left: parent.left
                                anchors.leftMargin: 20
                                width: parent.width - 2 * anchors.leftMargin
                                height: 60
                                from:0
                                to:100
                                value: 0
                                topPadding: progressBarCurrentImagePredict.height / 2 - 3
                                contentItem: Item{
                                    width: parent.width
                                    height: 6
                                    Rectangle {
                                        width: progressBarCurrentImagePredict.visualPosition * parent.width
                                        height: 6
                                        radius: 1
                                        color: "#27f294"
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        width: parent.width - parent.anchors.leftMargin
                        height: 200
                        border.width: default_border_width
                        border.color: border_color
                        color: background_color_0

                        Column {
                            width: parent.width
                            height: parent.height
                            spacing: 10
                            Label{
                                text: qsTr("current unpredict")
                                width: parent.width
                                height: 40
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                color: text_color
                                background: Rectangle {
                                    color: background_color_2
                                }
                            }
                            Label{
                                id:currentUnpredict
                                text: "0"
                                width: parent.width
                                height: 40
                                font.pixelSize: 40
                                font.bold: true
                                font.family: "Heiti"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                color: text_color
                                background: Rectangle {
                                    color: background_color_0
                                }
                            }
                            Label{
                                text: qsTr("Total Image progress")
                                width: parent.width
                                height: 40
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                color: text_color
                                background: Rectangle {
                                    color: background_color_2
                                }
                            }
                            ProgressBar{
                                id:progressBarTotalImagePredict
                                anchors.left: parent.left
                                anchors.leftMargin: 20
                                width: parent.width - 2 * anchors.leftMargin
                                height: 40
                                from:0
                                to:100
                                value: 0
                                topPadding: progressBarTotalImagePredict.height / 2 - 3
                                contentItem: Item{
                                    width: parent.width
                                    height: 6
                                    Rectangle {
                                        width: progressBarTotalImagePredict.visualPosition * parent.width
                                        height: 6
                                        radius: 1
                                        color: "#27f294"
                                    }
                                }
                            }
                        }
                    }

                    Button{
                        id:buttonOpenImage
                        width: parent.width - parent.anchors.leftMargin
                        height: 40
                        //x:parent.width/4
                        contentItem: Text {
                            text:"打开"
                            color: text_color
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            anchors.fill: parent
                            color: (parent.hovered|parent.pressed)?button_hover_color:button_color
                            radius: 20
                        }
                        onClicked: {
                            folderDialog.mission="loadPicFolder"
                            folderDialog.open()
                        }
                    }

                    Button{
                        width: parent.width
                        height: 100
                        visible: false
                        onClicked: {

                        }
                    }

                }
            }
        }

        Rectangle{
            id:colContent
            width: parent.width - colOperate.width - predictArea.width - 15
            height: parent.height
            color: background_color_0

            // left shrink btn
            Rectangle {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                color: background_color_0
                width: 20
                height: 40


                Image {
                    id: leftShrink
                    anchors.centerIn: parent
                    source: "./resource/left_01.png"
                    fillMode: Image.PreserveAspectFit
                    opacity: 1

                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true;
                    onEntered:  {
                        if (leftShrinkStatus == 0) {
                            leftShrink.source = "./resource/left_02.png"
                        } else {
                            leftShrink.source = "./resource/left_12.png"
                    }
                    }
                    onExited: {
                        if (leftShrinkStatus == 0) {
                            leftShrink.source = "./resource/left_01.png"
                        } else {
                            leftShrink.source = "./resource/left_11.png"
                    }
                    }
                    onPressed: {
                        if (leftShrinkStatus == 0) {
                            leftShrinkStatus = 1
                            leftShrink.source = "./resource/left_11.png"
                            operateFunctionalArea.visible = false
                            operateFunctionalArea.width = 0

                        } else {
                            leftShrinkStatus = 0
                            leftShrink.source = "./resource/left_01.png"
                            operateFunctionalArea.visible = true
                            operateFunctionalArea.width = 200

                        }
                    }
                }

            }

            // right shrink btn
            Rectangle {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                color: background_color_0
                width: 20
                height: 40

                Image {
                    id: rightShrink
                    anchors.centerIn: parent
                    source: "./resource/right_01.png"
                    fillMode: Image.PreserveAspectFit
                    opacity: 1
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true;
                    onEntered:  {
                        if (rightShrinkStatus == 0) {
                            rightShrink.source = "./resource/right_02.png"
                        } else {
                            rightShrink.source = "./resource/right_12.png"
                    }
                    }
                    onExited: {
                        if (rightShrinkStatus == 0) {
                            rightShrink.source = "./resource/right_01.png"
                        } else {
                            rightShrink.source = "./resource/right_11.png"
                    }
                    }
                    onPressed: {
                        if (rightShrinkStatus == 0) {
                            rightShrinkStatus = 1
                            rightShrink.source = "./resource/right_11.png"
                            predictArea.visible = false
                            predictArea.width = 0

                        } else {
                            rightShrinkStatus = 0
                            rightShrink.source = "./resource/right_01.png"
                            predictArea.visible = true
                            predictArea.width = 200

                        }
                    }
                }

            }

            Rectangle {
                anchors.centerIn: parent
                width: parent.width - 40
                height: parent.height - 20
                color: background_color_0

                Column {
                    width: parent.width
                    height: parent.height
                    spacing: 20

                    Rectangle {
                        id: rectImage
                        width: parent.width
                        height: parent.height * 2 / 3
                        color: background_color_0
                        CustomedImageCanvas{
                            id:testCanvas
                            anchors.centerIn: parent
                            //type: 1
                            //scollHide: false
                            //buttonAndSliderHide:false
                            anchors.fill:parent;
                        }
                    }

                    Rectangle {
                        id: rectControlAndInfo
                        width: parent.width
                        height: parent.height/3
                        color: background_color_0

                        Column {
                            anchors.fill: parent
                            spacing: 20

                            Rectangle {
                                id:rectIndexControll
                                width: parent.width
                                height: 40
                                color: background_color_0
                                Row{
                                    id: rowIndexControll
                                    anchors.fill: parent
                                    Button {
                                        id: prevButton
                                        width: parent.width/5
                                        height: parent.height

                                        contentItem: Text {
                                        text: qsTr("Last")
                                            color: text_color
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        background: Rectangle {
                                            //border.width : 1
                                            //border.color : border_color
                                            color: (parent.hovered|parent.pressed)?button_hover_color:button_color
                                            radius: 20
                                        }
                                        onClicked: {
                                            listViewLabels.model = [];
                                            ReviewController_Predict.previousImage();

                                        }
                                    }
                                    TextInput{
                                        id: currentIndexLabel
                                        width: parent.width*3/2/5
                                        height: parent.height
                                        font.family: "Microsoft YaHei UI"
                                        font.pointSize: 16
                                        color: text_color
                                        text: "0"
                                        horizontalAlignment: Text.AlignRight
                                        selectByMouse: true
                                        validator: IntValidator{
                                            bottom: -1

                                        }
                                        onAccepted:function (){
                                            //ReviewController_Predict.setPartCurrentIndex(parseInt(currentIndexLabel.text)-1,true);
                                        }
                                    }
                                    Label{
                                        id: totalLabel
                                        width: parent.width*3/2/5
                                        height: parent.height
                                        font.family: "Microsoft YaHei UI"
                                        font.pointSize: 16
                                        text:"/"
                                        color: text_color

                                    }

                                    Button {
                                        id: nextButton
                                        width: parent.width/5
                                        height: parent.height

                                        contentItem: Text {
                                        text: qsTr("Next")
                                            color: text_color
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        background: Rectangle {
                                            //border.width : 1
                                            //border.color : border_color
                                            color: (parent.hovered|parent.pressed)?button_hover_color:button_color
                                            radius: 20
                                        }

                                        onClicked: {
                                            listViewLabels.model = [];
                                            ReviewController_Predict.nextImage();
                                        }
                                    }

                                }
                            }

                            Rectangle{
                                width: parent.width
                                height: parent.height/2-20
                                color: background_color_0
                                AOIInfoDisplayGrid{
                                    id:infoFirstDisplayGrid
                                    anchors.fill: parent
                                }
                            }

                            Rectangle{
                                width: parent.width
                                height: parent.height/2-20
                                color: background_color_0

                                AOIInfoDisplayGrid{
                                    id:infoSecondDisplayGrid
                                    anchors.fill: parent
                                }

                            }
                        }
                    }
                }


            }

        }

        Rectangle {
            id: predictArea
            visible: true
            width: 200
            height: parent.height
            border.width : default_border_width
            border.color : border_color
            color: background_color_0

            Rectangle{
                id:labelTitle
                width: parent.width
                anchors.top: parent.top
                anchors.topMargin: 1
                height: 60
                color: background_color_1
                Text {
                    id: labelAttributeSelect
                    anchors.fill: parent
                    text:qsTr("predict result")
                    font.pixelSize: 20
                    font.bold: true
                    color: text_color
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle{
                id:rectListViewLabels
                width: parent.width
                height: parent.height-labelTitle.height
                anchors.top: parent.top
                anchors.topMargin: labelTitle.height + labelTitle.anchors.topMargin + 10
                color: background_color_0
                ListView{
                    id:listViewLabels
                    anchors.fill: parent
                    spacing: 10

                    delegate:
                        Rectangle{
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            width: parent.width - 2 * anchors.leftMargin
                            height: 50
                            border.width : default_border_width
                            border.color : border_color
                            color: background_color_0
                            Column {
                                anchors.fill: parent
                                Text {
                                  anchors.left: parent.left
                                  anchors.leftMargin: 5
                                  text: qsTr("ID")+": " + modelData.ID
                                  font.pixelSize: 14
                                  color: text_color
                                  width:parent.width
                                  height: parent.height/2
                                  verticalAlignment: Text.AlignVCenter
                                }
                                Text {
                                  anchors.left: parent.left
                                  anchors.leftMargin: 5
                                  id:labelText
                                  text: qsTr("Label")+": " + label_list[modelData.Label]
                                  font.pixelSize: 14
                                  color: text_color
                                  width:parent.width
                                  height: parent.height/2
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
                    model: []
                }
            }

        }



    }

    Timer{
        id:nextTimer
        interval: 10
        repeat: true
        onTriggered: {
            updateCurrentProgressValue();
        }
    }

    Component.onCompleted: {
        label_list = ReviewController_Review.getLabelList();
        console.log("label_list in qml:",label_list);
        ReviewController_Predict.updateImagePath(-1);
        initInformation();
        updateInfo();
        currentUnpredict.text = ReviewController_Predict.getUnpredictImageCount();
        ReviewController_Predict.currentImageFinished.connect(updateUnpredicitProgressValue);
        ReviewController_Predict.signalImageInfoUpdated.connect(updateInfo);
        ReviewController_Predict.updateImagePredicitInfo.connect(updateIDAndLabel);
        ReviewController_Predict.initInformation.connect(initInformation);
        ReviewController_Predict.updateImagePredicitInfo.connect(updateInfo);
    }

    function initInformation(){
        var currenIndexAndTotal = ReviewController_Predict.getCurrentIndexAndTotal();
        currentIndexLabel.text = String(currenIndexAndTotal[0]);
        totalLabel.text = "/"+currenIndexAndTotal[1];
        currentUnpredict.text = ReviewController_Predict.getUnpredictImageCount();
        var predict_total = ReviewController_Predict.getNeedPredictCountTotal();
        if(ReviewController_Predict.getUnpredictImageCount() === 0)
            progressBarTotalImagePredict.value = 100;
        else
            progressBarTotalImagePredict.value = ( predict_total-ReviewController_Predict.getUnpredictImageCount() )/predict_total * 100;
        progressBarCurrentImagePredict.value = 0;
    }

    function updateCurrentProgressValue(){
        if(progressBarCurrentImagePredict.value < 90)
            progressBarCurrentImagePredict.value += 10;
    }

    function updateUnpredicitProgressValue(){
        currentUnpredict.text = ReviewController_Predict.getUnpredictImageCount();
        var predict_total = ReviewController_Predict.getNeedPredictCountTotal();
        progressBarTotalImagePredict.value = ( predict_total-ReviewController_Predict.getUnpredictImageCount() )/predict_total * 100;
        progressBarCurrentImagePredict.value = 100;
        if(autoNextSwitch.checked){
            ReviewController_Predict.autoNextController();
        }
    }

    function updateIDAndLabel(){
        listViewLabels.model = [];
        listViewLabels.model = ReviewController_Predict.getImagePredictInfo();

    }

    function updateImage(draw_polygon){
        var result_current_json = ReviewController_Predict.getImagePredictInfo();
        testCanvas.clearMarkList();
        testCanvas.loadImage(ReviewController_Predict.getCurrentImagePath());
        //testCanvas.initDrawImageRateParamWithOriginSize()
        if(draw_polygon === 1)
        {
            var label_list = ReviewController_Predict.getLabelList();
            for(var i=0; i<result_current_json.length; ++i)
            {
                var x1 = result_current_json[i]["x1"];
                var x2 = result_current_json[i]["x2"];
                var y1 = result_current_json[i]["y1"];
                var y2 = result_current_json[i]["y2"];
                var type =  Number(result_current_json[i]["Label"]);
                var label = label_list[type];
                var ID = result_current_json[i]["ID"];
                testCanvas.addMark(ID,label,[[x1,y1],[x2,y1],[x2,y2],[x1,y2]],2,colorList[type],colorList[type],1);
            }
        }
        testCanvas.myPaint();
    }

    function updateInfo(){
        var currenIndexAndTotal = ReviewController_Predict.getCurrentIndexAndTotal();
        currentIndexLabel.text=String(currenIndexAndTotal[0]);
        totalLabel.text="/"+currenIndexAndTotal[1];
        var info_list=ReviewController_Review.getFirstGridInfo();
        var is_predict_value_list=["是","否"];
        var predict_result_list=["---","正常","异常"];
        for(var i=0; i<info_list.length; ++i){
//            print(info_list[i].role,info_list[i].title)
            if(info_list[i].role==="IsPredicted"){
                info_list[i].value=is_predict_value_list[info_list[i].value];
            }
            if(info_list[i].role==="PredictResult"){
                info_list[i].value= predict_result_list[info_list[i].value];
            }
        }

        infoFirstDisplayGrid.updateAOITitleValue(info_list);
        infoSecondDisplayGrid.updateAOITitleValue(ReviewController_Predict.getSecondGridInfo());
        updateIDAndLabel();
        updateImage(1);
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
