#include "SqlRecordCountTableModel.h"
#include <QDebug>
#include <QString>
SqlRecordCountTableModel::SqlRecordCountTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant SqlRecordCountTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal)
        return column_header_roles_[section];
    else {
        return row_header_roles_[section];
    }
}

int SqlRecordCountTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return data_.size();
}

int SqlRecordCountTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
//    qDebug()<<"column count"<<column_header_roles_.size();
    return column_header_roles_.size();

}

Qt::ItemFlags SqlRecordCountTableModel::flags(const QModelIndex &index) const
{
    if(index.isValid())
        return Qt::ItemIsSelectable|Qt::ItemIsEditable;
    else
        return Qt::NoItemFlags;
}


int SqlRecordCountTableModel::getKey(int role) const
{
    int columnIndex = role - Qt::UserRole-1 ;
    return columnIndex;
}

QHash<int, QByteArray> SqlRecordCountTableModel::roleNames() const
{
    QHash<int, QByteArray> role_names;
    int colNum=column_header_roles_.size();
    for(int j=0; j<colNum; ++j){
        QString value=column_header_roles_.at(j);
        role_names[j+Qt::UserRole+1]=value.toLocal8Bit();
    }
//    qDebug()<<role_names;
    return role_names;
}

QVariant SqlRecordCountTableModel::getData(int row, int column)
{
    return data_[row][column];
}

QVariant SqlRecordCountTableModel::data(const QModelIndex &index, int role) const
{


    if (!index.isValid())
        return QVariant(0);
    int row=index.row(), col=role>Qt::UserRole?role-Qt::UserRole-1:index.column();
    //    qDebug()<<"row,col,role"<<index.row()<<index.column()<<role;
    if(row>= row_header_roles_.size() || col>=column_header_roles_.size())
        return QVariant(0);
    else{
        return data_[row][col];
    }

}

bool SqlRecordCountTableModel::setData(const QModelIndex &index, const QVariant &value, int role=0)
{
    if(!index.isValid() || index.row()>= row_header_roles_.size() || index.column()>=column_header_roles_.size())
        return false;
    data_[index.row()][index.column()]=value;

    return true;
}

bool SqlRecordCountTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    int colNum=column_header_roles_.size();
    QVariantList list;
    for(int j=0; j<colNum; ++j){
        list<<QVariant(0);
    }
    for(int i=row; i<row+count; ++i){
        data_.insert(i,list);
        row_header_roles_.insert(i,QString(""));
//        qDebug()<<"finish inser row i"<<i;
    }
//    qDebug()<<"row_num:"<<data_.size();
    endInsertRows();
    return true;
}

bool SqlRecordCountTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    for(int j=column; j<column+count; ++j){
        column_header_roles_.insert(j,QString(""));
//        qDebug()<<"finish insert column i"<< j;
    }
//    qDebug()<<"columnum:"<<column_header_roles_.size();
    int num= data_.size();
    for(int i=0; i<num; ++i){
        for(int j=column; j<column+count; ++j){
            data_[i].insert(j,QVariant(0));
        }
    }
    endInsertColumns();
    return true;
}

bool SqlRecordCountTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    for(int i=row+count-1; i>row-1; --i){
        data_.removeAt(i);
        row_index_map_.remove(row_header_roles_[i]);
        row_header_roles_.removeAt(i);
    }
    endRemoveRows();

    return true;
}

bool SqlRecordCountTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    int num=data_.size();
    for(int i=0; i<num; ++i){
        for(int j=column+count-1; j>column-1; --j){
            data_[i].removeAt(j);
        }
    }
    for(int j=column+count-1; j>column-1; --j){

        column_index_map_.remove(column_header_roles_[j]);
        column_header_roles_.removeAt(j);
    }
    // FIXME: Implement me!
    endRemoveColumns();

    return true;
}

bool SqlRecordCountTableModel::appendRow(QString row_role, QVariantList record, bool from_inner)
{
    int num=data_.size();
    if(!insertRow(num))
        return false;

    if(!setRow(num,row_role,record,from_inner))
        return false;
    return true;
}

bool SqlRecordCountTableModel::appendRow(QString row_role, QJsonArray record, bool from_inner)
{
    return appendRow(row_role,record.toVariantList(),from_inner);
}

