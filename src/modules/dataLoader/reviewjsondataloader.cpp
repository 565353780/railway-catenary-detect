#include "reviewjsondataloader.h"

#include "modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h"
ReviewJsonDataLoader::ReviewJsonDataLoader(RailwayCatenaryDataManager *database_manager, QObject *parent)
    :QObject(parent), database_manager_(database_manager)
{

}

bool ReviewJsonDataLoader::loadReviewJson(QString filename)
{
    bool success=false;
    /*
     *
    foreach(bbox,label_array)
    database_manager_->insertRecord2Review(review_attr);
    */
    return success;

}
