import QtQuick 2.7
import QtQuick.Controls 1.4
//import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
//import QtCharts 2.2
import QtQuick.Controls 2.4

//import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.0
import QtQuick.Controls.Material 2.0


//import MainContorller 1.0
ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1200
    height: 1000
    title: qsTr("railway-detect")+"v1.0"


    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        PagePredict {
            id:pagePredict

        }

        PageStatistics {
            id: pageStatistics

        }

//        PageReview {
//            id:pageReview

//        }
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
    header: TabBar {
        width: parent.width
        id: tabBar
        currentIndex: swipeView.currentIndex
        background: Rectangle{
            border.color: "gray"

        }

        Repeater {
            model: [qsTr("Predict"), qsTr("Statistics")];//,qsTr("Review")
            TabButton {
                text: modelData

            }
        }
    }
    footer: StatusBar {
        id: statusBar
        height: 30
        Row{
            anchors.fill: parent
            BusyIndicator {
                height:parent.height
                id: busyIndicator
                visible: false
            }
            Text {
                id: textLoading
                height: parent.height
                text: qsTr("Loading")
                visible: false
                font.pixelSize: 20
                font.bold: true
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
                ReviewController.updatePicModel();
            }
        }

    }

    Component.onCompleted: {

    }
}

