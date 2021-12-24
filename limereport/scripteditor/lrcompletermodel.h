#ifndef COMPLETERMODEL_H
#define COMPLETERMODEL_H

#include <QAbstractItemModel>
#include <QIcon>

class CompleterModel;

class CompleterItem {
public:
    CompleterItem():m_parent(0), m_model(0){}
    CompleterItem(QString text, QIcon icon):m_parent(0), m_text(text), m_icon(icon), m_model(0){}
    ~CompleterItem();
    int rowCount() const {return m_children.count();}
    CompleterItem* child(int row) const {return m_children.at(row).data();}
    CompleterItem* parent() const {return m_parent;}
    int row() const{
        if (m_parent){
            for(int i=0; i <  m_parent->m_children.count(); ++i){
                CompleterItem* c = m_parent->m_children.at(i).data();
                if (c == this) return i;
            }
        }
        return -1;
    }
    QString text(){return m_text;}
    QIcon icon() {return m_icon;}

    void setIcon(const QIcon &newIcon);
    void setText(const QString &newText);
    void setModel(CompleterModel* model){m_model=model;}
    void clear(){m_children.clear();}
    void appendRow(CompleterItem* child);
    void appendRows(const QList<CompleterItem *> &children);
private:
    CompleterItem* m_parent;
    QVector<QSharedPointer<CompleterItem> > m_children;
    QString m_text;
    QIcon m_icon;
    CompleterModel* m_model;
};


class CompleterModel : public QAbstractItemModel
{
    friend CompleterItem;
public:
    explicit CompleterModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    CompleterItem* invisibleRootItem(){return &m_root;}
    QList<CompleterItem*> findItems(const QString &text,
                                    Qt::MatchFlags flags = Qt::MatchExactly,
                                    int column = 0) const;
    void clear();
private:
    CompleterItem *itemFromIndex(const QModelIndex index) const;
    QModelIndex indexFromItem(CompleterItem *item) const;
private:
    CompleterItem m_root;
public:

};

#endif // COMPLETERMODEL_H
