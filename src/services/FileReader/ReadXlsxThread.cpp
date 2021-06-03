#include "ReadXlsxThread.h"

#include "ServerMainContorller.h"

//#include <QtXlsx>
//#include <xlsxdocument.h>
//#include <xlsxchartsheet.h>
//#include <xlsxcellrange.h>
//#include <xlsxchart.h>

ReadXlsxThread::ReadXlsxThread()
{
    start_record_=0;
}

void ReadXlsxThread::setStart_record(int start_record)
{
    start_record_ = start_record;
}

void ReadXlsxThread::setXlsx_filename(const QString &xlsx_filename)
{
    xlsx_filename_ = xlsx_filename;
}

void ReadXlsxThread::setDb(QSqlDatabase *db)
{
    db_ = db;
}

void ReadXlsxThread::setSave_image_path(const QString &save_image_path)
{
    save_image_path_ = save_image_path;
}

void ReadXlsxThread::setMy_controller(ServerMainContorller *my_controller)
{
    my_controller_ = my_controller;
}

void ReadXlsxThread::run()
{
//    QXlsx::Document xlsx(xlsx_filename_);
//    QDir dir;
//    dir.mkdir(save_image_path_);
//    xlsx.selectSheet(xlsx.sheetNames()[0]);
//    QXlsx::Worksheet* sheet=xlsx.currentWorksheet();
//    sheet->writeMediaPictures(save_image_path_,start_record_);
//    qDebug()<<"finish export images";
}