bool SqlRecordCountTableModel::setRow(int row, QString row_role, QVariantList record, bool from_inner)
{
    if(row<0 || row>=data_.size() || record.size()!=column_index_map_.size())
        return false;
    row_index_map_[row_role]=row;
    row_header_roles_[row]=row_role;
    int colNum=column_index_map_.size();
    for(int j=0; j<colNum; ++j){
        setData(index(row,j),record[j],0);
    }
    if(!from_inner)
        emit dataChanged(this->index(row,0),this->index(row,0));
    return true;
}

bool SqlRecordCountTableModel::setRow(int row, QString row_role, QSqlRecord record)
{
    if(row<0 || row>=data_.size())
        return false;
    row_index_map_[row_role]=row;
    row_header_roles_[row]=row_role;
    int colNum=column_index_map_.size();
    for(int j=0; j<colNum; ++j){
        if(record.contains(column_header_roles_[j]))
            setData(index(row,j),record.value(column_header_roles_[j]),0);
    }
    emit dataChanged(this->index(row,0),this->index(row,colNum));
    return true;
}

bool SqlRecordCountTableModel::setColumn(int column, QString col_role)
{
    if(column<0 || column>=column_header_roles_.size() )
        return false;
    column_header_roles_[column]=col_role;
    column_index_map_[col_role]=column;
    return true;
}

bool SqlRecordCountTableModel::setColumnData(int column, QStringList dataList)
{
    if(column<0 || column>=column_header_roles_.size() )
        return false;
    int row_num=data_.size();
    for(int i=0; i<row_num; ++i){
        data_[i][column]=QVariant(dataList[i]);
    }
    emit dataChanged(index(0,column),index(row_num-1,column));
    return true;
}

bool SqlRecordCountTableModel::appendColumn(QString col_role){
    int colnum=column_header_roles_.size();
    if(!insertColumn(colnum))
        return false;
    if(!setColumn(colnum,col_role))
        return false;
    return true;
}

void SqlRecordCountTableModel::clear()
{
    clearRows();
    column_header_roles_.clear();
    column_index_map_.clear();
    row_index_map_.clear();
    row_header_roles_.clear();
    data_.clear();
}

void SqlRecordCountTableModel::clearRows()
{
    int num=rowCount();
    if(num>0){
        this->removeRows(0,num);
        emit dataChanged(this->index(0,0),this->index(0,0));
    }
}

void SqlRecordCountTableModel::initWithColAndRowRoles(QStringList row_roles, QStringList col_roles)
{
    column_header_roles_.clear();
    column_index_map_.clear();
    row_index_map_.clear();
    row_header_roles_.clear();
    int num=data_.size();
    data_.clear();
    if(num>0)
        emit dataChanged(this->index(0,0),this->index(0,0));
    int colnum=col_roles.size();
    int row_num=row_roles.size();
    QVariantList list;
    for(int j=0; j<colnum; ++j){
        appendColumn(col_roles[j]);
        list<<QVariant(0);
    }

    for(int i=0; i<row_num; i++){
        appendRow(row_roles[i],list,true);
    }
}

void SqlRecordCountTableModel::initWithColAndRowRoles(QStringList row_roles, QJsonArray col_roles)
{
    QStringList col_stringList;
    int num=col_roles.size();
    for(int j=0; j<num; ++j){
        col_stringList<<col_roles.at(j).toString();
    }
    initWithColAndRowRoles(row_roles,col_stringList);
}

void SqlRecordCountTableModel::addToRole(QString row_role, QString column_role)
{
    int i=row_index_map_.value(row_role,-1);
    int j=column_index_map_.value(column_role,-1);
    if(i>-1 && j>-1){
        setData(index(i,j),QVariant(data(index(i,j)).toInt()+1));
    }
}

void SqlRecordCountTableModel::addToRole(int row, int column)
{
    if(row<data_.size() && column<column_header_roles_.size()){
        setData(index(row,column),QVariant(data(index(row,column)).toInt()+1));
    }
}

void SqlRecordCountTableModel::printColMap()
{
    qDebug()<<column_index_map_;
}
