#ifndef REVIEWJSONDATALOADER_H
#define REVIEWJSONDATALOADER_H

#include <QObject>

class RailwayCatenaryDataManager;

class ReviewJsonDataLoader : public QObject
{
    Q_OBJECT
public:
    explicit ReviewJsonDataLoader(RailwayCatenaryDataManager* database_manager, QObject *parent = nullptr);
private:
    RailwayCatenaryDataManager* database_manager_;
signals:

public slots:
    Q_INVOKABLE bool loadReviewJson(QString filename);
};

#endif // REVIEWJSONDATALOADER_H
