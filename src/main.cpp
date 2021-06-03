#include <QDir>
#include <QQmlContext>
#include <QtWebEngine/QtWebEngine>
#include <QTranslator>
#include <QMutex>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>

#include "singleapplication.h"
#include "modules/statistics/statisticcontroller.h"
#include "services/SqlTable/SqlRecordCountTableModel.h"

#include "modules/detect/objectdetect.h"
#include "modules/review/reviewcontroller.h"
#include "modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h"
#include "modules/dataLoader/catenarypicdataloader.h"
#include "modules/dataLoader/reviewjsondataloader.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    SingleApplication app(argc, argv, "railway-catenary-detect");
    QTranslator trans;
    trans.load(":/railway-catenary-detect.qm");
    app.installTranslator(&trans);
    QDir::setCurrent(QApplication::applicationDirPath());

    ObjectDetect object_detect;

    QJsonObject img_Obj;

    img_Obj.insert("imagepath", "Z:/chLi/tiedaobu/2C_mask/valid_dataset/0.jpg");
    img_Obj.insert("PicID", "ASUIdjAOS");

    object_detect.slotBeginPredict(img_Obj);
    object_detect.slotBeginPredict(img_Obj);
    object_detect.slotBeginPredict(img_Obj);
    object_detect.slotBeginPredict(img_Obj);
    object_detect.slotBeginPredict(img_Obj);

    if (app.isRunning())
    {
        app.sendMessage("message from other instance.");
        return 0;
    }


    QtWebEngine::initialize();



    QQmlApplicationEngine engine;



    RailwayCatenaryDataManager datamanager;
    datamanager.loadConfig("configs/railway_config.json");
    datamanager.initDatabaseFromConfig();
    // statisticController
    CatenaryPicDataLoader pic_data_loader(&datamanager);
    ReviewController review_controller(0,&datamanager);
    StatisticController statistic_controller(0,&datamanager);





    engine.rootContext()->setContextProperty("ReviewController",&review_controller);
    engine.rootContext()->setContextProperty("StatisticController", &statistic_controller);
    engine.rootContext()->setContextProperty("TableLabelModel", statistic_controller.getTableLabelData());
    engine.rootContext()->setContextProperty("PicDataLoader",&pic_data_loader);

    engine.load(QUrl(QStringLiteral("qrc:/UI/railway-detect.qml")));

    if (engine.rootObjects().isEmpty()) return -1;
    return app.exec();
}
