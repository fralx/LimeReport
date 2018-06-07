#include <QString>
#include <QtTest>
#include "../limereport/lrdatadesignintf.h"

class CallbackDSTest : public QObject
{
    Q_OBJECT

public:
    CallbackDSTest();
private:
    LimeReport::CallbackDatasource* m_testDS;
    LimeReport::CallbackDatasource* m_test1DS;
    int m_currentRow;
protected Q_SLOTS:
    void slotTestOneSlotDS(LimeReport::CallbackInfo info, QVariant& data);
    void slotGetCallbackData(LimeReport::CallbackInfo info, QVariant& data);
    void slotChangePos(const LimeReport::CallbackInfo::ChangePosType& type, bool& result);
private Q_SLOTS:
    void testOneSlotDS();
    void testTwoSlotDS();

};

CallbackDSTest::CallbackDSTest()
{
    m_testDS = new LimeReport::CallbackDatasource();
    connect(m_testDS, SIGNAL(getCallbackData(LimeReport::CallbackInfo,QVariant&)),
            this, SLOT(slotTestOneSlotDS(LimeReport::CallbackInfo,QVariant&)));

    m_test1DS = new LimeReport::CallbackDatasource();
    m_currentRow = -1;
    connect(m_test1DS, SIGNAL(getCallbackData(LimeReport::CallbackInfo,QVariant&)),
            this, SLOT(slotGetCallbackData(LimeReport::CallbackInfo,QVariant&)));
    connect(m_test1DS, SIGNAL(changePos(LimeReport::CallbackInfo::ChangePosType,bool&)),
            this, SLOT(slotChangePos(LimeReport::CallbackInfo::ChangePosType,bool&)));
}



void CallbackDSTest::slotTestOneSlotDS(LimeReport::CallbackInfo info, QVariant& data)
{
    QStringList columns;
    columns << "Name" << "Value";
    QVector<QString> values;
    values<<"Mazda"<<"Nissan";
    int dataIndex = 0;
    switch (info.dataType) {
        case LimeReport::CallbackInfo::RowCount:
            data = 10;
            break;
        case LimeReport::CallbackInfo::ColumnCount:
            data = columns.size();
            break;
        case LimeReport::CallbackInfo::ColumnHeaderData: {
            data = columns.at(info.index);
            break;
        }
        case LimeReport::CallbackInfo::ColumnData:
            if (info.index > 5){
                dataIndex = 1;
            }
            if (info.columnName == "Name")
                data = values[dataIndex];
            else {
                data = info.index;
            }
            break;
        default: break;
    }
}

void CallbackDSTest::slotGetCallbackData(LimeReport::CallbackInfo info, QVariant& data)
{
    QStringList columns;
    columns << "Name" << "Value";
    QVector<QString> values;
    values<<"Mazda"<<"Nissan";
    int dataIndex = 0;
    switch (info.dataType) {
        case LimeReport::CallbackInfo::ColumnCount:
            data = columns.size();
            break;
        case LimeReport::CallbackInfo::ColumnHeaderData: {
            data = columns.at(info.index);
            break;
        }
        case LimeReport::CallbackInfo::HasNext:{
            data = (info.index < 9);
            break;
        }
        case LimeReport::CallbackInfo::ColumnData:
            if (info.index > 5){
                dataIndex = 1;
            }
            if (info.columnName == "Name")
                data = values[dataIndex];
            else {
                data = m_currentRow;
            }
            break;
        default: break;
    }
}

void CallbackDSTest::slotChangePos(const LimeReport::CallbackInfo::ChangePosType& type, bool& result)
{
    if (type == LimeReport::CallbackInfo::First) {m_currentRow = 0; result = true;}
    else {if (m_currentRow<9) m_currentRow++; result = (m_currentRow <= 9);}
}

