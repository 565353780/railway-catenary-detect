import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls 2.4



Rectangle{
    id:rectBackground0
    width: parent.height
    height: parent.width
    transform: Rotation{ origin.x:rectBackground0.width/2; origin.y: rectBackground0.height/2; angle: 90}

    x:(parent.width-parent.height)/2
    y:(parent.height-parent.width)/2

    gradient: Gradient{
        GradientStop{
            position: 0
            color: "#132463"
        }
        GradientStop{
            position: 1
            color: "#001433"
        }
    }


    Image {
        id: name
        anchors.fill: parent
        source: "./resource/star_background.png"
        fillMode: Image.Tile
        opacity: 0.5

    }
}
