#include "xsqlquerymodel.h"

XSqlQueryModel::XSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
{

}

QVariant XSqlQueryModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::TextAlignmentRole){
            QVariant value = Qt::AlignCenter;
            return value;
        }
        return QSqlQueryModel::data(index,role);
}
