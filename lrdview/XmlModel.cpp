#include "XmlModel.h"
#include <QDebug>

XmlModel::XmlModel(QByteArray *data) {
    if (data) {
        m_doc.setContent(*data);
        initModel();
    }
}

void XmlModel::setXMLData(QByteArray *data) {
    if (data) {
        beginResetModel();
        m_doc.setContent(*data);
        initModel();
        endResetModel();
    }
}

void XmlModel::initModel(){
    m_items = m_doc.firstChildElement("items");
    parseHeaders();
}

void XmlModel::parseHeaders()
{
    m_fields.clear();
    QDomNode root = m_doc.firstChildElement("items");
    QDomNode item = root.firstChild();
    for (int i=0; i<item.childNodes().count();++i){
        QDomNode attr = item.childNodes().item(i);
         m_fields.append(attr.nodeName());
    }
}

QModelIndex XmlModel::index(int row, int column, const QModelIndex &parent) const
{
    if (m_fields.isEmpty())
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex XmlModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int XmlModel::rowCount(const QModelIndex &parent) const
{
    return m_items.childNodes().count();
}

int XmlModel::columnCount(const QModelIndex &parent) const
{
    return m_fields.count();
}

QVariant XmlModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.isValid()){
            QDomNode data = m_items.childNodes().at(index.row()).childNodes().item(index.column());
            return data.toElement().text();
        } else return QVariant();
    } else return QVariant();
}

QVariant XmlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal&&role==Qt::DisplayRole){
        return m_fields[section];
    } else return QVariant();
}
