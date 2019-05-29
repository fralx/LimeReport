#ifndef LRPREPAREDPAGES_H
#define LRPREPAREDPAGES_H

#include "lrpagedesignintf.h"
#include "lrpreparedpagesintf.h"

namespace LimeReport {

class PreparedPages: public IPreparedPages{
public:
    PreparedPages(ReportPages* pages):m_pages(pages){}
    ~PreparedPages(){}
// IPreviewPages interface
private:
    bool loadFromFile(const QString &fileName);
    bool loadFromString(const QString data);
    bool loadFromByteArray(QByteArray *data);
    bool saveToFile(const QString &fileName);
    QString saveToString();
    QByteArray saveToByteArray();
    void clear();
private:
    bool readPages(ItemsReaderIntf::Ptr reader);
    ReportPages* m_pages;
};

} // namespace LimeReport

#endif // LRPREPAREDPAGES_H
