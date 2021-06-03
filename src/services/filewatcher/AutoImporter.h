#ifndef AUTOIMPORTER_H
#define AUTOIMPORTER_H

#include <QObject>
#include <QQueue>
#include <QPair>
#include <QMap>
class CopyFileThread;
class RemoveFileThread;
class QTimer;
class FileSystemWatcher;
/**
 * @brief The AutoImporter class    for file importing,use file system watcher and copythread,remove thread
 */
class AutoImporter : public QObject
{
    Q_OBJECT
public:
    explicit AutoImporter(QObject *parent = nullptr);
    ~AutoImporter();

    /**
     * @brief watch_path_dir_prefix
     * @return
     */
    QMap<QString, QString> watch_path_dir_prefix() const;

    /**
     * @brief set_watch_path_key_value
     * @param watch_path_dir_prefix
     */
    void set_watch_path_key_value(const QMap<QString, QString> &watch_path_dir_prefix);

    /**
     * @brief insert_watch_path_dir_prefix
     * @param dir
     * @param prefix
     */
    void insert_watch_path_dir_prefix(QString dir, QString prefix);

    /**
     * @brief remove_watch_path
     * @param path
     */
    void remove_watch_path(QString path);

    QString default_backup_path() const;

    void set_default_backup_path(const QString &default_backup_path);

    void set_rename_entry_max_(int);

    void testRename();

    /**
     * @brief pushPCBImageIntoCopy  try to copy board image from PCBImage dir
     */
    void pushPCBImageIntoCopy();

private:
    QMap<QString,QString>   watch_path_key_value_;
    QString                 default_backup_path_;
    CopyFileThread*         copy_thread_;
    RemoveFileThread*       remove_thread_;
    FileSystemWatcher*      file_watcher_;
    QQueue<QPair<QString, QString>> to_copy_src_dst_queue_;
    QQueue<QString>         to_remove_dst_queue_;
    QQueue<QPair<QString, QString>> to_copy_pcbimage_src_dst_queue_;
    CopyFileThread*         copy_pcbimage_thread_;
    QTimer*     delay_timer_;
    QTimer*     remove_timer_;
    QTimer*     rename_timer_;
    QString     rename_dir_;
    int         rename_dir_entry_max_=1000;
    bool        rename_success_;
    //Temp function for clearing Judge Function
signals:
    void signalFinishCopy(QString dst, bool success, QString prefix);
public slots:

    /**
     * @brief slotActualBeginCopy   normal copy actual begin
     */
    void slotActualBeginCopy();

    /**
     * @brief slotActualBeginRemove remove begin
     */
    void slotActualBeginRemove();

    /**
     * @brief slotActualBeginCopyPCBImage board image copy begin
     */
    void slotActualBeginCopyPCBImage();

    /**
     * @brief slotInsertToCopy  handle copy request and push into task queque
     * @param prefix
     * @param filename
     */
    void slotInsertToCopy(QString prefix,QString filename);


    /**
     * @brief slotInsertPCBToCopy handle board image copy request
     * @param filename
     */
    void slotInsertPCBToCopy(QString filename);

    /**
     * @brief slotHandleCopyPCBImageFinished    exec when board image copy finished
     * @param src                               src path
     * @param dst                               dst path
     * @param success
     */
    void slotHandleCopyPCBImageFinished(QString src, QString dst, bool success);

    /**
     * @brief slotHandleCopyFinished    exec when normal file/dir copy finished
     * @param src
     * @param dst
     * @param success
     */
    void slotHandleCopyFinished(QString src, QString dst, bool success);

    /**
     * @brief slotHandleRemoveFinished  exec when delete finished
     * @param src
     * @param success
     */
    void slotHandleRemoveFinished(QString src,bool success);

    /**
     * @brief slotHandleFileOperationFound  exec when file operation found in watching dir
     * @param dir       dir
     * @param operation operation:add,delete,rename
     * @param suffix    file suffix
     * @param filename  filename
     */
    void slotHandleFileOperationFound(QString dir, QString operation,QString suffix, QString filename);

    /**
     * @brief slotHandleDirNum  max num of file in watching dir,otherwise, cut files into another dir;
     * @param dir
     * @param num
     */
    void slotHandleDirNum(QString dir, int num);

    /**
     * @brief slotRenameAndMakeDir
     */
    void slotRenameAndMakeDir();
};

#endif // AUTOIMPORTER_H
