import QtQuick.Controls 2.4
import QtQuick 2.4
ScrollView{
    id:scroll;
    clip: true;
    contentWidth: width
    contentHeight: gridInfo.height
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

    Grid{

        //bottomPadding: 10
        id: gridInfo
        width: parent.width
        columns: width/200
        height: Math.ceil(repeaterAOIInformation.count / gridInfo.columns) * 70
        spacing: 10
        Repeater {
            id: repeaterAOIInformation
            model:[]
            Rectangle{
                width: parent.width/gridInfo.columns
                height: 70
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
