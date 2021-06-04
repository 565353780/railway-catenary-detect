import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.4
import Qt.labs.platform 1.0
import QtQuick.Controls.Material 2.0


ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1200
    height: 1000
    title: qsTr("AOI-AI SYSTEM")+"v1.0"
    Component.onCompleted: {
        MyController.signalSetVisible.connect(function(b){
            mainWindow.show();
        });
    }

    onVisibilityChanged: {
        MyController.windowVisibleChanged(visible);
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
        Menu{
            title:qsTr("save")
            font.pixelSize: 20
            MenuItem{
                text: qsTr("export csv")
                onTriggered: {
                    folderDialog.acceptLabel=qsTr("save")
                    folderDialog.open()
                }
            }
        }
    }
    header: Column{
        Row{
            visible: false
            id: rowWatchPath
            z:2
            height: 60
            width: 400
            topPadding: 10
            Label{
                id: watchLabel
                height: 40
                text:qsTr("Watching")
                verticalAlignment: Text.AlignVCenter
                leftPadding: 20
                rightPadding: 20
            }
            Button{
                id:showPathsButton
                width: parent.width-watchLabel.width
                background:Rectangle{
                    color: "#fbf2f2"
                    border.color: "#afaeae"
                    radius: 10
                }
                onClicked: {
                    pathColumn.visible=!pathColumn.visible;
                }
            }

            Component.onCompleted: {
                var pathList=MyController.getWatchPathList();
                repeaterPathPrefix.model=MyController.getWatchPathPrefix();
                watchPathRepeater.model=pathList;
                showPathsButton.text=pathList[0];

            }
        }

        TabBar {
            width: parent.width
            id: tabBar
            currentIndex: swipeView.currentIndex
            Repeater {
                model: [qsTr("Review")]
                TabButton {
                    text: modelData
                }
            }
        }
    }

    Row{
        id: pathColumn
        width: 540
        height: 400
        visible: false;
        topPadding: 20
        x:0
        y:0
        z:2
        Column{
            id:pathPrefix
            width: 100
            Repeater{
                id:repeaterPathPrefix
                model:3
                Label{
                    text: modelData
                    width: parent.width
                    height:40
                    background: Rectangle{
                        anchors.fill: parent
                        border.color: "gray"
                    }
                }
            }
        }
        Column{
            id:pathValue
            width: 440
            height: 400
            z:2
            visible: true;
            Repeater{
                id: watchPathRepeater
                model:[]

                TextField{
                    height:40
                    text: modelData
                    selectByMouse: true
                    width: parent.width
                    background: Rectangle{
                        anchors.fill: parent
                        border.color: "gray"
                    }
                }
            }
            Button{
                id: commitButton
                text:qsTr("commit")
                width: parent.width
                onClicked: {
                    pathColumn.visible=false;
                    showPathsButton.text=watchPathRepeater.itemAt(2).text;
                    var watchPathList=[];

                    for(var i=0; i<watchPathRepeater.count; ++i){
                        watchPathList[i]=(watchPathRepeater.itemAt(i).text);
                    }

                    MyController.setWatchPathList(watchPathList);

                }
            }
        }
    }
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        PageReview {
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
        nameFilters: ["Text Files ( *.xml)", "*.*"]
        fileMode: FileDialog.OpenFiles
        onAccepted: {
            for(var i in files){
                var file_url=files[i];
                var filepath = String(file_url)
                //print(filepath)
                var filePathSplit=filepath.slice(8).split('/');
                var filename=String(filePathSplit[filePathSplit.length-1]);
                var suffix=filename.substring(filename.lastIndexOf('.')+1,filename.length);

//                busyIndicator.visible = true
//                busyIndicator.running = true
//                textLoading.visible = true
//                textLoading.text=filename+qsTr("Loading");

                //remove file:///
                //print(Qt.platform.os , suffix);
                if (Qt.platform.os === "windows" || Qt.platform ==="linux") {
                    if(suffix==="xlsx" || suffix==="xls"){
                        //MyController.readXlsxInThread(filepath.slice(8));
                    }
                    else if(suffix==="xml"){
                        //print(filepath.slice(8));
                        MyController.readXmlFile(filepath.slice(8));
                    }
                }
                else if(Qt.platform=="linux"){
                    if(suffix=="xlsx" || suffix=="xls"){
                        //MyController.readXlsxInThread(filepath.slice(8));
                    }
                    else if(suffix=="xml"){
                        MyController.readXmlFile(filepath.slice(7));
                    }
                }
            }
        }
        Component.onCompleted: {
            fileMode=FileDialog.OpenFiles
            MyController.signalFinishDataImport.connect(
                        function(){
                            busyIndicator.visible = false
                            busyIndicator.running = false
                            textLoading.visible = false
                            textLoading.text=qsTr("Loading");
                        });
        }
    }

    FolderDialog {
        id: folderDialog
        title: qsTr("Please choose an image file")

        onAccepted: {
            var filepath =  String(folderDialog.folder)
            //remove file:///
            if (Qt.platform.os == "windows") {
                if(folderDialog.acceptLabel===qsTr("save"))
                {
                    MyController.writeToCsv(filepath.slice(8)+"/"+qsTr("DataBase")+Qt.formatDateTime(new Date(), "yyyy-MM-dd_hh-mm-ss")+".csv")
                }
                else if(folderDialog.acceptLabel===qsTr("open folder")){
                    MyController.openXmlFolder(filepath.slice(8)+"/")
                }
            }
            else if(Qt.platform.os=="linux") {
                if(folderDialog.acceptLabel===qsTr("save"))
                {
                    MyController.writeToCsv(filepath.slice(7)+"/"+qsTr("DataBase")+Qt.formatDateTime(new Date(), "yyyy-MM-dd_hh-mm-ss")+".csv")
                }
                else if(folderDialog.acceptLabel===qsTr("open folder")){
                    MyController.openXmlFolder(filepath.slice(7)+"/")
                }
            }
        }

    }


}


/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
