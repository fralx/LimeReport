#ifndef LRCODEEDITOR_H
#define LRCODEEDITOR_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include "lrscripthighlighter.h"

QT_BEGIN_NAMESPACE
class QWidget;
class QCompleter;
class QKeyEvent;
class QScrollBar;
QT_END_NAMESPACE

namespace LimeReport{

class CodeEditor :public QPlainTextEdit
{
    Q_OBJECT
public:
    CodeEditor(QWidget* parent=0);
    void setCompleter(QCompleter* value);
    QCompleter* compleater() const{ return m_completer;}
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    void resizeEvent(QResizeEvent *event);
private:
    QString textUnderCursor() const;
    bool    matchLeftParenthesis(QTextBlock currentBlock, QChar parenthesisType, int i, int numLeftParentheses);
    bool    matchRightParenthesis(QTextBlock currentBlock, QChar parenthesisType, int i, int numRightParentheses);
    void    createParenthesisSelection(int pos);
    bool    charIsParenthesis(QChar character, ParenthesisType type);
    QChar   getParenthesisReverceChar(QChar parenthesisChar);
private slots:
    void insertCompletion(const QString& completion);
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void matchParentheses();
private:
    QCompleter* m_completer;
    QWidget *lineNumberArea;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

} // namespace LimeReport

#endif // LRCODEEDITOR_H
