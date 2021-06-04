import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TableView {
//    property color background_color_0: Qt.rgba(0,0,0,0)
//    property color background_color_1: "#7f95e6"
//    property color background_color_2: "#94a6ea"
//    property color text_color: "#FFFFFF"
//    property color border_color: "#3BBEE3"
    property color header_color: Qt.rgba(0.1,0.3,0.8,0.9);
    property color cell_color: Qt.rgba(0.1,0.3,0.9,0.4);
    property color cell_hovered_color: Qt.rgba(0.9,0.3,0.9,0.4);

    id: tableview
    property int cellHandlerFlag: 0
    property var holdMenuModelDataArray: null
    backgroundVisible: false;
    frameVisible: false;
    itemDelegate: cell
    headerDelegate: header
    rowDelegate: row
    model: ListModel{}

//    style: {
//        color:Qt.rgba(0,0,0,0)
//    }

    // 定义表头的委托
    Component {
        id: header
        Rectangle {
            id: headerRect
            height: 40
            width: 80
            border.color: border_color
            color: header_color
            radius: 3

            Text {
                text: String(styleData.value)
                font.family: "微软雅黑"
                font.pixelSize: 12
                anchors.centerIn: parent
                color: text_color
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }


        }
    }

    Component {
        id: cell
        Rectangle {
            id: tableCell
            anchors.fill: parent;
            anchors.margins: 3;
            border.color: border_color;
            radius: 3;
            color: cell_color

            width: 80
            height:40
            Text {
                text: String(styleData.value)
                font.family: "微软雅黑"
                font.pixelSize: 12
                anchors.centerIn: parent
                color: text_color
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true;
                onEntered:  {
                    tableCell.color=cell_hovered_color;
                }
                onExited: {
                    tableCell.color=cell_color;

                }
                onPressed: {
                    if (cellHandlerFlag == 1) {
                        cellHandler(styleData.value, styleData.row, styleData.column)
                    }
                }
            }

            /*
            Button {
                width: 80
                height:40


                style:ButtonStyle{
                    background:Rectangle{
                        anchors.fill: parent;
                        color: background_color_2;
                    }
                }

                onClicked: {
                    print("cell = ")
                    print("row = ", styleData.row)
                    print("column = ", styleData.column)
                    print("value = ", styleData.value)
                }

            }
            */
        }
    }

    // 定义行的委托
    Component {
        id: row
        Rectangle {
            id: rect;
            height: 42;
            color: styleData.selected?Qt.rgba(1,0.1,0.1,0.4):background_color_0
        }
    }

    // 更新表格标题列高度和role
    function updateColumn2Table(arrayTitle, arrayWidth, arrayRoles) {
        if (arrayTitle.length !== arrayWidth.length || arrayTitle.length > arrayRoles.length) {
            return;
        }

        initColumns(arrayTitle)
    }

    function initColumns(column_config) {
        var col_count = tableview.columnCount;
        for (var i = 0; i < col_count; i++) {
            tableview.removeColumn(i);
        }
        var prefix = "import QtQuick 2.7;import QtQuick.Controls 1.4;TableViewColumn {width: tableview.width/"; //创建TableViewColumn的代码

        // 循环添加TableViewColumn
        for (i = 0; i < column_config.length; i++) {
            var str = prefix + String(column_config.length) + ";role:\"" + column_config[i]["role"] + "\";title:\"" +  column_config[i]["title"] + "\"}";
            tableview.addColumn(Qt.createQmlObject(str, tableview, "dynamicSnippet1"));
        }
    }
}
