import QtQuick 2.0
import QtQuick.Controls 2.4

Column{
    spacing: 10
    anchors.fill: parent
    Label {
        width: parent.width
        height: 30
        text: modelData.title
        font.bold: true
        background: Rectangle{
            color: "#90b2e0";
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    Rectangle
    {
        width: parent.width
        height: 30
        Label {
            id: subLabe
            width: parent.width
            height: 30
            text: modelData.value
            elide: Qt.ElideMiddle
            MouseArea {
                hoverEnabled: true
                anchors.fill: parent
                onEntered: {
                    ToolTip.text = "";
                    if(ToolTip.text.length != 0)
                        ToolTip.visible = true;
                    else
                        ToolTip.visible = false;
                }
                onExited: {
                     ToolTip.visible = false;
                }
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        ComboBox{
            id: comb
            visible: false
            width: parent.width
            height: 30
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
