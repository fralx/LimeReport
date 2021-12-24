/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2021 by Alexander Arin                                  *
 *   arin_a@bk.ru                                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ****************************************************************************/
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QMenu>

#include "lrglobal.h"
#include "lrobjectinspectorwidget.h"
#include "lrobjectitemmodel.h"

namespace LimeReport{

ObjectInspectorTreeView::ObjectInspectorTreeView(QWidget *parent)
    :QTreeView(parent), m_propertyDelegate(0)
{
    setRootIsDecorated(false);
    initColorMap();
    setEditTriggers(
        QAbstractItemView::DoubleClicked |
        QAbstractItemView::SelectedClicked |
        QAbstractItemView::EditKeyPressed
    );
    m_propertyDelegate = new PropertyDelegate(this);
    setItemDelegate(m_propertyDelegate);
    QPalette p = palette();
    p.setColor(QPalette::AlternateBase,QColor(229,255,214));
    setPalette(p);
}

ObjectInspectorTreeView::~ObjectInspectorTreeView(){}

void ObjectInspectorTreeView::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
    ObjectPropItem *node = nodeFromIndex(index);
    StyleOptionViewItem so = options;
    bool alternate = so.features & StyleOptionViewItem::Alternate;
    if (node){
        if ((!node->isHaveValue())){
            const QColor c = options.palette.color(QPalette::Dark);
            painter->fillRect(options.rect,c);
            so.palette.setColor(QPalette::AlternateBase,c);
        } else {
            if ( index.isValid()&&(nodeFromIndex(index)->colorIndex()!=-1) ){
                QColor fillColor(getColor(nodeFromIndex(index)->colorIndex()%m_colors.count()));
                so.palette.setColor(QPalette::AlternateBase,fillColor.lighter(115));
                if (!alternate){
                    painter->fillRect(options.rect,fillColor);
                }
            }
        }
    }
    QTreeView::drawRow(painter,so,index);
    painter->save();
    QColor gridLineColor = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor,&so));
    painter->setPen(gridLineColor);
    painter->drawLine(so.rect.x(),so.rect.bottom(),so.rect.right(),so.rect.bottom());
    painter->restore();
}

void ObjectInspectorTreeView::mousePressEvent(QMouseEvent *event)
{

    if ((event->button()==Qt::LeftButton)){
        QModelIndex index=indexAt(event->pos());
        if (index.isValid()){
            if (event->pos().x() < indentation()) {
                if (!nodeFromIndex(index)->isHaveValue())
                    setExpanded(index,!isExpanded(index));
            } else {
                if ((index.column()==1)&&(!nodeFromIndex(index)->isHaveChildren())) {
                    setCurrentIndex(index);

                    Qt::ItemFlags flags = index.model()->flags(index);
                    if ( !(((flags & Qt::ItemIsEditable) == 0) || ((flags & Qt::ItemIsEnabled) == 0)) )
                        edit(index);

                    return ;
                }
            }
        }
    }

    QTreeView::mousePressEvent(event);
}

void ObjectInspectorTreeView::initColorMap()
{
    m_colors.reserve(6);
    m_colors.push_back(QColor(255,230,191));
    m_colors.push_back(QColor(255,255,191));
    m_colors.push_back(QColor(191,255,191));
    m_colors.push_back(QColor(199,255,255));
    m_colors.push_back(QColor(234,191,255));
    m_colors.push_back(QColor(255,191,239));
}

QColor ObjectInspectorTreeView::getColor(const int index) const
{
    return m_colors[index];
}

void ObjectInspectorTreeView::reset()
{
    QTreeView::reset();
    for (int i=0;i<model()->rowCount();i++){
        if (!nodeFromIndex(model()->index(i,0))->isHaveValue()){
          setFirstColumnSpanned(i,model()->index(i,0).parent(),true);
        }
    }
}

ObjectPropItem * ObjectInspectorTreeView::nodeFromIndex(QModelIndex index) const
{
    return qvariant_cast<LimeReport::ObjectPropItem*>(index.data(Qt::UserRole));
}

void ObjectInspectorTreeView::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Return){
        if(!m_propertyDelegate->isEditing()){
            QModelIndex index = currentIndex().model()->index(currentIndex().row(),
                                                              1,currentIndex().parent());
            edit(index);
            event->accept();
        }
    } else QTreeView::keyPressEvent(event);
}

void ObjectInspectorTreeView::commitActiveEditorData(){
    if (state()==QAbstractItemView::EditingState){
        commitData(indexWidget(currentIndex()));
    }
}

bool PropertyFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (sourceParent.isValid()) return true;    
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    return sourceModel()->data(index).toString().contains(filterRegExp());
#else
    return sourceModel()->data(index).toString().contains(filterRegularExpression());
#endif
}

