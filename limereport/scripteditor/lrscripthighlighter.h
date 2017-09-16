#ifndef LRSCRIPTHIGHLIGHTER_H
#define LRSCRIPTHIGHLIGHTER_H

#include <QSyntaxHighlighter>

namespace LimeReport{

class ScriptHighlighter : QSyntaxHighlighter{
public:
    ScriptHighlighter(QTextDocument* parent): QSyntaxHighlighter(parent){}
    // QSyntaxHighlighter interface
protected:
    void highlightBlock(const QString& text);
};

}
#endif // LRSCRIPTHIGHLIGHTER_H
