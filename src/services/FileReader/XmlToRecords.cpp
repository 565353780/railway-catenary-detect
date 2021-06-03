#include "XmlToRecords.h"
#include <QFile>
#include <QJsonObject>
#include <QDebug>
XmlToRecords::XmlToRecords()
{

}

bool lessInKey(QMap<QString,QString> v1, QMap<QString,QString> v2){
    int result=v1["RefNo"].compare(v2["RefNo"]);
    return (result<0);
}
QList<QMap<QString,QString>> XmlToRecords::readFile(QString filename)
{
    QList<QDomNode> target_node_list;
    QList<QMap<QString,QString>> target_attr_list;
    QDomDocument doc("mydocument");
    QFile file(filename);
//    qDebug()<<"reading file:"<<filename;
    if (!file.open(QIODevice::ReadOnly))
        return target_attr_list;
    if (!doc.setContent(&file)) {
        file.close();
        return target_attr_list;;
    }
    file.close();
    doc_elem_ = doc.documentElement();

    findTargetNodesByTagPath(target_node_list, target_tag_);
    int num=target_node_list.size();
    for(int i=0; i<num; ++i){
        QMap<QString,QString> node_attr_;
        QDomNode node=target_node_list.at(i);
        foreach (QString key, key_attr_.keys()) {
            QList<QDomNode> attr_node=nodePath(node,key);
            int node_num=attr_node.count();
            for(int j=0; j<node_num;++j) {
                appendAttr(attr_node.at(j),key_attr_.value(key).toObject(),node_attr_);
            }
        }
        target_attr_list.push_back(node_attr_);
    }

    num=target_attr_list.size();
//    for(int i=0; i<num; ++i){
//        target_attr_list[i]["InspPartNum"]=QString::number((target_attr_list[i]["InspPartNum"]).toDouble()/num);
//        //qDebug()<<target_attr_list[i]["InspPartNum"];
//    }
    qSort(target_attr_list.begin(), target_attr_list.end(), &lessInKey);
//    qDebug()<<target_attr_list;
    return target_attr_list;
}

void XmlToRecords::loadConfig(const QJsonObject &parse_config_obj)
{
    parse_config_obj_=parse_config_obj;
    if(parse_config_obj_.contains("targetTag")){
        target_tag_=parse_config_obj_.value("targetTag").toString();
    }
    if(parse_config_obj_.contains(("keyAttr"))){
        key_attr_=parse_config_obj_.value("keyAttr").toObject();
        //qDebug()<<"key attr:"<<key_attr_;
    }
    if(parse_config_obj.contains("sortKey")){
        sort_field_array_=parse_config_obj.value("sortKey").toArray();
    }else{
        sort_field_array_=QJsonArray();
    }
}

QList<QDomNode> XmlToRecords::nodePath(QDomNode node, QString Path)
{
    QStringList pathList=Path.split('/');
    QList<QDomNode> result;
    result<<node;
    int num=pathList.size();
    for (int i=0; i<num; ++i) {
        QString Tag=pathList[i];

        if(Tag.isEmpty()) continue;
        if(Tag.compare("Parent")==0){
            QList<QDomNode> tmp_result;
            foreach (QDomNode tmp_node, result) {
                tmp_result.push_back(tmp_node.parentNode());
            }
            result=tmp_result;
        }
        else if(Tag.compare("PWD")==0){

        }
        else{

            QList<QDomNode> tmp_result;
            foreach (QDomNode tmp_node, result) {
                QDomNodeList tmp_list=tmp_node.toElement().elementsByTagName(Tag);
                int tmp_num=tmp_list.count();
                for(int j=0; j<tmp_num; ++j){
                    tmp_result.push_back(tmp_list.at(j));
                }
            }
            result=tmp_result;
        }
    }
    return result;
}

void XmlToRecords::appendAttr(const QDomNode &node, QJsonObject attr_field, QMap<QString, QString> &attr_list_)
{
    auto node_attr_elem=node.attributes();
    foreach (QString key, attr_field.keys()) {
        QString final_key=attr_field.value(key).toString();
        QString value=node_attr_elem.namedItem(key).nodeValue();

        if(attr_list_.contains(final_key)){
            attr_list_.insert(final_key,attr_list_.value(final_key,"")+";"+value);
        }
        else{
            attr_list_.insert(final_key,value);
        }

    }
}

void XmlToRecords::findTargetNodesByTagPath(QList<QDomNode> &list, QString Path)
{
    QStringList pathList=Path.split('/');
    list.push_back(doc_elem_);
    foreach (QString tag, pathList) {
        if(tag.isEmpty()) continue;
        QList<QDomNode> new_list;
        foreach (QDomNode node, list) {
            QDomNodeList tmp_list=node.toElement().elementsByTagName(tag);
            for(int i=0; i<tmp_list.size(); ++i){
                new_list.push_back(tmp_list.at(i));
            }
        }
        list.clear();
        list=new_list;
    }
}

bool XmlToRecords::lessBySortField(QMap<QString, QString> &attr_0, QMap<QString, QString> &attr_1)
{
    int num=sort_field_array_.size();
    bool result=false;
    for(int i=0; i<num; ++i){
        QString v0=attr_0[sort_field_array_[i].toString()],v1=attr_1[sort_field_array_[i].toString()];
        if(!v0.isEmpty() && !v1.isEmpty()){
            if(v0.compare(v1)<0){
                break;
                result=true;
            }
        }
    }
    return result;
}
