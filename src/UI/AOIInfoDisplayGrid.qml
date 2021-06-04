import QtQuick.Controls 2.4
import QtQuick 2.4
ScrollView{
    property var background_color_0: Qt.rgba(0,0,0,0)//"#132463"
    id:scroll;
    clip: true;
    contentWidth: width
    contentHeight: gridInfo.height
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

    Grid{
        id: gridInfo
        width: parent.width - 80
        columns: width/200
        height: Math.ceil(repeaterAOIInformation.count / gridInfo.columns) * 70
        spacing: 10
        Repeater {
            id: repeaterAOIInformation
            model:[]
            Rectangle{
                width: parent.width/gridInfo.columns
                height: 60
                color: background_color_0
                AOIInfoDisplayBlock{

                }
            }
        }

        /**
            model类型：jsonarray，每个元素包含 title,value,role三个属性
          */
    }
    function updateAOITitleValue(model)
    {
        repeaterAOIInformation.model = [];
        repeaterAOIInformation.model = model;
    }

}
