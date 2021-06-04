#ifndef CATENARYPICDATALOADER_H
#define CATENARYPICDATALOADER_H
#include <QObject>

#include "modules/dataLoader/reviewjsondataloader.h"

class RailwayCatenaryDataManager;
class CatenaryPicDataLoader : public QObject
{
    Q_OBJECT
public:
    explicit CatenaryPicDataLoader(RailwayCatenaryDataManager *database_manager, QObject *parent = nullptr);

    ~CatenaryPicDataLoader();
private:
    RailwayCatenaryDataManager *database_manager_;

    ReviewJsonDataLoader *review_json_dataloader_;

signals:

public slots:
    Q_INVOKABLE bool loadPic(QString filePath);

    Q_INVOKABLE bool loadPicsFromDir(QString dirPath);
};

#endif // CATENARYPICDATALOADER_H
