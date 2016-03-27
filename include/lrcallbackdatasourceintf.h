#ifndef LRVIRTUALDATASOURCEINTF
#define LRVIRTUALDATASOURCEINTF
#include <QObject>
namespace LimeReport {

struct CallbackInfo{
    enum DataType{IsEmpty, HasNext, ColumnHeaderData, ColumnData, ColumnCount, RowCount};
    enum ChangePosType{First, Next};
    DataType dataType;
    int index;
    QString columnName;
};

class ICallbackDatasource :public QObject{
    Q_OBJECT
signals:
    void getCallbackData(const LimeReport::CallbackInfo& info, QVariant& data);
    void changePos(const LimeReport::CallbackInfo::ChangePosType& type, bool& result);
};

}

#endif // LRVIRTUALDATASOURCEINTF