ObjectInspectorWidget::ObjectInspectorWidget(QWidget *parent)
    :QWidget(parent), m_filterModel(0)
{
    m_objectInspectorView = new ObjectInspectorTreeView(this);
    m_propertyModel = new BaseDesignPropertyModel(this);
    m_filterModel = new PropertyFilterModel(this);
    m_filterModel->setSourceModel(m_propertyModel);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    m_filterModel->setFilterRegExp(QRegExp("", Qt::CaseInsensitive, QRegExp::FixedString));
#else
    m_filterModel->setFilterRegularExpression(QRegularExpression("", QRegularExpression::CaseInsensitiveOption));
#endif
    m_objectInspectorView->setModel(m_filterModel);
    QVBoxLayout* l = new QVBoxLayout();
    QLineEdit* le = new QLineEdit(this);
    QToolButton * pbClear = new QToolButton(this);
    pbClear->setToolTip(tr("Clear"));
    pbClear->setIcon(QIcon(":/items/clear.png"));
    connect(pbClear, SIGNAL(clicked()), le, SLOT(clear()));
    le->setPlaceholderText(tr("Filter"));
    connect(le, SIGNAL(textChanged(const QString&)), this, SLOT(slotFilterTextChanged(const QString&)));

    QToolButton* settingButton = new QToolButton(this);
    settingButton->setIcon(QIcon(":/items/images/settings.png"));
    QMenu* settingMenu = new QMenu(settingButton);
    m_translateProperties = settingMenu->addAction(tr("Translate properties"));
    m_translateProperties->setCheckable(true);
    m_translateProperties->setChecked(translateProperties());

    connect(m_translateProperties, SIGNAL(toggled(bool)),
            this, SLOT(slotTranslatePropertiesChecked(bool)));

    settingButton->setMenu(settingMenu);
    settingButton->setPopupMode(QToolButton::InstantPopup);

    QHBoxLayout* h = new QHBoxLayout();
    h->setSpacing(2);
    h->addWidget(le);
    h->addWidget(pbClear);
    h->addWidget(settingButton);
    l->addLayout(h);
    l->addWidget(m_objectInspectorView);
    int margin = Const::DOCKWIDGET_MARGINS;
    l->setContentsMargins(margin, margin, margin, margin);
    l->setSpacing(2);
    this->setLayout(l);
}

void ObjectInspectorWidget::setModel(QAbstractItemModel *model)
{

    m_filterModel->setSourceModel(model);

}

void ObjectInspectorWidget::setAlternatingRowColors(bool value)
{
    m_objectInspectorView->setAlternatingRowColors(value);
}

void ObjectInspectorWidget::setRootIsDecorated(bool value)
{
    m_objectInspectorView->setRootIsDecorated(value);
}

void ObjectInspectorWidget::setColumnWidth(int column, int width)
{
    m_objectInspectorView->setColumnWidth(column, width);
}

int ObjectInspectorWidget::columnWidth(int column)
{
    return m_objectInspectorView->columnWidth(column);
}

void ObjectInspectorWidget::expandToDepth(int depth)
{
    m_objectInspectorView->expandToDepth(depth);
}

void ObjectInspectorWidget::commitActiveEditorData()
{
    m_objectInspectorView->commitActiveEditorData();
}

void ObjectInspectorWidget::setValidator(ValidatorIntf *validator)
{
    m_propertyModel->setValidator(validator);
}

bool ObjectInspectorWidget::subclassesAsLevel()
{
    return m_propertyModel->subclassesAsLevel();
}

void ObjectInspectorWidget::setSubclassesAsLevel(bool value)
{
    m_propertyModel->setSubclassesAsLevel(value);
}

bool ObjectInspectorWidget::translateProperties()
{
    return m_propertyModel->isTranslateProperties();
}

void ObjectInspectorWidget::setTranslateProperties(bool value)
{
    m_propertyModel->setTranslateProperties(value);
    m_translateProperties->setChecked(value);
    update();
}

const QObject *ObjectInspectorWidget::object(){
    return m_propertyModel->currentObject();
}

void ObjectInspectorWidget::setObject(QObject *object)
{
    m_propertyModel->setObject(object);
}

void ObjectInspectorWidget::setMultiObjects(QList<QObject *> *list)
{
    m_propertyModel->setMultiObjects(list);
}

void ObjectInspectorWidget::clearObjectsList()
{
    m_propertyModel->clearObjectsList();
}

void ObjectInspectorWidget::updateProperty(const QString &propertyName)
{
    m_propertyModel->updateProperty(propertyName);
}

void ObjectInspectorWidget::slotFilterTextChanged(const QString &filter)
{
    if (m_filterModel)
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        m_filterModel->setFilterRegExp(QRegExp(filter, Qt::CaseInsensitive, QRegExp::FixedString));
#else
        m_filterModel->setFilterRegularExpression(QRegularExpression(filter, QRegularExpression::CaseInsensitiveOption));
#endif
}

void ObjectInspectorWidget::slotTranslatePropertiesChecked(bool value)
{
    setTranslateProperties(value);
}

} //namespace LimeReport
