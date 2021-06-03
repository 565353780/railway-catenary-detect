#ifndef SQLRECORDCOUNTTABLEMODEL_H
#define SQLRECORDCOUNTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QSqlRecord>
#include <QJsonArray>

class SqlRecordCountTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SqlRecordCountTableModel(QObject *parent = nullptr);

    // Header:
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE Qt::ItemFlags flags(const QModelIndex &index)  const;

    int getKey(int role) const;
    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE QVariant getData(int row, int column);
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role);
    // Add data:
    Q_INVOKABLE bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    bool appendRow(QString row_role, QVariantList record, bool from_inner=false);
    bool appendRow(QString row_role, QJsonArray record, bool from_inner=false);
    bool setRow(int row, QString row_role, QVariantList record, bool from_inner=false);
    bool setRow(int row, QString row_role, QSqlRecord record);

    bool setColumn(int column, QString col_role);
    bool setColumnData(int column, QStringList dataList);
    bool appendColumn(QString col_role);

    Q_INVOKABLE void clear();

    Q_INVOKABLE void clearRows();
    Q_INVOKABLE void initWithColAndRowRoles(QStringList row_roles, QStringList col_roles);
    Q_INVOKABLE void initWithColAndRowRoles(QStringList row_roles, QJsonArray col_roles);
    void addToRole(QString row_role, QString column_role );
    void addToRole(int row, int column);
    void printColMap();
signals:

private:
    QList<QVariantList> data_;
    QStringList         column_header_roles_;
    QStringList         row_header_roles_;
    QMap<QString, int>  row_index_map_;
    QMap<QString, int>  column_index_map_;
};

#endif // SQLRECORDCOUNTTABLEMODEL_H
