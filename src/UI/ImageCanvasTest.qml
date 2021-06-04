import QtQuick 2.0

import QtQuick.Controls 1.4
import QtQuick.Controls 2.14
Item {
    anchors.fill: parent
    Flickable {
        id: flick
        x: 20
        y: 20
        width: parent.width - 100
        height: parent.height-50
        clip: true
        Rectangle{
            color: "#000000"
            anchors.fill: parent
            Rectangle{
                color: "#000000"
                anchors.fill: parent
                Canvas{

                    width: 300
                    height: 300

                    id:canvasMain
                    onImageLoaded: {
                        computeDrawImageParam();
                        myPaint();
                    }
                    onAvailableChanged: {
                        ctx=getContext("2d");
                    }
                }

            }

            Image{
                id:imageToDraw
                visible: false;
                fillMode: Image.PreserveAspectFit
            }
        }
        Image {
            id: image
            source: "/firstImage"
            visible: false;
            anchors.horizontalCenter: parent.horizontalCenter
            property real originWidth: 820
            property real originHeight: 550
            fillMode: Image.PreserveAspectFit
            function computeOriginSize() {
                var scale = (flick.width / originWidth)
                        < (flick.height / originHeight) ? (flick.width / originWidth) : (flick.height / originHeight)
                slider.value = scale * 100

            }
            Component.onCompleted: {
                computeOriginSize();
            }
        }
    }

    Rectangle {
        id:scoll01
        anchors.right: flick.right
        width: 8
        height: flick.visibleArea.heightRatio * (parent.height-100)
        y: flick.visibleArea.yPosition * (parent.height-100)
        color: "gray"
        radius: 4
    }

    Rectangle {
        id:scoll02
        anchors.bottom: flick.bottom
        height: 8
        width: flick.visibleArea.widthRatio * (parent.width -100)
        x: flick.visibleArea.xPosition * (parent.width -100)
        color: "gray"
        radius: 4
    }

    Row {
        id: rowScale
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: parent.width

        Button {
            id: buttonMinus
            width: 66
            height: 36
            BorderImage {
                id: minusImage
                source: "/minus"
                width: 36
                height: 36
            }
            background: Rectangle {
                color: (buttonMinus.hovered | buttonMinus.pressed) ? "#80ee666e" : "#00ffffff"
                radius: 10
            }
        }
        Slider {
            id: slider
            width: rectImage.width - 200
            from: 1
            to: 200
            value: 100
        }
        Button {
            id: buttonPlus
            width: 66
            height: 36
            BorderImage {
                id: plusImage
                source: "/plus"
                width: 36
                height: 36
            }
            background: Rectangle {
                color: (buttonPlus.hovered
                        || buttonPlus.pressed) ? "#80ee666e" : "#00ffffff"
                radius: 10
            }
        }
    }

    property var ctx;
    property real scale: 1;
    property var translate: Object();
    property var polygonList:[];
    property var  drawImageParam: Object();


    function clearCtx(){
        if(!ctx) {ctx=canvasMain.getContext("2d");print("no context available");return;}
        ctx.fillStyle=Qt.rgba(0,0,0,1);
        ctx.fillRect(0,0,canvasMain.canvasSize.width,canvasMain.canvasSize.height);
        canvasMain.requestPaint()
    }

    function clearPolygonList(){
        while(polygonList.length>0){
            polygonList.pop()
        }
    }

    function addPolygon(polygon){

    }

    function drawPolygon(param){
        var polygon=param["polygon"],
                lineWidth=param["lineWidth"]||2,
                edgeColor=param["edgeColor"]||Qt.rgba(1,0,0,1),
                faceColor=param["faceColor"]||Qt.rgba(1,1,1,0);
        if(!ctx) {print("no context available");return;}
        if(polygon.length<2) {print("polygon point less than 2");return;}
        ctx.lineWidth = lineWidth;
        ctx.strokeStyle = edgeColor;
        ctx.fillStyle = faceColor;
        var num=polygon.length;
        ctx.beginPath();
        ctx.moveTo(Math.floor(polygon[0][0]*imageToDraw.width*scale),Math.floor(polygon[0][1]*imageToDraw.height*scale));
        for(var i=1; i<=num; ++i){
            ctx.lineTo(Math.floor(polygon[i%num][0]*imageToDraw.width*scale),Math.floor(polygon[i%num][1]*imageToDraw.height*scale));
        }
        ctx.stroke();
        ctx.moveTo(0,0);
    }

    function drawPolygonList(){
        if(!ctx) {print("no context available");return;}
        var num=polygonList.length;
        for(var i=0; i<num; ++i){
            drawPolygon({polygon:polygonList[i]});
        }
        canvasMain.requestPaint();
    }

    function drawImage(param){
        var sx=param["sx"],sy=param["sy"],sw=param["sw"],sh=param["sh"],
                dx=param["dx"],dy=param["dy"],dw=param["dw"],dh=param["dh"];
        if(!ctx) {print("no context available");return;}
        if(sx<0 || sx+sw>imageToDraw.width || sy<0 || sy+sh>imageToDraw.height) {print("source coord out of range"); return;}
        if(dx<0|| dx+dw>canvasMain.canvasSize.width || dy<0 || dy+dh>canvasMain.canvasSize.height) {print("destine coord out of range"); return;}

        ctx.drawImage(imageToDraw,sx,sy,sw,sh,dx,dy,dw,dh);

    }
    function computeDrawImageParam(){
        scale=canvasMain.canvasSize.width/imageToDraw.width<canvasMain.canvasSize.height/imageToDraw.height?canvasMain.canvasSize.width/imageToDraw.width:canvasMain.canvasSize.height/imageToDraw.height;
        drawImageParam={sx:0,sy:0,sw:imageToDraw.width,sh:imageToDraw.height,
        dx:0,dy:0,dw:Math.floor(imageToDraw.width*scale),dh:Math.floor(imageToDraw.height*scale)}
    }

    function setArea(x1,x2,y1,y2){
        x1=x1/imageToDraw.width;
        x2=x2/imageToDraw.width;
        y1=y1/imageToDraw.height;
        y2=y2/imageToDraw.height;
        //print("area set:",x1,x2,y1,y2)
        var polygon=[[x1,y1],[x2,y1],[x2,y2],[x1,y2]];
        polygonList=[polygon];
    }

    function translate(x,y){

    }

    function myPaint() {
        clearCtx();
        drawImage(drawImageParam);
        drawPolygonList();
        canvasMain.requestPaint();
    }

    function loadImage(src){
        canvasMain.unloadImage(imageToDraw);
        imageToDraw.source=src;
        canvasMain.loadImage(imageToDraw);
    }

    function loadData(data){
        canvasMain.computeDrawImageParam();
    }

    function clear(){
        canvasMain.clearCtx();
        canvasMain.unloadImage(imageToDraw);
    }

    function setSize(w,h){
        canvasMain.canvasSize=Qt.size(w,h);
        canvasMain.width=w;
        canvasMain.height=h;
        width=w;
        height=h;
    }

    Connections {
        target: buttonMinus
        onClicked: {
            slider.value = slider.value * 0.9
        }
    }

    Connections {
        target: buttonPlus
        onClicked: {
            slider.value = slider.value * 1.1
        }
    }

    Connections {
        target: slider
        onValueChanged: {
            setSize(flick.width*slider.value/100,flick.width*slider.value/100);
            computeDrawImageParam();
            //pageReview.getContentValue(slider);
            flick.contentWidth =flick.width*slider.value/100
            flick.contentHeight = flick.width*slider.value/100
            myPaint();
        }
    }

}
