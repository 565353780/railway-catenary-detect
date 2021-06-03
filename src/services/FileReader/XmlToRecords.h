#ifndef XMLTORECORDS_H
#define XMLTORECORDS_H

#include <QtXml>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QJsonArray>
/**
 * @brief The XmlToRecords class reading xml file to qmap accroding to config
 */
class XmlToRecords
{
public:
    XmlToRecords();
    /**
     * @brief readFile  read the xml file in filename
     * @param filename
     * @return  QList<QMap>
     */
    QList<QMap<QString, QString> > readFile(QString filename);

    /**
     * @brief loadConfig
     * @param config_json
     */
    void loadConfig(const QJsonObject &config_json);

    /**
     * @brief getChildAttr get to chlid element, unused
     * @param parent_attr
     * @param node
     * @param tag
     * @param field_name
     */
    void getChildAttr(QMap<QString, QString> &parent_attr, QDomNode &node,QString tag, QJsonObject field_name);

    /**
     * @brief nodePath  get domnodes matches the path
     * @param node      XML dom node
     * @param Path      key,key,key
     * @return QList<QDomNode>
     */
    QList<QDomNode> nodePath(QDomNode node, QString Path);

    /**
     * @brief appendAttr    append current node's attribute key value to attr_list_
     * @param node          node
     * @param attr_field    to visit attributes
     * @param attr_list_    output
     */
    void appendAttr(const QDomNode &node, QJsonObject attr_field, QMap<QString, QString> &attr_list_);

    /**
     * @brief findTargetNodesByTagPath  get nodes with the target tag
     * @param list      output
     * @param Path      key,key
     *
     */
    void findTargetNodesByTagPath(QList<QDomNode> &list, QString Path);

    /**
     * @brief lessBySortField   for sorting records
     * @param attr_0
     * @param attr_1
     * @return attr_0<attr_1
     */
    bool lessBySortField(QMap<QString, QString> &attr_0, QMap<QString, QString> &attr_1);
private:
    QXmlStreamReader* my_reader_;
    QSqlDatabase* db_;
    QJsonObject parse_config_obj_;
    QString     target_tag_;
    QJsonObject key_attr_;
    QJsonArray  sort_field_array_;
    QDomElement doc_elem_;
};

#endif // XMLTORECORDS_H
