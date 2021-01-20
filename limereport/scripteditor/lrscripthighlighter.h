#ifndef LRSCRIPTHIGHLIGHTER_H
#define LRSCRIPTHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QSet>

namespace LimeReport{

enum ParenthesisType {LeftParenthesis, RightParenthesis, ParenthesisTypeCount};

#define PARENHEIS_COUNT 3
const QChar parenthesisCharacters[ParenthesisTypeCount][PARENHEIS_COUNT] = {
    {'(', '{', '['},
    {')', '}', ']'}
};

struct ParenthesisInfo
{
    char character;
    int position;
};

class TextBlockData : public QTextBlockUserData
{
public:
    TextBlockData(){}
    ~TextBlockData();
    QVector<ParenthesisInfo *> parentheses();
    void insert(ParenthesisInfo *info);

private:
    QVector<ParenthesisInfo *> m_parentheses;
};

class ScriptHighlighter : QSyntaxHighlighter{
public:
    ScriptHighlighter(QTextDocument* parent);
protected:
    void highlightBlock(const QString& text);
    enum ScriptFormats {
        NumberFormat, StringFormat, KeywordFormat,
        CommentFormat, FormatsCount
    };
    QTextCharFormat m_formats[FormatsCount];
    bool isKeyWord(const QString& word);
    void createParentheisisInfo(const char& literal, TextBlockData *data, const QString& text);
private:
    QSet<QString> m_keywords;
};


}
#endif // LRSCRIPTHIGHLIGHTER_H
