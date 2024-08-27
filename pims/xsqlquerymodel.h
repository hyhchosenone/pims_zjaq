#ifndef XSQLQUERYMODEL_H
#define XSQLQUERYMODEL_H

#include <QSqlQueryModel>

class XSqlQueryModel : public QSqlQueryModel
{
public:
    explicit XSqlQueryModel(QObject *parent = nullptr);

public:
    QVariant data(const QModelIndex &index, int role) const override;
};

#endif // XSQLQUERYMODEL_H
