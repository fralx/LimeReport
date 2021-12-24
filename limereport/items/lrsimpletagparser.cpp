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
#include "lrsimpletagparser.h"
#include <QDebug>
#include <QStringList>
#if QT_VERSION < 0x060000
#include <QRegExp>
#else
#include <QRegularExpression>
#endif
namespace LimeReport{

void HtmlContext::fillTagVector(QString html)
{
#if QT_VERSION < 0x060000
    QRegExp rx("<[^<]*>");
    QString buff=html;
    int curPos=0;
    while(buff.contains(rx)){
        int pos=rx.indexIn(buff);
        curPos+=pos;
        buff=buff.right(buff.length()-pos);
        if (rx.cap().at(1)!='/'){
            int initPos=curPos;
            parseTag(m_tags,buff,initPos);
        }
        buff=buff.right(buff.length()-rx.matchedLength());
    }
#else
    QRegularExpression rx("<[^<]*>");
    QString buff=html;
    while(buff.contains(rx)){
        QRegularExpressionMatch match = rx.match(buff);
        // TODO: Qt6 port
    }

#endif
}

QString HtmlContext::parseTag(QVector<Tag *> &storage, QString text, int &curPos, bool createTag)
{
#if QT_VERSION < 0x060000
    QRegExp rx("<[^<]*>");
    int pos=rx.indexIn(text);
    int begPos=pos+curPos;
    QString buff=text.right(text.length()-(pos+rx.matchedLength()));

    QString tagName=rx.cap(0);
    tagName.remove('<');
    tagName.remove('>');

    while (buff.contains(rx)){
        pos=rx.indexIn(buff);
        buff=buff.right(buff.length()-pos);
        curPos+=pos;
        if (extractWord(rx.cap(0),1).compare(extractWord(tagName,1),Qt::CaseInsensitive)==0){
            if (rx.cap(0).at(1)=='/'){
                if (createTag) storage.append(new Tag(tagName,begPos,curPos));
                return buff.right(buff.length()-rx.matchedLength());
            } else {
                buff=parseTag(storage,buff,curPos,false);

            }
        } else {
            buff=buff.right(buff.length()-rx.matchedLength());
        }
    }
#else
    QRegularExpression rx("<[^<]*>");
    // TODO: Qt6 port
#endif
    return "";
}

void HtmlContext::parseSymbs(QString text)
{
#if QT_VERSION < 0x060000
    QRegExp rx("<[^<]*[^/]>");
    while (text.contains(rx)){
        int pos=rx.indexIn(text);
        if (rx.cap().compare("<br>",Qt::CaseInsensitive)==0)
            m_symbs.append(new Symb(rx.cap(),pos));
        text.remove(pos,rx.matchedLength());
    }

    foreach(QString pattern, m_symbPatterns){
        rx.setPattern(pattern);
        while (text.contains(rx)){
            int pos=rx.indexIn(text);
            m_symbs.append(new Symb(rx.cap(0),pos));
            text.replace(rx.cap(0)," ");
        }
    }
#else
    QRegularExpression rx("<[^<]*>");

    while (text.contains(rx)){
        int pos=text.indexOf(rx); //rx.indexIn(text);
        if (rx.cap().compare("<br>",Qt::CaseInsensitive)==0)
            m_symbs.append(new Symb(rx.cap(),pos));
        text.remove(pos,rx.matchedLength());
    }
    // TODO: Qt6 port
#endif
}

void HtmlContext::initSymbPatterns()
{
    m_symbPatterns<<"&[^&]*;"<<"<[^<]*/>";
}

HtmlContext::HtmlContext(QString html)
{
    fillTagVector(html);
    parseSymbs(html);
    initSymbPatterns();
}

HtmlContext::~HtmlContext()
{
    clearTags();
    clearSymbs();
}

QString HtmlContext::extractWord(QString text, int index)
{
    text.remove('<');
    text.remove('>');
    text.remove('/');
    int counter=1;
    QString retWord("");
    for (int i=0;i<text.length();i++){
        if (text.at(i)==' '){
            if (counter==index) {return retWord;}
            else {retWord="";counter++;}
        }
        retWord+=text.at(i);
    }
    if (counter==index) return retWord;
    else return "";
}

QVector<TagDiff> HtmlContext::tagVectDiff(QVector<Tag *> source, QVector<Tag *> dest)
{
    QVector<TagDiff> resVect;
    for(int i=0;i<source.count();++i){
        if (!dest.contains(source.at(i))){
            TagDiff tagDiff;
            tagDiff.tag=source.at(i);
            tagDiff.direction=TagDiff::Outer;
            resVect.append(tagDiff);
        }
    }
    for(int i=0;i<dest.count();++i){
        if (!source.contains(dest.at(i))){
            TagDiff tagDiff;
            tagDiff.tag=dest.at(i);
            tagDiff.direction=TagDiff::Inner;
            resVect.append(tagDiff);
        }
    }
    return resVect;
}

bool HtmlContext::isVectorEqual(QVector<Tag *> source, QVector<Tag *> dest)
{
    if (source.count()!=dest.count()) return false;
    foreach(Tag* tag,source){
        if (!dest.contains(tag)) return false;
    }
    return true;
}

//QString HtmlContext::extendTextByTags(QString text, int pos)
//{
//    QString curText="";
//    QVector<Tag*> curTags=tagsAt(pos);
//    for(int i=0;i<curTags.count();i++){
//        curText+='<'+curTags.at(i)->tagText()+'>';
//    }

//    for(int i=0;i<text.length();i++,pos++){
//        QVector<Tag*> tagsAtPos=tagsAt(pos);
//        if (!HtmlContext::isVectorEqual(curTags,tagsAtPos)){
//            QVector<TagDiff> diffs=HtmlContext::tagVectDiff(curTags,tagsAtPos);
//            foreach(TagDiff diff,diffs){
//                if (diff.direction==TagDiff::Inner){
//                    curText+='<'+diff.tag->tagText()+'>';
//                    curTags.append(diff.tag);
//                }
//                else{
//                    curText+="</"+HtmlContext::extractWord(diff.tag->tagText(),1)+'>';
//                    curTags.remove(curTags.indexOf(diff.tag));
//                }
//            }
//        }
//        Symb s=symbAt(pos);
//        if (s.isValid()){
//            if (s.isTag()) curText+=s.text()+text.at(i);
//            else curText+=s.text();
//        } else curText+=text.at(i);
//    }

//    curTags=tagsAt(pos);
//    for(int i=0;i<curTags.count();i++){
//        curText+="</"+HtmlContext::extractWord(curTags.at(i)->tagText(),1)+'>';
//    }

//    return curText;
//}

QVector<Tag *> HtmlContext::tagsAt(int pos)
{
    QVector<Tag*> result;
    foreach(Tag* tag,m_tags){
        if ((pos>=tag->begin())&&(pos<=tag->end())) result.append(tag);
    }
    return result;
}

Symb HtmlContext::symbAt(int pos)
{
    foreach(Symb* symb,m_symbs){
        if (pos==symb->pos()) return *symb;
    }
    return Symb();
}

void HtmlContext::clearTags()
{
    foreach(Tag* tag,m_tags) delete tag;
    m_tags.clear();
}

void HtmlContext::clearSymbs()
{
    foreach(Symb* symb,m_symbs) delete symb;
    m_tags.clear();
}

}
