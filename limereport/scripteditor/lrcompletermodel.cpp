#include "lrcompletermodel.h"
#include <QDebug>

CompleterModel::CompleterModel(QObject *parent) : QAbstractItemModel(parent){m_root.setModel(this);}

QModelIndex CompleterModel::index(int row, int column, const QModelIndex &parent) const
{
    CompleterItem const *parentItem;
    if (!parent.isValid())
        parentItem = &m_root;
    else
        parentItem = static_cast<CompleterItem*>(parent.internalPointer());

    if ((parentItem == nullptr)
        || (row < 0)
        || (column < 0)
        || (row >= parentItem->rowCount())
        || (column >= 1))
    {
        return QModelIndex();
    }
    return createIndex(row, column, parentItem->child(row));
}

QModelIndex CompleterModel::parent(const QModelIndex &child) const
{
    if (child.isValid()){
        CompleterItem *childItem = static_cast<CompleterItem*>(child.internalPointer());
        CompleterItem *parentItem = childItem->parent();
        if (parentItem != &m_root) {
            return indexFromItem(parentItem);
        }
    }
    return QModelIndex();
}

int CompleterModel::rowCount(const QModelIndex &parent) const
{
   if (parent.isValid()){
        CompleterItem *parentItem = static_cast<CompleterItem*>(parent.internalPointer());
        return parentItem->rowCount();
    }
    return m_root.rowCount();
}

int CompleterModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant CompleterModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()){
        CompleterItem* item = static_cast<CompleterItem*>(index.internalPointer());
        switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            if (!item) return QVariant();

            if (index.column()==0){
                return item->text();
            } else {
                return "";
            }
        case Qt::DecorationRole :
            if (!item) return QIcon();
            if (index.column()==0){
                return item->icon();
            } else return QIcon();
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QList<CompleterItem *> CompleterModel::findItems(const QString &text, Qt::MatchFlags flags, int column) const
{
    QModelIndexList indexes = match(index(0, column, QModelIndex()),
                                        Qt::DisplayRole, text, -1, flags);
    QList<CompleterItem*> items;
    const int numIndexes = indexes.size();
    items.reserve(numIndexes);
    for (int i = 0; i < numIndexes; ++i)
        items.append(itemFromIndex(indexes.at(i)));
    return items;
}

void CompleterModel::clear()
{
    beginResetModel();
    m_root.clear();
    endResetModel();
}

CompleterItem *CompleterModel::itemFromIndex(const QModelIndex index) const
{
    if ((index.row() < 0) || (index.column() < 0) || (index.model() != this))
            return nullptr;
    CompleterItem *parent = static_cast<CompleterItem*>(index.internalPointer());
    if (parent == nullptr)
            return nullptr;
    CompleterItem *item = parent->child(index.row());
    return item;
}

QModelIndex CompleterModel::indexFromItem(CompleterItem *item) const
{
    if (item && item->parent()){
        return createIndex(item->row(), 0, item);
    }
    return QModelIndex();
}

CompleterItem::~CompleterItem(){}

void CompleterItem::setIcon(const QIcon &newIcon)
{
    m_icon = newIcon;
}

void CompleterItem::setText(const QString &newText)
{
    m_text = newText;
}

void CompleterItem::appendRow(CompleterItem *child){
    child->m_parent = this;
    child->m_model = this->m_model;
    m_children.append(QSharedPointer<CompleterItem>(child));
    if (m_model){
        QModelIndex start = m_model->indexFromItem(child);
        emit m_model->dataChanged(start, start);
    }
}

void CompleterItem::appendRows(const QList<CompleterItem*> &children){
    foreach(CompleterItem* item, children){
        appendRow(item);
    }
}
