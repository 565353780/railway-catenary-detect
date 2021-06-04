#ifndef REVIEWJSONDATALOADER_H
#define REVIEWJSONDATALOADER_H

#include <QObject>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

class RailwayCatenaryDataManager;

class ReviewJsonDataLoader : public QObject
{
    Q_OBJECT
public:
    explicit ReviewJsonDataLoader(RailwayCatenaryDataManager* database_manager, QObject *parent = nullptr);
private:
    bool insertReview(QString PicID,
                      QString ID,
                      QString Time,
                      QString label,
                      QString x1,
                      QString x2,
                      QString y1,
                      QString y2,
                      QString polygon);

    bool insertReview(QJsonObject insert_Obj);

    QString getFileHash(QString file_path);

    RailwayCatenaryDataManager* database_manager_;

    QDir dir_;

    QStringList label_list_;
signals:

public slots:
    Q_INVOKABLE bool loadReviewJson(QString filename);
};

#endif // REVIEWJSONDATALOADER_H
