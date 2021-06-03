import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Qt.labs.platform 1.0

import QtWebEngine 1.7

Page {
    id: pageStatistics
    width: 960
    height: 800

    // close calendar
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            calendarBegin.visible = false
            calendarEnd.visible = false
        }
    }

    Rectangle{
        anchors.fill: parent
        border.color: "gray"
        border.width: 1
    }

    Row{
        id:rowMain
        anchors.fill: parent
        anchors.topMargin: 1

        Page {
            id: pageViewStatistics
            width: parent.width - 200
            height: parent.height

            footer: TabBar {
                id: tabbarfooter
                currentIndex: swipeTableChart.currentIndex
                TabButton {
                    text: qsTr("Review-State")
                    checkable: true
                    background: Rectangle {
                        border.color: "gray"
                        color: (parent.hovered || parent.checked) ? "red" : "white"
                        opacity: 0.1
                        radius: 5
                    }
                    checked: true
                }
            }

            Column {
                anchors.fill: parent

                SwipeView {
                    id: swipeTableChart
                    z: -1
                    currentIndex: tabbarfooter.currentIndex
                    width: parent.width
                    height: parent.height

                    // Review
                    Page {
                        z: -1
                        // id: tabReview
                        visible: swipeTableChart.currentIndex == 0
                        Column {
                            // id: columnChartReview
                            anchors.fill: parent

                            // Label Type Table
                            CustomedTableview {
                                id: tablelabel;
                                width: parent.width
                                height: 200
                                Component.onCompleted: {
                                    initTableLabelHeaderColumns()
                                    updataTableLabel()
                                }
                            }

                            // static mode
                            Row {
                                width: parent.width
                                height: parent.height - tablelabel.height
                                WebEngineView {
                                    id: pieviewpicturedetectedstatic
                                    height: parent.height
                                    width: parent.width
                                    visible: true
                                    url: "file:///home/abaci/chLi/github/railway-catenary-detect/railway-catenary-detect/UI/html/pie_pictureDetected.html"
                                    Component.onCompleted: {
                                        updataPieViewPictureDectected()
                                    }
                                    onLoadingChanged: {
                                        if (pieviewpicturedetectedstatic.loading == false) {
                                            updataPieViewPictureDectected()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // right operate area
        Rectangle{
            id: rectOperate
            width: 200
            height: parent.height
            color: "white"
            Column {
                id: columnOperate
                z: 2
                anchors.fill: parent
                spacing: 10

                Column {
                    id: colSelect
                    height: parent.height * 2 / 3
                    width: parent.width
                    spacing: 40
                    bottomPadding: 40

                    Label {
                        text: qsTr("Date Interval")
                        font.pixelSize: 20
                        font.bold: true
                        width: parent.width
                        height: 60
                        padding: 20
                        background: Rectangle {
                            color: "#b1b3b5"
                        }
                    }
                    // 日期选择区，鼠标点击非挂历区，关闭挂历显示
                    Button {
                        id: selectDate
                        width: parent.width - 20
                        text: qsTr("Begin ") + Qt.formatDate(calendarBegin.selectedDate, "yyyy-MM-dd");
                        onClicked: function() {
                            calendarBegin.visible = true
                            calendarEnd.visible = false
                        }
                        Calendar {
                            id: calendarBegin
                            z: 2
                            x: 150
                            y: 160
                            visible: false
                            anchors.right: parent.left
                            anchors.top: parent.top
                            selectedDate: new Date("2019/01/01");
                            onClicked: {
                                visible = false;
                                selectDate.text = qsTr("Begin ") + Qt.formatDate(calendarBegin.selectedDate, "yyyy-MM-dd");
                                updataData()
                                //updateFilterCombobox();
                            }
                        }
                    }

                    Button {
                        id: selectEndDate
                        width: parent.width - 20
                        text: qsTr("End ") + Qt.formatDate(calendarEnd.selectedDate, "yyyy-MM-dd");
                        onClicked: function() {
                            calendarEnd.visible = true
                            calendarBegin.visible = false
                        }
                        Calendar {
                            id: calendarEnd
                            z: 1
                            visible: false
                            anchors.right: parent.left
                            anchors.top: parent.top
                            selectedDate: new Date()
                            onClicked: {
                                visible = false;
                                selectEndDate.text = qsTr("End ") + Qt.formatDate(calendarEnd.selectedDate, "yyyy-MM-dd");
                                updataData()
                                //updateFilterCombobox();
                            }
                        }
                    }

                    Label {
                        id: labelAttributeSelect
                        text: qsTr("Attribute Select")
                        font.pixelSize: 20
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                        width: parent.width
                        height: 60
                        padding: 20
                        background: Rectangle {
                            color: "#b1b3b5"
                        }
                    }

                    Column {
                        width: parent.width
                        height: 200
                        Repeater {
                            id: repeaterCountSlectValue
                            model: 1
                            ComboBox {
                                model: []
                                property string displayTitle: "";
                                property string actualRole: ""
                                width: parent.width - 20
                                height: 60
                                x: 10;
                                font.pixelSize: 12
                                popup.font.pixelSize: 12
                                popup.width: 200
                                popup.contentWidth: 200
                                displayText: displayTitle + ":" + currentText
                                onActivated: updateFilterCombobox()
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        //updataData()
    }

    function updataData()
    {
        var attributesfilter = ""
        for (var i = 0; i < repeaterCountSlectValue.count; ++i) {
            if (repeaterCountSlectValue.itemAt(i).currentIndex > 0) {
                if (attributesfilter.length > 0)
                {
                    attributesfilter += ","
                }
                attributesfilter += repeaterCountSlectValue.itemAt(i).actualRole + ":" + repeaterCountSlectValue.itemAt(i).currentText
            }
        }
        var beginDate = Qt.formatDate(calendarBegin.selectedDate, "yyyy/MM/dd");
        var endDate= Qt.formatDate(calendarEnd.selectedDate, "yyyy/MM/dd");

        StatisticController.updateUserFilter(attributesfilter, beginDate, endDate);
        StatisticController.updateData();

        updataTableLabel()
        updataPieViewPictureDectected()

    }

    function updataTableLabel()
    {
        //print(TableLabelModel.data(0,3));
        //tablelabel.initColumns(TableLabelModel);
        //tablelabel.initColumns([{role:"0",width:"100",title:"0"},{role:"1",width:"100",title:"1"},{role:"2",width:"100",title:"2"}])
        tablelabel.model = TableLabelModel

        var column0 = tablelabel.getColumn(0);
        if (column0.width < 100) {
            column0.width = 100;
        }
    }

    function initTableLabelHeaderColumns()
    {
        var headerColumns = StatisticController.getTableLabelHeader()
        print("headerColumns = ", headerColumns)
        var labelHeader = []
        for (var col_i = 0; col_i < headerColumns.length; col_i++)
        {
            labelHeader.push({
                             role: String(headerColumns[col_i]),
                             width: "100",
                             title: String(headerColumns[col_i])
                             })
        }
        tablelabel.initColumns(labelHeader);
    }

    function updataPieViewPictureDectected()
    {
        var pictureDetectedInfo = StatisticController.getPictureDetectedInfo();
        var command = "setData('"
                + pictureDetectedInfo[0] + "','"
                + pictureDetectedInfo[1] + "','"
                + "')";
        pieviewpicturedetectedstatic.runJavaScript(command, function callback(){});

        command = "resetSize("
                + String(pieviewpicturedetectedstatic.width) + ","
                + String(pieviewpicturedetectedstatic.height)
                + ");";
        pieviewpicturedetectedstatic.runJavaScript(command, function callback(){});
    }

    function updateFilterCombobox() {
        var filterObj = StatisticController.getNameValuesForTimeFilter();
        repeaterCountSlectValue.model = Object.keys(filterObj).length
        var i = 0;
        for (var key in filterObj) {
            var fieldObj = filterObj[key];
            repeaterCountSlectValue.itemAt(i).displayTitle = fieldObj["title"];
            repeaterCountSlectValue.itemAt(i).actualRole = fieldObj["role"]
            repeaterCountSlectValue.itemAt(i).model = fieldObj["values"];
            ++i;
        }
    }

}
