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
#ifndef LRSIMPLETAGPARSER_H
#define LRSIMPLETAGPARSER_H

#include <QVector>
#include <QString>
#include <QStringList>

namespace LimeReport{

class Tag{
public:
    Tag(QString text,int beginPos,int endPos)
        :m_tagText(text),m_beginPos(beginPos),m_endPos(endPos){}
    Tag():m_tagText(""),m_beginPos(-1),m_endPos(-1){}
    bool isValid(){return (!m_tagText.isEmpty())&&(m_beginPos>0)&&(m_endPos>0);}
    QString tagText() const {return m_tagText;}
    int begin() const {return m_beginPos;}
    int end() const {return m_endPos;}
private:
    QString m_tagText;
    int m_beginPos;
    int m_endPos;
};

class Symb{
public:
    Symb(QString text, int pos):m_text(text),m_pos(pos){}
    Symb():m_text(""),m_pos(-1){}
    bool isValid(){return (!m_text.isEmpty())&&(m_pos>0);}
    bool isTag(){return isValid()&&m_text.at(0)=='<';}
    QString text(){return m_text;}
    int pos(){return m_pos;}
private:
    QString m_text;
    int m_pos;
};

struct TagDiff{
    enum Direction {
        Inner=0,
        Outer=1
    };
    Tag* tag;
    Direction direction;
};

class HtmlContext
{
public:
    HtmlContext(QString html);
    ~HtmlContext();
    static QString extractWord(QString text,int index);
    static QVector<TagDiff> tagVectDiff(QVector<Tag*> source, QVector<Tag*> dest);
    static bool isVectorEqual(QVector<Tag*> source, QVector<Tag*> dest);
    void fillTagVector(QString html);
    //QString extendTextByTags(QString text, int pos);
    QVector<Tag *> tagsAt(int pos);
    Symb symbAt(int pos);
    void clearTags();
    void clearSymbs();
private:
    static QString parseTag(QVector<Tag*>& storage,QString text,int& curPos, bool createTag=true);
    void parseSymbs(QString text);
    void initSymbPatterns();
private:
    QVector<Tag*> m_tags;
    QVector<Symb*> m_symbs;
    QStringList m_symbPatterns;
};
}
#endif // LRSIMPLETAGPARSER_H
