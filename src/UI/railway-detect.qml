import QtQuick 2.7
import QtQuick.Controls 1.4
//import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
//import QtCharts 2.2
import QtQuick.Controls 2.4

//import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.0


//import MainContorller 1.0
ApplicationWindow {

    //	0752FF		345EBF		0234A6		457DFF		76A0FF test
    property color background_color_0: Qt.rgba(0,0,0,0)//"#132463"
    property color background_color_1: Qt.rgba(4/255,71/255,188/255)//"#7f95e6"
    property color background_color_2: "#94b6e9"//Qt.rgba(0.1,0.25,0.7,1)//"#75C3FF"//"
    property color text_color: "#FFFFFF"
    property color border_color: Qt.rgba(21/255,66/255,93/255,1)
    property color button_hover_color: Qt.rgba(0.2,0.35,0.9,0.9)
    property int default_border_width: 2;

    id: mainWindow
    visible: true
    width: 1200
    height: 1000
    title: qsTr("railway-detect")+"v1.0"


    TabView {
        id: tabViewMain
        anchors.fill: parent


        Tab{
            title: qsTr("Predict")
            PagePredict {
                id:pagePredict

            }
        }
        Tab{
            title: qsTr("Statistics")
            PageStatistics {
                id: pageStatistics

            }
        }
        Tab{
            title: qsTr("Review")
            PageReview {
                id:pageReview

            }
        }
        style:
            TabViewStyle {
                  frameOverlap: 1
                  tab: Rectangle {
                      color: (styleData.selected||styleData.hovered) ? button_hover_color :background_color_2
                      border.color:  border_color
                      implicitWidth: Math.max(tabViewMain.width/3 , 280)
                      implicitHeight: 40
                      radius: 2
                      Text {
                          id: text
                          anchors.centerIn: parent
                          text: styleData.title
                          color: styleData.selected ? "white":"black";
                          font.pixelSize: 16
                          font.family: "Heiti"
                          font.bold: true;
                      }
                  }
                  frame: Rectangle { color: background_color_2 }
        }

    }
    MenuBar{
        id: mainMenuBar

        Menu{
            title: qsTr("open")
            font.pixelSize: 20
            MenuItem{

                text: qsTr("open xml")
                onTriggered: {
                    fileDialog.acceptLabel = qsTr("open");
                    fileDialog.mission="loadPic"
                    fileDialog.open()
                }
            }
            MenuItem{
                text: qsTr("open xml folder")
                onTriggered: {
                    folderDialog.acceptLabel= qsTr("open folder");
                    folderDialog.open()
                }
            }
        }

    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose an image file")
        nameFilters: ["Image ( *.jpg)", "*.*"]
        fileMode: FileDialog.OpenFiles
        property  string mission: "value"
        onAccepted: {
            for(var i in files){
                var file_url=files[i];
                var filepath = String(file_url)
                //print(filepath)
                var filePathSplit=filepath.slice(8).split('/');
                var filename=String(filePathSplit[filePathSplit.length-1]);
                var suffix=filename.substring(filename.lastIndexOf('.')+1,filename.length);


                if (Qt.platform.os === "windows" ) {
                    filepath=filepath.slice(8)


                }
                else if(Qt.platform==="linux"){
                    filepath=filepath.slice(7)
                }
                if(mission=="loadPic"){
                    var success=PicDataLoader.loadPic(filepath);
                    if(success){
                        pagePredict.updateInfo();
                    }
                }
            }
        }
        Component.onCompleted: {
            fileMode=FileDialog.OpenFiles
        }
    }

    FolderDialog {
        id: folderDialog
        title: qsTr("Please choose an image file")
        property string mission: "value"
//        signal signalLoadPicFromDir(string path);
        onAccepted: {
            var filepath =  String(folderDialog.folder)
            //remove file:///
            if (Qt.platform.os == "windows") {
                filepath=filepath.slice(8)
            }
            else if(Qt.platform.os=="linux") {
                filepath=filepath.slice(7)
            }
            if(mission=="loadPicFolder"){
                PicDataLoader.loadPicsFromDir(filepath)
                ReviewController_Predict.updatePicModel();
                ReviewController_Review.updatePicModel();
            }
        }

    }

    Component.onCompleted: {
//        folderDialog.signalLoadPicFromDir.connect(PicDataLoader.slotloadPicsFromDir);
    }

}

