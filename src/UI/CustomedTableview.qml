import QtQuick 2.7
import QtQuick.Controls 1.4

TableView {
    id: tableview
    property var holdMenuModelDataArray: null
    backgroundVisible: false;
    frameVisible: false;
    itemDelegate: cell
    headerDelegate: header
    rowDelegate: row
    model: ListModel{}

    // 定义表头的委托
    Component {
        id: header
        Rectangle {
            id: headerRect;
            height: 40;
            width: 80
            border.color: "white";
            color: (styleData.value.length > 0) ? "#c3fafb" : "transparent"
            radius: 3;
            Text {
                id:textHeader
                z:1
                text: "<a href=\" " +String(styleData.row)+","+String(styleData.column)+"\">"+String(styleData.value)+String(styleData.column)+ "f</a>."
                anchors.centerIn: parent
                font.family: "微软雅黑";
                font.pixelSize: 16;
                color: "black";
                onLinkActivated: console.log(link + " link activated")

            }

        }
    }

    Component {
        id: cell
        Rectangle {
            id: tableCell
            anchors.fill: parent;
            anchors.margins: 3;
            border.color: "blue";
            radius: 3;
            color: (styleData.column === 0) ?  Qt.rgba(1,0.99,0.55,1): "transparent";
            height:42
            Text {
                id: textID;
                text: String(styleData.value) ;
                font.family: "微软雅黑";
                font.pixelSize: 12;
                // font.underline: (styleData.column===tableview.columnCount-1)
                anchors.fill: parent;
                color: "black";
                // elide: Text.ElideRight;
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.underline: true;

            }


        }
    }

    // 定义行的委托
    Component {
        id: row
        Rectangle {
            id: rect;
            height: 42;
            color: "transparent";
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
            tableview.removeColumn(0);
        }
        var prefix = "import QtQuick 2.7;import QtQuick.Controls 1.4;TableViewColumn {width: (tableview.width-20)/"; //创建TableViewColumn的代码

        // 循环添加TableViewColumn
        var totalWidth = 0;
        for (i = 0; i < column_config.length; i++) {
            var str = prefix + String(column_config.length) + ";role:\"" + column_config[i]["role"] + "\";title:\"" +  column_config[i]["title"] + "\"}";
            var curCol = tableview.addColumn(Qt.createQmlObject(str, tableview, "dynamicSnippet1"));
        }

    }

    function linkClicked(link){
        print(link);
    }
}
