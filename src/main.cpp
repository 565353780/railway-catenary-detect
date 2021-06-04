#include <QDir>
#include <QQmlContext>
#include <QtWebEngine/QtWebEngine>
#include <QTranslator>
#include <QMutex>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QThread>

#include "singleapplication.h"
#include "modules/statistics/statisticcontroller.h"
#include "services/SqlTable/SqlRecordCountTableModel.h"

#include "modules/detect/objectdetect.h"
#include "modules/review/reviewcontroller.h"
#include "modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h"
#include "modules/dataLoader/catenarypicdataloader.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    SingleApplication app(argc, argv, "railway-catenary-detect");
    QTranslator trans;
    trans.load(":/railway-catenary-detect.qm");
    app.installTranslator(&trans);
    QDir::setCurrent(QApplication::applicationDirPath());

    if (app.isRunning())
    {
        app.sendMessage("message from other instance.");
        return 0;
    }


    QtWebEngine::initialize();

    QQmlApplicationEngine engine;



    RailwayCatenaryDataManager datamanager;
    //datamanager.insertStandardFromJson("configs/2C_Standard.json");
    datamanager.loadConfig("configs/railway_config.json");
    datamanager.initDatabaseFromConfig();
    // statisticController
    CatenaryPicDataLoader pic_data_loader(&datamanager);
    ReviewController review_controller_predict(0,&datamanager);
    ReviewController review_controller_review(0,&datamanager,false,false);
    StatisticController statistic_controller(0,&datamanager);
    //statistic_controller.loadConfig("configs/label_config.json");
    //statistic_controller.loadConfig("configs/4C_U-shaped_config.json", "Pic_4C");
    //statistic_controller.loadConfig("configs/2C_Standard.json", "Pic_2C");


    engine.rootContext()->setContextProperty("ReviewController_Predict",&review_controller_predict);
    engine.rootContext()->setContextProperty("ReviewController_Review",&review_controller_review);
    engine.rootContext()->setContextProperty("StatisticController", &statistic_controller);
    engine.rootContext()->setContextProperty("TableLabelModel", statistic_controller.getTableLabelData());
    engine.rootContext()->setContextProperty("PicDataLoader",&pic_data_loader);

    engine.load(QUrl(QStringLiteral("qrc:/UI/railway-detect.qml")));

    if (engine.rootObjects().isEmpty()) return -1;

    return app.exec();
}
