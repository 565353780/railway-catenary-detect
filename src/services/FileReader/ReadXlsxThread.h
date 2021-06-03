#ifndef READXLSXTHREAD_H
#define READXLSXTHREAD_H

#include <QThread>
#include <QSqlDatabase>

class ServerMainContorller;
/**
 * @brief The ReadXlsxThread class for reading xlsx ,unused
 */
class ReadXlsxThread :public QThread
{
    Q_OBJECT
public:
    ReadXlsxThread();

    ServerMainContorller* my_controller_;
    QString save_image_path_;
    QString xlsx_filename_;
    int start_record_;
    void run();
    void setMy_controller(ServerMainContorller *my_controller);
    void setSave_image_path(const QString &save_image_path);
    void setXlsx_filename(const QString &xlsx_filename);
    void setDb(QSqlDatabase *db);

    void setStart_record(int start_record);

private:
    QSqlDatabase* db_;
};

#endif // READXLSXTHREAD_H
