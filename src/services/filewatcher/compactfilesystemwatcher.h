#ifndef FILE_SYSTEM_WATCHER_H
#define FILE_SYSTEM_WATCHER_H

#include <QObject>
#include <QMap>
#include <QFileSystemWatcher>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QDebug>



class FileSystemWatcher : public QObject
{
    Q_OBJECT

public:
    explicit FileSystemWatcher(QObject *parent = 0);
    ~FileSystemWatcher();
    void clearPaths();
    void removePath(QString &);
    void addWatchPath(QString path, bool watchfile=false);

    void searchForAllFileHeading(QString heading, QString DirPath);
    void set_file_suffix_filters(const QStringList &suffix_filters);
    void set_dir_suffix_filters(const QStringList &dir_suffixs);

public slots:
    void directoryUpdated(const QString &path);  // 目录更新时调用，path是监控的路径
    void fileUpdated(const QString &path);   // 文件被修改时调用，path是监控的路径

private:
    QFileSystemWatcher *my_system_watcher_=nullptr;  // QFileSystemWatcher变量
    QMap<QString, QStringList> m_currentContentsMap; // 当前每个监控的内容目录列表
    QDateTime       last_modified_time_;

    QStringList     file_suffix_filters_;
    QStringList     dir_suffix_filters_;
    int     file_num_=-1;
signals:
    void fileMoifyInfo(QString);
    void signalXmlModified(QString);
    void signalFileOperationInfo(QString dir, QString operation,QString suffix, QString filename);
    void signalDirFileNum(QString dir, int num);
};

#endif // FILE_SYSTEM_WATCHER_H
