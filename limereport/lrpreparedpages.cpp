#include "lrpreparedpages.h"

#include "serializators/lrxmlreader.h"
#include "serializators/lrxmlwriter.h"

namespace LimeReport {

bool PreparedPages::loadFromFile(const QString &fileName)
{
    ItemsReaderIntf::Ptr reader = FileXMLReader::create(fileName);
    return readPages(reader);
}

bool PreparedPages::loadFromString(const QString data)
{
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(data);
    return readPages(reader);
}

bool PreparedPages::loadFromByteArray(QByteArray *data)
{
    ItemsReaderIntf::Ptr reader = ByteArrayXMLReader::create(data);
    return readPages(reader);
}

bool PreparedPages::saveToFile(const QString &fileName)
{
    if (!fileName.isEmpty()){
        QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
        foreach (PageItemDesignIntf::Ptr page, *m_pages){
            writer->putItem(page.data());
        }
        return writer->saveToFile(fileName);
    }
    return false;
}

QString PreparedPages::saveToString()
{
    QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
    foreach (PageItemDesignIntf::Ptr page, *m_pages){
        writer->putItem(page.data());
    }
    return writer->saveToString();
}

QByteArray PreparedPages::saveToByteArray()
{
    QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
    foreach (PageItemDesignIntf::Ptr page, *m_pages){
        writer->putItem(page.data());
    }
    return writer->saveToByteArray();
}

void PreparedPages::clear()
{
    m_pages->clear();
}

bool PreparedPages::readPages(ItemsReaderIntf::Ptr reader)
{
    clear();
    if (reader->first()){
        PageItemDesignIntf::Ptr page = PageItemDesignIntf::create(0);
        if (!reader->readItem(page.data()))
            return false;
        else {
            m_pages->append(page);
            while (reader->next()){
                page = PageItemDesignIntf::create(0);
                if (!reader->readItem(page.data())){
                    m_pages->clear();
                    return false;
                } else {
                    m_pages->append(page);
                }
            }
        }

        return true;
    }
    return false;
}

}
