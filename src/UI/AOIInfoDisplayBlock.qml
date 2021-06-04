import QtQuick 2.0
import QtQuick.Controls 2.4

Column{
    property var background_color_0: Qt.rgba(0,0,0,0)//"#132463"
    property var background_color_1: "#5472de"
    property var background_color_2: "#94a6ea"
    property var text_color: "#FFFFFF"
    property var border_color: Qt.rgba(21/255,66/255,93/255,1)//"#3BBEE3"
    property int default_border_width: 2;
    //spacing: 10
    anchors.fill: parent
    Rectangle {
        width: parent.width
        height: parent.height
        border.width: default_border_width
        border.color: border_color
        color: background_color_0

        Label {
            anchors.top: parent.top
            width: parent.width
            height: parent.height/2
            Text {
                anchors.fill: parent
                text: modelData.title
                font.pixelSize: 20
                font.bold: true
                color: text_color
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle{
                color: background_color_2
            }
        }

        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: parent.height/2
            color: background_color_0
            Label {
                id: subLabe
                width: parent.width
                height: parent.height
                Text {
                    anchors.fill: parent
                    text: modelData.value
                    font.bold: true
                    color: text_color
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle{
                    color: background_color_0
                }
                elide: Qt.ElideMiddle
                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onEntered: {
                        ToolTip.text = modelData.value;
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
            ComboBox{
                id: comb
                visible: false
                width: parent.width
                height: 40
                model : processText(modelData.value)
                function processText(data){
                    var textlist=String(data).split(";");
                    if(textlist.length > 1 && modelData.role === 1)
                    {
                       visible = true;
                       subLabe.visible = false;
                    }
                    return textlist;
                }
            }
        }





    }


}
