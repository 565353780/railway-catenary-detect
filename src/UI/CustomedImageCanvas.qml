import QtQuick 2.0

import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
Item {
    anchors.fill: parent
    Flickable {
        id: flick
        x: 20
        y: 20
        z: 1
        width: parent.width - 100
        height: parent.height - 40
        clip: true
        Rectangle{
            anchors.fill: parent
            Rectangle{
                anchors.fill: parent
                Canvas{
                    id:canvasMain
                    onImageLoaded: {
                        myPaint();
                    }
                    onAvailableChanged: {
                        ctx=getContext("2d");
                    }
                }
            }


        }
        Component.onCompleted: {
            flick.contentHeight=flick.height
            flick.contentWidth=flick.width
            canvasMain.canvasSize=Qt.size(flick.width,flick.height)
        }
    }
    Image{
        id:imageToDraw
        visible: false;
        fillMode: Image.PreserveAspectFit
    }

    Rectangle {
        id:scoll01
        anchors.right: flick.right
        width: 8
        z:2
        height: flick.visibleArea.heightRatio * (parent.height-80)
        y: flick.visibleArea.yPosition * (parent.height-80)
        color: "gray"
        radius: 4
    }

    Rectangle {
        id:scoll02
        z:2
        anchors.bottom: parent.bottom
        height: 8
        width: flick.visibleArea.widthRatio * (parent.width - 100)
        x: flick.visibleArea.xPosition * (parent.width -100)
        color: "gray"
        radius: 4
    }

    MouseArea{
        id:mouse
        hoverEnabled: true
        z:-1
        anchors.bottom:  flick.bottom
        anchors.horizontalCenter: flick.horizontalCenter
        width:flick.width
        height: 100
        onEntered: {
            if(buttonAndSliderHide)
            {
                slider.visible = true;
                buttonMinus.visible = true;
                buttonPlus.visible = true;
            }
        }

        onExited: {
            if( (mouseY > 80 || mouseY < 20) && buttonAndSliderHide)
            //if(!mouse.containsMouse && (mouseY > 80 || mouseY < 20))
            {
                slider.visible = false;
                buttonMinus.visible = false;
                buttonPlus.visible = false;
            }
        }

    }

    Rectangle {
        id: buttonMinus
        width: 66
        height: 36
        z:2
        visible: !buttonAndSliderHide
        anchors.left: flick.left
        anchors.bottom: flick.bottom
        anchors.bottomMargin: 30
        BorderImage {
            id: minusImage
            source: "resource/minus.png"
            width: 36
            height: 36
        }
        color: (mouseMinus.hovered |mouseMinus.pressed) ? "#80ee666e" : "#00ffffff"
        MouseArea{
            id:mouseMinus
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: {
                slider.value = slider.value * 0.9;
            }
        }
    }

    Slider {
        id: slider
        z:2
        anchors.left: buttonMinus.right
        anchors.right: buttonPlus.left
        anchors.bottom: flick.bottom
        anchors.bottomMargin: 30
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        height: 50
        from: 1
        to: 200
        visible: !buttonAndSliderHide
        value: 100

    }

    Rectangle{
        id: buttonPlus
        width: 66
        height: 36
        z:2
        visible: !buttonAndSliderHide
        anchors.right: flick.right
        anchors.bottom: flick.bottom
        anchors.bottomMargin: 30
        BorderImage {
            id: plusImage
            source: "resource/plus.png"
            width: 36
            height: 36
        }
        color: (mousePlus.hovered | mousePlus.pressed) ? "#80ee666e" : "#00ffffff"
        MouseArea{
            id:mousePlus
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: {
                slider.value = slider.value * 1.1;
            }

        }
    }

    property real type : 0;
    property bool scollHide : true;
    property bool buttonAndSliderHide : true;
    property var polyAxes;
    property var ctx;
    property real scale: 1;
    property var translate: Object();
    property var markList:[];
    property var  drawImageParam: Object();
    property var  drawImageRateParam: Object();
    property real  contentWidth: flick.width
    property real  contentHeight: flick.height



    function clearCtx(){

        if(!ctx) {ctx=canvasMain.getContext("2d");print("no context available");return;}
        ctx.fillStyle=Qt.rgba(1,1,1,1);
        ctx.fillRect(0,0,canvasMain.canvasSize.width,canvasMain.canvasSize.height);
        canvasMain.requestPaint()
    }


    function initDrawImageRateParam(){
        drawImageRateParam={
            sxr:0,
            syr:0,
            swr:1,
            shr:1,
            dxr:0,
            dyr:0,
            dwr:1,
            dhr:1
        }
    }

    function initDrawImageRateParamWithOriginSize(){
        var image_scale=canvasMain.canvasSize.width/imageToDraw.width<canvasMain.canvasSize.height/imageToDraw.height?canvasMain.canvasSize.width/imageToDraw.width:canvasMain.canvasSize.height/imageToDraw.height;
        drawImageRateParam={
            sxr:0,
            syr:0,
            swr:1,
            shr:1,
            dxr:0,
            dyr:0,
            dwr:image_scale*imageToDraw.width/canvasMain.canvasSize.width,
            dhr:image_scale*imageToDraw.height/canvasMain.canvasSize.height
        }
    }


    function setDrawImageRateParam(sxr,syr,swr,shr,dxr,dyr,dwr,dhr){
        drawImageRateParam={
            sxr:sxr,
            syr:syr,
            swr:swr,
            shr:shr,
            dxr:dxr,
            dyr:dyr,
            dwr:dwr,
            dhr:dhr
        }
    }



    function loadImage(src){
        canvasMain.unloadImage(imageToDraw);
        imageToDraw.source=src;
        imageToDraw.update();
        //initDrawImageRateParam();
        canvasMain.loadImage(imageToDraw);

    }

    function computeDrawImageParam(){
        drawImageParam={
            sx:drawImageRateParam["sxr"]*imageToDraw.width,
            sy:drawImageRateParam["syr"]*imageToDraw.height,
            sw:drawImageRateParam["swr"]*imageToDraw.width,
            sh:drawImageRateParam["shr"]*imageToDraw.height,
            dx:drawImageRateParam["dxr"]*canvasMain.canvasSize.width,
            dy:drawImageRateParam["dyr"]*canvasMain.canvasSize.height,
            dw:drawImageRateParam["dwr"]*canvasMain.canvasSize.width,
            dh:drawImageRateParam["dhr"]*canvasMain.canvasSize.height
        }
    }

    function drawImage(param){
        var sx=param["sx"],sy=param["sy"],sw=param["sw"],sh=param["sh"],
                dx=param["dx"],dy=param["dy"],dw=param["dw"],dh=param["dh"];
        if(!ctx) {print("no context available");return;}
        if(sx<0 || sx+sw>imageToDraw.width || sy<0 || sy+sh>imageToDraw.height) {print("source coord out of range"); return;}
        if(dx<0|| dx+dw>canvasMain.canvasSize.width || dy<0 || dy+dh>canvasMain.canvasSize.height) {print("destine coord out of range"); return;}
        ctx.drawImage(imageToDraw,sx,sy,sw,sh,dx,dy,dw,dh);
        print("sx,sy,sw,sh,dx,dy,dw,dh",sx,sy,sw,sh,dx,dy,dw,dh)

    }



    function clearMarkList(){
        while(markList.length>0){
            markList.pop()
        }
    }

    function addMark(label,polygon,lineWidth,edgeColor,faceColor){
        var param={};
        param["label"]=label;
        param["polygon"]=polygon;
        param["lineWidth"]=lineWidth;
        param["edgeColor"]=edgeColor;
        param["faceColor"]=faceColor;

        markList.push(param);
    }

    function drawMark(param){
        var polygon = param["polygon"],
        lineWidth=param["lineWidth"]||2,
        edgeColor=param["edgeColor"]||Qt.rgba(1,0,0,1),
        faceColor=param["faceColor"]||Qt.rgba(1,1,1,0),
        label=param["label"]||"";
        if(!ctx) {print("no context available");return;}
        if(polygon.length<2) {print("polygon point less than 2");return;}
        ctx.lineWidth = lineWidth;
        ctx.strokeStyle = edgeColor;
        ctx.fillStyle = faceColor;
        var num=polygon.length;
        ctx.beginPath();
        ctx.moveTo(Math.floor(polygon[0][0]*canvasMain.width),Math.floor(polygon[0][1]*canvasMain.height));
        for(var i = 1; i <= num; ++i){
            ctx.lineTo(Math.floor(polygon[i%num][0]*canvasMain.width),Math.floor(polygon[i%num][1]*canvasMain.height));
        }
        ctx.stroke();
        ctx.fillText(label,Math.floor(polygon[0][0]*canvasMain.width),Math.floor(polygon[0][1]*canvasMain.height))
        ctx.stroke();
        ctx.moveTo(0,0);
    }

    function drawMarkList(){
        if(!ctx) {print("no context available");return;}
        var num=markList.length;
        for(var i=0; i<num; ++i){
            drawMark(markList[i]);
        }
        canvasMain.requestPaint();
    }





    function myPaint() {
        clearCtx();
        computeDrawImageParam();
        drawImage(drawImageParam);
        drawMarkList();
        canvasMain.requestPaint();
    }


    function setSize(w,h){
        canvasMain.canvasSize=Qt.size(w,h);
        canvasMain.width=w;
        canvasMain.height=h;
        width=w;
        height=h;
    }


    function sliderHide(){
        if(slider.value < 100 && scollHide)
        {
            scoll01.visible = false;
            scoll02.visible = false;
        }
        if(slider.value > 100 && scollHide)
        {
            scoll01.visible = true;
            scoll02.visible = true;
        }
    }

    Connections {
        target: slider
        onValueChanged: {

            scale=slider.value/100;

            canvasMain.width=flick.contentWidth = flick.width*scale;
            canvasMain.height=flick.contentHeight = flick.height*scale;
            canvasMain.canvasSize=Qt.size(flick.contentWidth,flick.contentHeight)
            clearCtx();
            myPaint();

            sliderHide();

        }
    }
}
