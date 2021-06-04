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
    property int leftShrinkStatus: 0
    property int rightShrinkStatus: 0
    property var picId;
    property var time;
    property var label_list;
    property var colorList: ["yellow","lightblue","green","organge","purple","pink"];

    RectBackground0 {}

    id: pagePredict
    width: 960
    height:800
    focus: true


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
            id: colOperate

            color: background_color_0
            width: 200
            height: parent.height
            border.width: default_border_width
            border.color: border_color

            Rectangle {
                id: filters
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 1
                anchors.topMargin: 1
                color: background_color_0
                width: parent.width - anchors.leftMargin
                height: 260

                Column{
                    width: parent.width
                    height: parent.height
                    spacing: 15
                    Label {
                        text: qsTr("filters")
                        font.pixelSize: 20
                        font.bold: true
                        width: parent.width
                        height: 60
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: text_color
                        background: Rectangle {
//                            border.width : 1
//                            border.color : border_color
                            color: background_color_1
                        }
                    }
                    ComboBox{
                        id:isPredictedCombo
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        width: parent.width - 2 * anchors.leftMargin
                        height: 40

                        model: ["---","未预测","已预测"]

                        contentItem: Text {
                            text: parent.currentText
                            color: text_color
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: background_color_2
                        }


                        onCurrentTextChanged: {
                            ReviewController_Review.selectImageShowStatus(currentIndex);
                            updateInfo();
                        }
                    }

                    ComboBox{
                        id: predictResultCombo
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        width: parent.width - 2 * anchors.leftMargin
                        height: 40
                        currentIndex: 3
//                        model: ["NONE", "Unpredicted", "Normal", "Abnormal"]
                        model: ["----", "未预测","正常", "异常"]
                        contentItem: Text {
                            text: parent.currentText
                            color: text_color
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: background_color_2
                        }

                        onCurrentTextChanged: {
                            updateInfo();
                        }
                    }
                }

            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.leftMargin: 1
                color: background_color_0
                width: parent.width - anchors.leftMargin
                height: parent.height - filters.height - filters.anchors.topMargin

                Column {
                    id:col
                    width: parent.width
                    height: parent.height
                    spacing: 20

                    Label {
                        id: labelOperateTitle
                        text: qsTr("Operations")
                        font.pixelSize: 20
                        font.bold: true
                        width: parent.width
                        height: 60
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: text_color
                        background: Rectangle {
//                            border.width : 1
//                            border.color : border_color
                            color: background_color_1
                        }
                    }

                    Column {
                        spacing: 20
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        width: parent.width - 2 * anchors.leftMargin
                        height: parent.height - labelOperateTitle.height

                        Switch{
                            id:switchAutoLoadPredict
                            width: parent.width
                            height: 50
                            text: qsTr("auto load predict")
                            contentItem: Text {
                                text: switchAutoLoadPredict.text
                                font: switchAutoLoadPredict.font
                                color: text_color
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 65
                            }
                            onClicked: {
                                if(checked)
                                {
                                    ReviewController_Review.setLoadPredictResultController(true);
                                    updateInfo();
                                }
                                else
                                {
                                    ReviewController_Review.setLoadPredictResultController(false);
                                }
                            }
                        }

                        Button{
                            id:buttonLoadPredictToReview
                            width: parent.width
                            height: 40
                            contentItem: Text {
                                text: qsTr("Load predict result")
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
                                ReviewController_Review.setLoadPredictResultController(true);
                                updateIDAndLabel();
                                updateImage(1);
                            }
                        }

                        Switch{
                            id:switchAddMark
                            width: parent.width
                            height: 50
                            text: checked?qsTr("turn off add mark"):qsTr("turn on add mark")
                            contentItem: Text {
                                text: switchAddMark.text
                                font: switchAddMark.font
                                color: text_color
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 65
                            }

                            onCheckedChanged: {
                                testCanvas.switchEditMode(switchAddMark.checked);
                            }
                        }

                        Button{
                            id:buttonDeleteCurrentMark
                            width: parent.width
                            height: 40
                            contentItem: Text {
                                text: qsTr("delete current")
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
                                if(testCanvas.currentMarkIndex > -1 && testCanvas.currentMarkIndex < testCanvas.markList.length){
                                ReviewController_Review.deleteMark(testCanvas.currentMarkIndex);
                                    updateIDAndLabel();
                                    updateImage(1);

                            }
                            }

                        }

                        Button{
                            id:buttonLastMark
                            width: parent.width
                            height: 40
                            contentItem: Text {
                                text: qsTr("select last mark")
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
                                testCanvas.lastMark();
                                //updateImage(1);
                            }
                        }

                        Button{
                            id:buttonNextMark
                            width: parent.width
                            height: 40
                            contentItem: Text {
                                text: qsTr("select next mark")
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
                                testCanvas.nextMark();
                                //updateImage(1);
                            }
                        }

                        Row {
                            width: parent.width
                            height: 40

                            TextField{
                                width: parent.width/2
                                height: parent.height
                                id:textFieldSelectMark
                                text: "0"
                                color: text_color
                                background: Rectangle {
                                    anchors.fill: parent
                                    color: background_color_2
                                }
                            }
                            Button{
                                width: parent.width/2
                                height: parent.height
                                id:buttonSelectMark
                                contentItem: Text {
                                    text: qsTr("select by id")
                                    color: text_color
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                background: Rectangle {
                                    anchors.fill: parent
                                    color: (parent.hovered|parent.pressed)?button_hover_color:button_color
                                }
                                onClicked:{
                                    var index = textFieldSelectMark.text;
                                    testCanvas.selectMarkById(index);
                                    //updateImage(1);
                                }
                            }
                        }

                        ComboBox{
                            id:labelbox
                            width: parent.width
                            height: 40
//                            model:["鸟窝","鸟"]
                            model:["绝缘子-U型抱箍-螺母异常","绝缘子-U型抱箍-螺母正常","绝缘子-U型抱箍-螺母模糊"]//chLi
                            contentItem: Text {
                                text: parent.currentText
                                color: text_color
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: background_color_2
                            }
                            onActivated: {
                                if(-1 < testCanvas.currentMarkIndex  && testCanvas.currentMarkIndex < listViewLabels.count){
                                    ReviewController_Review.reviseMark(testCanvas.currentMarkIndex,labelbox.currentIndex);
                                    updateIDAndLabel();
                                    testCanvas.updateCanvasMark();
                                    testCanvas.myPaint();
                                }
                            }
                        }

                        Button{
                            id:saveMark;
                            width: parent.width
                            height: 40
                            contentItem: Text {
                                text: qsTr("save mark")
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
                                ReviewController_Review.saveMarkToReview();
                            }
                        }



                    }
                }

            }
        }

        Rectangle{
            id:colContent
            width: parent.width - colOperate.width - predictArea.width
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
                            colOperate.visible = false
                            colOperate.width = 0

                        } else {
                            leftShrinkStatus = 0
                            leftShrink.source = "./resource/left_01.png"
                            colOperate.visible = true
                            colOperate.width = 200

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
                                            if(!switchAutoLoadPredict.checked)
                                                ReviewController_Review.setLoadPredictResultController(false);
                                            ReviewController_Review.previousImage();
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
                                            //ReviewController_Review.setPartCurrentIndex(parseInt(currentIndexLabel.text)-1,true);
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
                                            if(!switchAutoLoadPredict.checked)
                                                ReviewController_Review.setLoadPredictResultController(false);
                                            ReviewController_Review.nextImage();
                                        }
                                    }

                                }
                            }

                            Rectangle{
                                id:rectInfoGrid
                                width: parent.width
                                height: parent.height-rectIndexControll.height-parent.padding*2-2*parent.spacing
                                color: background_color_0
                                Column{
                                    anchors.fill: parent
                                    spacing: 10
                                    Rectangle{
                                        width: parent.width
                                        height: parent.height/2-10
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

                            // 加汇总信息

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
                                  text: qsTr("Label")+": " + label_list[Number(modelData.Label)]
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
            //updateCurrentProgressValue();
        }
    }

    Component.onCompleted: {
        label_list = ReviewController_Review.getLabelList();
        ReviewController_Review.updateImagePath(-1);
        updateInfo();
        ReviewController_Review.signalImageInfoUpdated.connect(updateInfo);
        ReviewController_Review.updateImagePredicitInfo.connect(updateIDAndLabel);
        ReviewController_Review.updateImagePredicitInfo.connect(updateInfo);
    }



    function onFilterUpdated(){
        var filter_array=[];
        filter_array.push({key:"isPredicted",operator:"=",value:isPredictedCombo.currentText})

        ReviewController_Review.slotSetFilter(filter_array);
    }

    function updateCanvasMark(){
        var result_current_json = ReviewController_Review.getImagePredictInfo();
        testCanvas.clearMarkList();
        var label_list = ReviewController_Review.getLabelList();
        for(var i=0; i<result_current_json.length; ++i)
        {
            var x1 = result_current_json[i]["x1"];
            var x2 = result_current_json[i]["x2"];
            var y1 = result_current_json[i]["y1"];
            var y2 = result_current_json[i]["y2"];
            var type =  Number(result_current_json[i]["label"]);
            var label = label_list[type];
            var ID = result_current_json[i]["ID"];
            print("label",label)
            if(i < ReviewController_Review.getAccountOfCurrentInfoFromReview())
                testCanvas.addMark(ID,label,[[x1,y1],[x2,y1],[x2,y2],[x1,y2]],2,colorList[type],colorList[type],0);
            else
                testCanvas.addMark(ID,label,[[x1,y1],[x2,y1],[x2,y2],[x1,y2]],2,colorList[type],colorList[type],1);
        }
    }

    function updateIDAndLabel(){
        listViewLabels.model = [];
        listViewLabels.model = ReviewController_Review.getImagePredictInfo();  
    }

    function updateImage(draw_polygon){
        var result_current_json = ReviewController_Review.getImagePredictInfo();
        testCanvas.clearMarkList();
        testCanvas.currentMarkIndex = -1;
        testCanvas.loadImage(ReviewController_Review.getCurrentImagePath());
        if(draw_polygon === 1)
        {
            var label_list = ReviewController_Review.getLabelList();
            for(var i=0; i<result_current_json.length; ++i)
            {
                var x1 = result_current_json[i]["x1"];
                var x2 = result_current_json[i]["x2"];
                var y1 = result_current_json[i]["y1"];
                var y2 = result_current_json[i]["y2"];
                var type =  Number(result_current_json[i]["Label"]);
                var label = label_list[type];
                var ID = result_current_json[i]["ID"];
                print("label",label)
                if(i < ReviewController_Review.getAccountOfCurrentInfoFromReview())
                    testCanvas.addMark(ID,label,[[x1,y1],[x2,y1],[x2,y2],[x1,y2]],2,colorList[type],colorList[type],0);
                else
                    testCanvas.addMark(ID,label,[[x1,y1],[x2,y1],[x2,y2],[x1,y2]],2,colorList[type],colorList[type],1);
            }
        }
        testCanvas.myPaint();
    }

    function updateInfo(){
        ReviewController_Review.set_pic_filter_array(isPredictedCombo.currentIndex, predictResultCombo.currentIndex)
        ReviewController_Review.updatePicModel()

        var currenIndexAndTotal = ReviewController_Review.getCurrentIndexAndTotal();
        currentIndexLabel.text=String(currenIndexAndTotal[0]);
        totalLabel.text="/"+currenIndexAndTotal[1];
        updateIDAndLabel();
        updateImage(1);
        var info_list=ReviewController_Review.getFirstGridInfo();
        var is_predict_value_list=["是","否"];
        var predict_result_list=["---","正常","异常"];
        for(var i=0; i<info_list.length; ++i){
            print(info_list[i].role,info_list[i].title)
            if(info_list[i].role==="IsPredicted"){
                info_list[i].value=is_predict_value_list[info_list[i].value];
            }
            if(info_list[i].role==="PredictResult"){
                info_list[i].value= predict_result_list[info_list[i].value];
            }
        }

        infoFirstDisplayGrid.updateAOITitleValue(info_list);
        infoSecondDisplayGrid.updateAOITitleValue(ReviewController_Review.getSecondGridInfo());
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*//*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
