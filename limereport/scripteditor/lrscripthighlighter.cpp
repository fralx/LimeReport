#include "lrscripthighlighter.h"
#include "lrglobal.h"
#include <QDebug>
#include <QPalette>

namespace LimeReport{

#define KEYWORDS_COUNT 59

static const char *const keywords[KEYWORDS_COUNT] = {
    "do","if","in","for","int","new","try","var","byte","case","char","else","enum",
    "goto","long","null","this","true","void","with","break","catch","class","const",
    "false","final","float","short","super","throw","while","delete","double","export",
    "import","native","public","return","static","switch","throws","typeof","boolean",
    "default","extends","finally","package","private","abstract","continue","debugger",
    "function","volatile","interface","protected","transient","implements","instanceof",
    "synchronized"
};

enum LiteralsType{SpaceFound, AlpahabetFound, NumberFound, HashFound, SlashFound, AsterixFound,
                 BracketFound, QuotationFound, ApostropheFound, SeparatorFound, BackSlashFound, LiteralsCount};
enum States {Start, MayBeKeyWord, Code, MayBeComment, Comment, Comment2, MayBeComment2End, String, String2, MayBeNumber, Separator, StatesCount};

void ScriptHighlighter::createParentheisisInfo(const char& literal, TextBlockData *data, const QString& text)
{
    int pos = text.indexOf(literal);
    while (pos != -1) {
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = literal;
        info->position = pos;
        data->insert(info);
        pos = text.indexOf(literal, pos + 1);
    }
}

void ScriptHighlighter::highlightBlock(const QString& text)
{
    int literal = -1;
    bool lastWasBackSlash = false;
    int state = previousBlockState() != -1 ? previousBlockState() : Start ;
    int oldState = -1;
    int stateMaschine[StatesCount][LiteralsCount] = {
        {Separator, MayBeKeyWord, MayBeNumber, Separator, MayBeComment, Separator, Separator, String, String2, Separator, Separator},
        {Separator, MayBeKeyWord, Code, Separator, MayBeComment, Separator, Separator, String, String2, Separator, Separator},
        {Separator, Code, Code, Separator, Separator, Separator, Separator, String, String2, Separator, Separator},
        {Separator, Code, MayBeNumber, Code, Comment, Comment2, Code, String, String2, Separator, Code},
        {Comment, Comment, Comment, Comment, Comment, Comment, Comment, Comment, Comment, Comment, Comment},
        {Comment2, Comment2, Comment2, Comment2, Comment2, MayBeComment2End, Comment2, Comment2, Comment2, Comment2, Comment2},
        {Comment2, Comment2, Comment2, Comment2, Separator, Comment2, Comment2, Comment2, Comment2, Comment2, Comment2},
        {String, String, String, String, String, String, String, Separator, String, String, String},
        {String2, String2, String2, String2, String2, String2, String2, String2, Separator, String2, String2},
        {Separator, Code, MayBeNumber, Separator, MayBeComment, Separator, Separator, String, String2, Separator, Code},
        {Separator, MayBeKeyWord, MayBeNumber, Separator, MayBeComment, Separator, Separator, String, String2, Separator, Separator }
    };

    QString buffer;

    if (text.isEmpty()) return;
    int i = 0;
    for (;;){
        QChar currentChar = text.at(i);
        switch(currentChar.toLatin1()){
            case ' ':
                literal = SpaceFound;
                break;
            case '/':
                literal = SlashFound;
                break;
            case '*':
                literal = AsterixFound;
                break;
            case '#':
                literal = HashFound;
                break;
            case '\'':
                literal = ApostropheFound;
                break;
            case '\\':
                literal = BackSlashFound;
                break;
            case '"':
                literal = QuotationFound;
                break;
            case '{': case '[': case '(':
            case '}': case ']': case ')':
                literal = BracketFound;
                break;
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
                literal = NumberFound;
                break;
            default:
                if (currentChar.isLetter())
                    literal = AlpahabetFound;
                else
                    literal = SeparatorFound;
        };

        lastWasBackSlash = !lastWasBackSlash && currentChar == QLatin1Char('\\');

        oldState = state;
        state = stateMaschine[state][literal];

        buffer += currentChar;

        if (oldState != state){
            switch( state ){
                case MayBeComment:
                    if (oldState == MayBeNumber){
                        setFormat(i-(buffer.length()-1), buffer.length()-1, m_formats[NumberFormat]);
                    }
                    buffer.clear();
                    buffer += currentChar;
                    break;
                case String:
                case String2:
                    buffer.clear();
                    buffer += currentChar;
                    break;
                case MayBeKeyWord:
                case MayBeNumber:
                    buffer.clear();
                    buffer += currentChar;
                    break;
                case Comment2:
                    setCurrentBlockState(Comment2);
                case Separator:
                    switch(oldState){
                        case MayBeComment2End:
                            setFormat(i-(buffer.length()-1), buffer.length(), m_formats[CommentFormat]);
                            setCurrentBlockState(-1);
                            buffer.clear();
                            break;
                        case MayBeKeyWord:
                            if (isKeyWord(buffer.left(buffer.length()-1))){
                                setFormat(i-(buffer.length()-1), buffer.length()-1, m_formats[KeywordFormat]);
                            }
                            buffer.clear();
                            break;
                        case MayBeNumber:
                            setFormat(i-(buffer.length()-1), buffer.length()-1, m_formats[NumberFormat]);
                            buffer.clear();
                        case String:
                        case String2:
                            setFormat(i-(buffer.length()-1), buffer.length(), m_formats[StringFormat]);
                            buffer.clear();
                            break;
                    }
                default:
                    break;
            }
        }

        if ( state == Comment || state == Comment2 ){
            setFormat(i-(buffer.length()-1), buffer.length(), m_formats[CommentFormat]);
        }

        if ( state == String || state == String2 ){
            setFormat(i-(buffer.length()-1), buffer.length(), m_formats[StringFormat]);
        }

        i++;
        if ( i>= text.length()) break;
    }

    TextBlockData *data = new TextBlockData;

    for (int i = 0; i < PARENHEIS_COUNT; ++i){
        createParentheisisInfo(parenthesisCharacters[LeftParenthesis][i].toLatin1(), data, text);
        createParentheisisInfo(parenthesisCharacters[RightParenthesis][i].toLatin1(), data, text);
    }

    setCurrentBlockUserData(data);
}

bool ScriptHighlighter::isKeyWord(const QString& word)
{
    for (int i = 0; i < KEYWORDS_COUNT-1; ++i){
        if (QLatin1String(keywords[i]) == word) return true;
    }
    return false;
}

ScriptHighlighter::ScriptHighlighter(QTextDocument* parent):
    QSyntaxHighlighter(parent)
{

    if ( isColorDark(QPalette().background().color())){
        m_formats[NumberFormat].setForeground(Qt::darkBlue);
        m_formats[StringFormat].setForeground(Qt::darkGreen);
        m_formats[KeywordFormat].setForeground(Qt::darkYellow);
        m_formats[CommentFormat].setForeground(Qt::darkGreen);
        m_formats[CommentFormat].setFontItalic(true);
    } else {
        m_formats[NumberFormat].setForeground(QColor("#ff6aad"));
        m_formats[StringFormat].setForeground(QColor("#b27f40"));
        m_formats[KeywordFormat].setForeground(QColor("#45c5d5"));
        m_formats[CommentFormat].setForeground(QColor("#a1a4a9"));
        m_formats[CommentFormat].setFontItalic(true);
    }
}

TextBlockData::~TextBlockData()
{
    foreach(ParenthesisInfo* info, m_parentheses){
        delete info;
    }
}

QVector<ParenthesisInfo*> TextBlockData::parentheses()
{
    return m_parentheses;
}

void TextBlockData::insert(ParenthesisInfo* info)
{
    int i = 0;
    while (i < m_parentheses.size() &&
        info->position > m_parentheses.at(i)->position)
        ++i;

    m_parentheses.insert(i, info);
}

} // namespace LimeReport
