#ifndef LRDATASOURCEINTF_H
#define LRDATASOURCEINTF_H
#include <QSharedPointer>
#include <QAbstractItemModel>
namespace LimeReport {

class IDataSource {
public:
    enum DatasourceMode{DESIGN_MODE,RENDER_MODE};
    typedef QSharedPointer<IDataSource> Ptr;
    virtual ~IDataSource() {}
    virtual bool next() = 0;
    virtual bool hasNext() = 0;
    virtual bool prior() = 0;
    virtual void first() = 0;
    virtual void last() = 0;
    virtual bool bof() = 0;
    virtual bool eof() = 0;
    virtual QVariant data(const QString& columnName) = 0;
    virtual QVariant dataByRowIndex(const QString& columnName, int rowIndex) = 0;
    virtual QVariant dataByKeyField(const QString& columnName, const QString& keyColumnName, QVariant keyData) = 0;
    virtual int columnCount() = 0;
    virtual QString columnNameByIndex(int columnIndex) = 0;
    virtual int columnIndexByName(QString name) = 0;
    virtual bool isInvalid() const = 0;
    virtual QString lastError() = 0;
    virtual QAbstractItemModel* model() = 0;
};

class IDataSourceHolder {
public:
    virtual ~IDataSourceHolder(){}
    virtual IDataSource* dataSource(IDataSource::DatasourceMode mode = IDataSource::RENDER_MODE) = 0;
    virtual QString lastError() const = 0;
    virtual bool isInvalid() const = 0;
    virtual bool isOwned() const = 0;
    virtual bool isEditable() const = 0;
    virtual bool isRemovable() const = 0;
    virtual void invalidate(IDataSource::DatasourceMode mode, bool dbWillBeClosed = false) = 0;
    virtual void update() = 0;
    virtual void clearErrors() = 0;
};

} // namespace LimeReport

#endif // LRDATASOURCEINTF_H