void CallbackDSTest::testOneSlotDS()
{
    QVERIFY2(m_testDS->bof(), "Failure test bof");
    QVERIFY2(!m_testDS->eof(), "Failure test eof");
    QVERIFY2(m_testDS->hasNext(), "Failure hasNext");
    QVERIFY2(m_testDS->columnCount() == 2, "Failure test column count");
    QVERIFY2(m_testDS->columnNameByIndex(0).compare("Name") == 0, "Failure test column name");
    QVERIFY2(m_testDS->columnNameByIndex(1).compare("Value") == 0, "Failure test column name");
    QVERIFY2(m_testDS->columnIndexByName("Name") == 0, "Failure test column index");
    QVERIFY2(m_testDS->columnIndexByName("Value") == 1, "Failure test column index");
    QVERIFY2(!m_testDS->data("Name").isValid(),"Failure test data on bof");
    QVERIFY2(!m_testDS->data("Value").isValid(),"Failure test data on bof");
    QVERIFY2(m_testDS->next(), "Failure next");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_testDS->data("Value").toInt(),0);
    QVERIFY2(!m_testDS->prior(), "Failure test prior");
    QVERIFY2(m_testDS->next(), "Failure next");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_testDS->data("Value").toInt(),1);
    QVERIFY2(m_testDS->prior(), "Failure test prior");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_testDS->data("Value").toInt(),0);
    QVERIFY2(m_testDS->next(), "Failure next");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_testDS->data("Value").toInt(),1);
    QVERIFY2(m_testDS->next(), "Failure next");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_testDS->data("Value").toInt(),2);
    QVERIFY2(m_testDS->prior(), "Failure test prior");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_testDS->data("Value").toInt(),1);
    QVERIFY2(!m_testDS->prior(), "Failure test prior");
    QVERIFY2(m_testDS->next(), "Failure next");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_testDS->data("Value").toInt(),2);
    for(int i = 3; i < 6; ++i) m_testDS->next();
    QCOMPARE(m_testDS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_testDS->data("Value").toInt(),5);
    QVERIFY2(m_testDS->next(), "Failure next");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_testDS->data("Value").toInt(),6);
    QVERIFY2(m_testDS->prior(), "Failure test prior");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_testDS->data("Value").toInt(),5);
    QVERIFY2(m_testDS->next(), "Failure next");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_testDS->data("Value").toInt(),6);
    for(int i = 7; i < 9; ++i) m_testDS->next();
    QCOMPARE(m_testDS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_testDS->data("Value").toInt(),8);
    QVERIFY2(m_testDS->next(), "Failure next");
    QCOMPARE(m_testDS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_testDS->data("Value").toInt(),9);
    QCOMPARE(m_testDS->hasNext(), false);
    QCOMPARE(m_testDS->next(), false);
    QCOMPARE(m_testDS->bof(), false);
    QCOMPARE(m_testDS->eof(), true);
    QCOMPARE(m_testDS->prior(), true);
    QCOMPARE(m_testDS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_testDS->data("Value").toInt(),8);
    QCOMPARE(m_testDS->hasNext(), true);
    QCOMPARE(m_testDS->next(), true);
    QCOMPARE(m_testDS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_testDS->data("Value").toInt(),9);
    QCOMPARE(m_testDS->hasNext(), false);
    QCOMPARE(m_testDS->next(), false);
    QCOMPARE(m_testDS->bof(), false);
    QCOMPARE(m_testDS->eof(), true);
    QCOMPARE(m_testDS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_testDS->data("Value").toInt(),9);
}


void CallbackDSTest::testTwoSlotDS()
{

    QVERIFY2(m_test1DS->bof(), "Failure test bof");
    QVERIFY2(!m_test1DS->eof(), "Failure test eof");
    QVERIFY2(m_test1DS->hasNext(), "Failure hasNext");
    QVERIFY2(m_test1DS->columnCount() == 2, "Failure test column count");
    QVERIFY2(m_test1DS->columnNameByIndex(0).compare("Name") == 0, "Failure test column name");
    QVERIFY2(m_test1DS->columnNameByIndex(1).compare("Value") == 0, "Failure test column name");
    QVERIFY2(m_test1DS->columnIndexByName("Name") == 0, "Failure test column index");
    QVERIFY2(m_test1DS->columnIndexByName("Value") == 1, "Failure test column index");
    QVERIFY2(!m_test1DS->data("Name").isValid(),"Failure test data on bof");
    QVERIFY2(!m_test1DS->data("Value").isValid(),"Failure test data on bof");
    QVERIFY2(m_test1DS->next(), "Failure next");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_test1DS->data("Value").toInt(),0);
    QVERIFY2(!m_test1DS->prior(), "Failure test prior");
    QVERIFY2(m_test1DS->next(), "Failure next");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_test1DS->data("Value").toInt(),1);
    QVERIFY2(m_test1DS->prior(), "Failure test prior");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_test1DS->data("Value").toInt(),0);
    QVERIFY2(m_test1DS->next(), "Failure next");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_test1DS->data("Value").toInt(),1);
    QVERIFY2(m_test1DS->next(), "Failure next");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_test1DS->data("Value").toInt(),2);
    QVERIFY2(m_test1DS->prior(), "Failure test prior");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_test1DS->data("Value").toInt(),1);
    QVERIFY2(!m_test1DS->prior(), "Failure test prior");
    QVERIFY2(m_test1DS->next(), "Failure next");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_test1DS->data("Value").toInt(),2);
    for(int i = 3; i < 6; ++i) m_test1DS->next();
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_test1DS->data("Value").toInt(),5);
    QVERIFY2(m_test1DS->next(), "Failure next");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_test1DS->data("Value").toInt(),6);
    QVERIFY2(m_test1DS->prior(), "Failure test prior");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Mazda"));
    QCOMPARE(m_test1DS->data("Value").toInt(),5);
    QVERIFY2(m_test1DS->next(), "Failure next");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_test1DS->data("Value").toInt(),6);
    for(int i = 7; i < 9; ++i) m_test1DS->next();
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_test1DS->data("Value").toInt(),8);
    QVERIFY2(m_test1DS->next(), "Failure next");
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_test1DS->data("Value").toInt(),9);
    QCOMPARE(m_test1DS->hasNext(), false);
    QCOMPARE(m_test1DS->next(), false);
    QCOMPARE(m_test1DS->bof(), false);
    QCOMPARE(m_test1DS->eof(), true);
    QCOMPARE(m_test1DS->prior(), true);
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_test1DS->data("Value").toInt(),8);
    QCOMPARE(m_test1DS->hasNext(), true);
    QCOMPARE(m_test1DS->next(), true);
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_test1DS->data("Value").toInt(),9);
    QCOMPARE(m_test1DS->hasNext(), false);
    QCOMPARE(m_test1DS->next(), false);
    QCOMPARE(m_test1DS->bof(), false);
    QCOMPARE(m_test1DS->eof(), true);
    QCOMPARE(m_test1DS->data("Name").toString(),QString("Nissan"));
    QCOMPARE(m_test1DS->data("Value").toInt(),9);
}

QTEST_APPLESS_MAIN(CallbackDSTest)

#include "tst_callbackdstest.moc"
