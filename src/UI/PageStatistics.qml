import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Qt.labs.platform 1.0

import QtWebEngine 1.7

Page {
//    property var background_color_0: Qt.rgba(0,0,0,0)//"#132463"
//    property var background_color_1: "#5472de"
//    property var background_color_2: "#94a6ea"
//    property var text_color: "#FFFFFF"
//    property var border_color: Qt.rgba(21/255,66/255,93/255,1)//"#3BBEE3"
//    property int default_border_width: 2
    property int default_height_label_0: 60
    property int default_height_label_1: 45

    property var old_filter_combo_number: 0
    property var hasInitTableLabel: 0

    id: pageStatistics
    width: 960
    height: 800

    RectBackground0 {}

    // close calendar
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            calendarBegin.visible = false
            calendarEnd.visible = false
        }
    }

    Rectangle {
        // 操作区域
        z: 5
        id: operateRectangle
        anchors.top: parent.top
        anchors.topMargin: 1
        anchors.left: parent.left
        border.width: default_border_width
        border.color: border_color
        color: background_color_0
        width: 200
        height: parent.height

        Rectangle {
            // 日期选择区域
            z: 6
            id: dateSelectArea
            anchors.top: parent.top
            anchors.left: parent.left
            color: background_color_0
            width: parent.width
            height: 260

            Column {
                z: 7
                width: parent.width - 2
                height: parent.height

                Rectangle {
                    width: parent.width
                    height: parent.height * 3 / 10
                    color: background_color_0
                    Rectangle {
                        anchors.top: parent.top
                        anchors.topMargin: 1
                        anchors.left: parent.left
                        anchors.leftMargin: 1
                        width: parent.width
                        height: default_height_label_0
                        color: background_color_1
                        Text {
                            anchors.fill: parent
                            text: qsTr("Date Interval")
                            font.pixelSize: 20
                            font.bold: true
                            color: text_color
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: parent.height * 3 / 10
                    color: background_color_0
                    Button {
                        id: selectDate
                        anchors.top: parent.top
                        anchors.topMargin: 1
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        width: parent.width - 2 * anchors.leftMargin
                        height: default_height_label_1
                        contentItem: Text {
                            id: beginbtn
                            text: qsTr("End ") + Qt.formatDate(calendarEnd.selectedDate, "yyyy-MM-dd")
                            color: text_color
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: background_color_2
                        }
                        onClicked: function() {
                            calendarBegin.visible = true
                            calendarEnd.visible = false
                        }
                        Calendar {
                            id: calendarBegin
                            visible: false
                            anchors.left: parent.right
                            anchors.leftMargin: parent.anchors.leftMargin + 1
                            anchors.top: parent.top
                            selectedDate: new Date("2019/01/01");
                            onClicked: {
                                visible = false;
                                beginbtn.text = qsTr("Begin ") + Qt.formatDate(calendarBegin.selectedDate, "yyyy-MM-dd");
                                updataData()
                                updateFilterCombobox();
                            }
                        }
                    }

                }

                Rectangle {
                    z: 8
                    width: parent.width
                    height: parent.height * 4 / 10
                    color: background_color_0
                    Button {
                        z: 9
                        id: selectEndDate
                        anchors.top: parent.top
                        anchors.topMargin: 1
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        width: parent.width - 2 * anchors.leftMargin
                        height: default_height_label_1
                        contentItem: Text {
                            id: endbtn
                            text: qsTr("End ") + Qt.formatDate(calendarEnd.selectedDate, "yyyy-MM-dd")
                            color: text_color
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: background_color_2
                        }
                        onClicked: function() {
                            calendarEnd.visible = true
                            calendarBegin.visible = false
                        }
                        Calendar {
                            id: calendarEnd
                            z: 10
                            visible: false
                            anchors.left: parent.right
                            anchors.leftMargin: parent.anchors.leftMargin + 1
                            anchors.top: parent.top
                            selectedDate: new Date()
                            onClicked: {
                                visible = false;
                                endbtn.text = qsTr("End ") + Qt.formatDate(calendarEnd.selectedDate, "yyyy-MM-dd");
                                updataData()
                                updateFilterCombobox();
                            }
                        }
                    }
                }

            }

        }

        Rectangle {
            // 属性选择区域
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            color: background_color_0
            width: parent.width
            height: parent.height - dateSelectArea.height


            Column {
                width: parent.width - 2
                height: parent.height

                Rectangle {
                    width: parent.width
                    height: dateSelectArea.height * 3 / 10
                    color: background_color_0
                    Rectangle {
                        anchors.top: parent.top
                        anchors.topMargin: 1
                        anchors.left: parent.left
                        anchors.leftMargin: 1
                        width: parent.width
                        height: default_height_label_0
                        color: background_color_1
                        Text {
                            id: labelAttributeSelect
                            anchors.fill: parent
                            text: qsTr("Attribute Select")
                            font.pixelSize: 20
                            font.bold: true
                            color: text_color
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                Column {
                    width: parent.width
                    height: parent.height - dateSelectArea.height * 3 / 10
                    spacing: 10

                    Repeater {
                        id: repeaterCountSlectValue
                        model: 0

                        ComboBox {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            width: parent.width - 2*anchors.leftMargin
                            height: default_height_label_1
                            font.pixelSize: 12
                            model: ['none']
                            property var displayTitle: '';
                            property var actualRole: ''
                            popup.font.pixelSize: 10
                            popup.width: width
                            popup.contentWidth: width
                            contentItem: Text {
                                text: displayTitle + ": " + actualRole
                                color: text_color
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: background_color_2
                            }
                            onActivated:  updateFilterCombobox()
                        }
                    }
                }
            }

        }
    }

    Rectangle {
        // 主显示区
        z: 1
        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.right: parent.right
        anchors.rightMargin: 4
        color: background_color_0
        width: parent.width - operateRectangle.width - anchors.rightMargin
        height: parent.height

        Rectangle {
            id: tablearea
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 1
            width: parent.width
            height: parent.height * 3 / 7
            color: background_color_0
            CustomedTableview {
                id: tablelabel
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 1
                width: parent.width
                height: parent.height
                cellHandlerFlag: 1
                function cellHandler(value_, row_, column_) {
                    print("row_ = ", row_)
                    print("column_ = ", column_)
                    print("value_ = ", value_)
                }
                Component.onCompleted: {
                    updataTableLabel()
                }
            }
        }

        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 1
            anchors.topMargin: parent.height + parent.anchors.topMargin
            width: parent.width
            height: parent.height - tablearea.height
            color: background_color_0
            WebEngineView {
                id: staticpieview
                //anchors.centerIn: parent
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 80
                height: parent.height - 20
                visible: true
                url: "file:///home/abaci/wjh/railway-catenary-detect/bin/html/pie_pictureDetected.html"
                Component.onCompleted: {
                    updataPieViewData()
                }
                onLoadingChanged: {
                    if (staticpieview.loading == false) {
                        updataPieViewData()
                    }
                }
            }
        }

    }

    Component.onCompleted: {
        //updataData()
        updateFilterCombobox()
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
        updataPieViewData()

    }

    function updataTableLabel()
    {
        if (hasInitTableLabel == 0) {
            initTableLabelHeaderColumns();
        }

        tablelabel.model = TableLabelModel

        var column0 = tablelabel.getColumn(0);
        if (column0.width < 100) {
            column0.width = 100;
        }
    }

    function initTableLabelHeaderColumns()
    {
        var titles = StatisticController.getTableLabelHeaderTitle()

        var roles = StatisticController.getTableLabelHeaderRole()
        var labelHeader = []
        for (var col_i = 0; col_i < titles.length; col_i++)
        {
            labelHeader.push({
                             role: String(roles[col_i]),
                             width: "100",
                             title: String(titles[col_i])
                             })
            hasInitTableLabel = 1
        }
        tablelabel.initColumns(labelHeader);
    }

    function updataPieViewData()
    {
        var info = StatisticController.getPieViewInfo();
        var command = "setData('"
                + info[0] + "','"
//                + 15 + "','"
//                + 101 + "','"
                + info[1] + "','"
                + "')";
        staticpieview.runJavaScript(command, function callback(){});

        command = "resetSize("
                + String(staticpieview.width) + ","
                + String(staticpieview.height)
                + ");";
        staticpieview.runJavaScript(command, function callback(){});
    }

    function updateFilterCombobox() {
        var filterObj = StatisticController.getAttributeFilterData();
        if (Object.keys(filterObj).length > 0 || old_filter_combo_number !== Object.keys(filterObj).length ) {
            old_filter_combo_number = Object.keys(filterObj).length
            repeaterCountSlectValue.model = Object.keys(filterObj).length
            var i = 0;
            for (var key in filterObj) {
                var fieldObj = filterObj[key];
                repeaterCountSlectValue.itemAt(i).displayTitle = fieldObj["title"]
                repeaterCountSlectValue.itemAt(i).actualRole = fieldObj["role"]
                repeaterCountSlectValue.itemAt(i).model = fieldObj["values"]
                ++i;
            }
        }
    }
}
