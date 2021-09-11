#ifndef XMLMODEL_H
#define XMLMODEL_H

#include <QAbstractItemModel>
#include <QDomDocument>
#include <QVector>

class XmlModel : public QAbstractItemModel
{
public:
    XmlModel(QByteArray* data = 0);
    void setXMLData(QByteArray* data);
    void parseHeaders();
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
private:
    void initModel();
private:
    QDomDocument m_doc;
    QDomNode m_items;
    QVector<QString> m_fields;
};

#endif // XMLMODEL_H
