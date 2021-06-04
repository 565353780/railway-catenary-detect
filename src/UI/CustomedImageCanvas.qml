import QtQuick 2.0

import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
Item {
    anchors.fill: parent
    focus: true;
    property var ctx;

    property int currentMarkIndex: -1;
    property point old_mouse_pos: Qt.point(0,0);
    property point translate: Qt.point(0,0);
    property point old_translate: Qt.point(0,0);
    property real image_scale: 1;

    property var markList:[];
    property var temp_mark_list:[];
    property var  drawImageParam: Object();

    property int operate_state: 0; //0:none 1:drag 2:add rect 3: add rect drag
    Canvas{
        id:canvasMain
        anchors.fill: parent
        onImageLoaded: {
            initDrawImageParamWithOriginSize();
            myPaint();
        }
        onAvailableChanged: {
            ctx=getContext("2d");
        }

        onWidthChanged: {
            myPaint();
        }
        onHeightChanged: {
            myPaint();
        }
    }




    MouseArea{
        id:addMarkInfo
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        onReleased: function onMouseReleased(event){
            if(operate_state===1)
            {
                operate_state=0;
            }
            var mouse_pos=Qt.point(event.x,event.y);

            if((mouse_pos.x === old_mouse_pos.x && mouse_pos.y === old_mouse_pos.y))
                operate_state=2;

            if(operate_state===3 && !(mouse_pos.x === old_mouse_pos.x && mouse_pos.y === old_mouse_pos.y)){
                var polygonList = {
                    "Label":" ",
                    "x1":(mouse_pos.x-translate.x)/(imageToDraw.width*image_scale),
                    "y1":(mouse_pos.y-translate.y)/(imageToDraw.height*image_scale),
                    "x2":(old_mouse_pos.x-translate.x)/(imageToDraw.width*image_scale),
                    "y2":(old_mouse_pos.y-translate.y)/(imageToDraw.height*image_scale),
                }
                operate_state=2;
                currentMarkIndex = markList.length;
                ReviewController_Review.addMark(polygonList);
                updateCanvasMark();
                updateIDAndLabel();
                myPaint();
            }
        }

        onPressed: function onPressedEvent(event){
            //print("mouse pressed",event.button)
            if(event.button===Qt.MidButton)
            {
                operate_state=1;
                old_mouse_pos=Qt.point(event.x,event.y);
                old_translate=translate;
            }
            else if(operate_state===2){
                operate_state=3;
                old_mouse_pos=Qt.point(event.x,event.y);
            }
        }

        onPositionChanged: function onMouseMoveEvent(event){
            if(operate_state===1)
            {
                print("old_mouse_pos",old_mouse_pos,"new pos",Qt.point(event.x,event.y));
                translate=Qt.point(old_translate.x+event.x-old_mouse_pos.x,old_translate.y+event.y-old_mouse_pos.y);
                print(translate);
                myPaint();
            }
            else if(operate_state==3){
                var mouse_pos=Qt.point(event.x,event.y);
                var polygon=[[(mouse_pos.x-translate.x)/(imageToDraw.width*image_scale),(mouse_pos.y-translate.y)/(imageToDraw.height*image_scale)],[(mouse_pos.x-translate.x)/(imageToDraw.width*image_scale),(old_mouse_pos.y-translate.y)/(imageToDraw.height*image_scale)],[(old_mouse_pos.x-translate.x)/(imageToDraw.width*image_scale),(old_mouse_pos.y-translate.y)/(imageToDraw.height*image_scale)],[(old_mouse_pos.x-translate.x)/(imageToDraw.width*image_scale),(mouse_pos.y-translate.y)/(imageToDraw.height*image_scale)]];
                print([mouse_pos.x,mouse_pos.y],[mouse_pos.x,old_mouse_pos.y],[old_mouse_pos.x,old_mouse_pos.y],[old_mouse_pos.x,mouse_pos.y]);
                temp_mark_list=[];
                addTempMark(" "," ",polygon,2,Qt.rgba(1,0,0,1),Qt.rgba(1,0,0,1),0);
                clearCtx();
                computeDrawImageParam();
                drawImage(drawImageParam);
                drawMarkList(markList);
                drawMarkList(temp_mark_list);
                canvasMain.requestPaint();
            }
        }

        onWheel:function onWheelEvent(wheel){
            print("wheel delta", wheel.angleDelta ,"scale",image_scale)
            var delta_scale=wheel.angleDelta.y/120*0.1;
            if(image_scale+delta_scale>=0.1 && image_scale+delta_scale<10){
            // (mouseX-translate.x)/oldscale  ==  mouseX-translate.x.new/newscale  mousex-(mousex-translatex.old)*(1+delta/oldscale)=translate.new
                translate.x-=(wheel.x-translate.x)*delta_scale/image_scale;//mouseX-(mouseX-translate.x)*(image_scale+delta_scale)/image_scale;
                translate.y-=(wheel.y-translate.y)*delta_scale/image_scale;//mouseY-(mouseY-translate.y)*(1+delta_scale/image_scale); //(1+delata/old)*translate.y -delta_scale/image_scale*mouseY
                image_scale+=delta_scale;
                myPaint();
                print("current scale",image_scale);
            }
        }
    }

    Image{
        id:imageToDraw
        visible: false;
        fillMode: Image.PreserveAspectFit
        cache:false;
    }

    function clearCtx(){

        if(!ctx) {ctx=canvasMain.getContext("2d");print("no context available");return;}
        ctx.fillStyle=Qt.rgba(0,0,0,1);
        ctx.fillRect(0,0,canvasMain.canvasSize.width,canvasMain.canvasSize.height);
        canvasMain.requestPaint();
    }


    function initDrawImageParamWithOriginSize(){
        image_scale=canvasMain.canvasSize.width/imageToDraw.width<canvasMain.canvasSize.height/imageToDraw.height?canvasMain.canvasSize.width/imageToDraw.width:canvasMain.canvasSize.height/imageToDraw.height;
//        print("origin canvas size",canvasMain.canvasSize,"init image width",imageToDraw.width,image_init_scale*imageToDraw.width,image_init_scale*imageToDraw.height);
        translate=Qt.point(0,0);
        temp_mark_list = [];
//        update_canvas_size=false;
//        slider.value=100*image_init_scale;
//        last_scale=canvas_scale=1;
//        update_canvas_size=true;
    }


    signal mousePosChanged(real x,real y);



    function loadImage(src){
        canvasMain.unloadImage(imageToDraw);
        //print("loading:",src);
        imageToDraw.source=src;
        imageToDraw.update();
        canvasMain.loadImage(imageToDraw);

    }

    function updateCanvasMark(){
        var result_current_json = ReviewController_Review.getImagePredictInfo();
        clearMarkList();
        var label_list = ReviewController_Review.getLabelList();
        for(var i=0; i<result_current_json.length; ++i)
        {
            var x1 = result_current_json[i]["x1"];
            var x2 = result_current_json[i]["x2"];
            var y1 = result_current_json[i]["y1"];
            var y2 = result_current_json[i]["y2"];
            var type =  Number(result_current_json[i]["Label"]);
            var label = label_list[type];
            var ID = result_current_json[i]["ID"];
            if(i < ReviewController_Review.getAccountOfCurrentInfoFromReview())
                addMark(ID,label,[[x1,y1],[x2,y1],[x2,y2],[x1,y2]],2,colorList[type],colorList[type],0);
            else
                addMark(ID,label,[[x1,y1],[x2,y1],[x2,y2],[x1,y2]],2,colorList[type],colorList[type],1);
        }
    }

    function computeDrawImageParam(){
        var w,h;
        w=canvasMain.canvasSize.width; h=canvasMain.canvasSize.height
        var dx1,a_dx1,dx2,a_dx2;
        dx1=translate.x; dx2=dx1+imageToDraw.width*image_scale;
        a_dx1=dx1>0?dx1:0;  a_dx2=dx2<w?dx2:w;
        print("dx1,a_dx1,dx2,a_dx2",dx1,a_dx1,dx2,a_dx2)
        if(a_dx1< w && a_dx2>0 ){
            drawImageParam.dx=a_dx1; drawImageParam.dw=a_dx2-a_dx1;
            drawImageParam.sx=Math.floor((a_dx1-dx1)/image_scale); drawImageParam.sw=Math.floor((a_dx2-a_dx1)/image_scale);
        }
        else{
            drawImageParam.dx=0; drawImageParam.dw=0;
            drawImageParam.sx=0; drawImageParam.sw=0;
        }

        var dy1,a_dy1,dy2,a_dy2;
        dy1=translate.y; dy2=dy1+imageToDraw.height*image_scale;
        a_dy1=dy1>0?dy1:0;  a_dy2=dy2<h?dy2:h;
        if(a_dy1< h && a_dy2>0 ){
            drawImageParam.dy=a_dy1; drawImageParam.dh=a_dy2-a_dy1;
            drawImageParam.sy=Math.floor((a_dy1-dy1)/image_scale); drawImageParam.sh=Math.floor((a_dy2-a_dy1)/image_scale);
        }
        else{
            drawImageParam.dy=0; drawImageParam.dh=0;
            drawImageParam.sy=0; drawImageParam.sh=0;
        }
        print("dy1,a_dy1,dy2,a_dy2",dy1,a_dy1,dy2,a_dy2)

    }

    function drawImage(param){

        var sx=param["sx"],sy=param["sy"],sw=param["sw"],sh=param["sh"],
                dx=param["dx"],dy=param["dy"],dw=param["dw"],dh=param["dh"];
        print("sx,sy,sw,sh,dx,dy,dw,dh",sx,sy,sw,sh,dx,dy,dw,dh)
        if(!ctx) {print("no context available");return;}
        if(sx<0 || sx+sw>imageToDraw.width || sy<0 || sy+sh>imageToDraw.height) {print("source coord out of range"); return;}
        if(dx<0|| dx+dw>canvasMain.canvasSize.width || dy<0 || dy+dh>canvasMain.canvasSize.height) {print("destine coord out of range"); return;}
        ctx.drawImage(imageToDraw,sx,sy,sw,sh,dx,dy,dw,dh);


    }

    function clearMarkList(){
        while(markList.length>0){
            markList.pop()
        }
    }

    function addPloygon(label,x1,y1,x2,y2){
        var param={};
        param["label"]=label;
        param["x1"]=x1;
        param["y1"]=y1;
        param["x2"]=x2;
        param["y2"]=y2;
        polygonList.push(param);
    }

    function getPolygonList(){
        return polygonList;
    }

    function addTempMark(ID,label,polygon,lineWidth,edgeColor,faceColor,textPos){
        var param={};
        param["ID"] = ID;
        param["label"]=label;
        param["polygon"]=polygon;
        param["lineWidth"]=lineWidth||2;
        param["edgeColor"]=edgeColor||Qt.rgba(0,0,1,1);
        param["faceColor"]=faceColor||Qt.rgba(0,0,1,1);
        param["textPos"]=textPos||0;
        temp_mark_list.push(param);

    }

    function addMark(ID,label,polygon,lineWidth,edgeColor,faceColor,textPos){
        var param={};
        param["ID"] = ID;
        param["label"]=label;
        param["polygon"]=polygon;
        param["lineWidth"]=lineWidth||2;
        param["edgeColor"]=edgeColor||Qt.rgba(0,0,1,1);
        param["faceColor"]=faceColor||Qt.rgba(0,0,1,1);
        param["textPos"]=textPos||0;
        markList.push(param);
    }

    function drawMark(param,isCurrent){
        var polygon = param["polygon"],
        lineWidth=param["lineWidth"]||2,
        edgeColor=param["edgeColor"]||Qt.rgba(0,0,1,1),
        faceColor=param["faceColor"]||Qt.rgba(0,0,1,1),
        ID = param["ID"],
        label=param["label"],
        textPos=param["textPos"]||0;


        if(isCurrent)
        {
            edgeColor=Qt.rgba(1,0,0,1);
            faceColor=Qt.rgba(1,0,0,1);
            lineWidth = lineWidth+2;
        }

        if(!ctx) {print("no context available");return;}
        if(polygon.length<2) {print("polygon point less than 2");return;}
        ctx.lineWidth = lineWidth;
        ctx.strokeStyle = edgeColor;
        ctx.fillStyle = faceColor;
        var num=polygon.length;
        ctx.beginPath();

        ctx.moveTo((translate.x+polygon[0][0]*imageToDraw.width*image_scale),(translate.y+polygon[0][1]*imageToDraw.height*image_scale));
        for(var i = 1; i <= num; ++i){
            //print("polygon i",i,polygon[i])
            ctx.lineTo((translate.x+polygon[i%num][0]*imageToDraw.width*image_scale),(translate.y+polygon[i%num][1]*imageToDraw.height*image_scale));
        }
        ctx.stroke();

        var tag = label+ "ID:" + ID;
        var half=Math.floor(polygon.length/2);
        var j=polygon[0][0] < polygon[half][0]?0:half;
        if(textPos===0){
            ctx.fillText(tag,(translate.x+polygon[j][0]*imageToDraw.width*image_scale),(translate.y+polygon[j][1]*imageToDraw.height*image_scale)) ;
        }
        else if(textPos===1){
            j=2-j
            ctx.fillText(tag,(translate.x+polygon[j][0]*imageToDraw.width*image_scale),(translate.y+polygon[j][1]*imageToDraw.height*image_scale)) ;


        }//ctx.fillText(label,Math.floor(polygon[0][0]*canvasMain.width+10),Math.floor(polygon[0][1]*canvasMain.height+10))
        ctx.stroke();
        ctx.moveTo(0,0);
    }

    function drawMarkList(mark_list){
        if(!ctx) {print("no context available");return;}
        var num = mark_list.length;
        for(var i=0; i<num; ++i){
            if(i === currentMarkIndex)
                continue;
            drawMark(mark_list[i],false);
        }
        if(-1<currentMarkIndex && currentMarkIndex <markList.length){
            drawMark(mark_list[currentMarkIndex],true)
        }
        canvasMain.requestPaint();
    }

    function myPaint() {
        clearCtx();
        computeDrawImageParam();
        drawImage(drawImageParam);
        drawMarkList(markList);
        //drawMarkList(temp_mark_list);
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

    function switchEditMode(editable){
        if(editable){
            operate_state=2;
        }
        else{
            operate_state=0;
        }
    }

    function lastMark(){
        currentMarkIndex = (currentMarkIndex + markList.length - 1) % markList.length;
        myPaint();
    }

    function nextMark(){
        currentMarkIndex = (currentMarkIndex + 1) % markList.length;
        myPaint();
    }

    function selectMarkById(ID){
        if(ID > -1 && ID < markList.length)
            currentMarkIndex = ID;
        myPaint();

    }
}
