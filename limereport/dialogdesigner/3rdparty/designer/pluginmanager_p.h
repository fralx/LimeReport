/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "shared_global_p.h"

#include <QtCore/QMap>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerCustomWidgetInterface;
class QDesignerPluginManagerPrivate;

class QDESIGNER_SHARED_EXPORT QDesignerPluginManager: public QObject
{
    Q_OBJECT
public:
    explicit QDesignerPluginManager(QDesignerFormEditorInterface *core);
    virtual ~QDesignerPluginManager();

    QDesignerFormEditorInterface *core() const;

    QObject *instance(const QString &plugin) const;

    QStringList registeredPlugins() const;

    QStringList findPlugins(const QString &path);

    QStringList pluginPaths() const;
    void setPluginPaths(const QStringList &plugin_paths);

    QStringList disabledPlugins() const;
    void setDisabledPlugins(const QStringList &disabled_plugins);

    QStringList failedPlugins() const;
    QString failureReason(const QString &pluginName) const;

    QList<QObject*> instances() const;
    QList<QDesignerCustomWidgetInterface*> registeredCustomWidgets() const;

    bool registerNewPlugins();

public slots:
    bool syncSettings();
    void ensureInitialized();

private:
    void updateRegisteredPlugins();
    void registerPath(const QString &path);
    void registerPlugin(const QString &plugin);

private:
    static QStringList defaultPluginPaths();
    
    QDesignerPluginManagerPrivate *m_d;
};

QT_END_NAMESPACE

#endif // PLUGINMANAGER_H
