#ifndef LRCODEEDITOR_H
#define LRCODEEDITOR_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>

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
    QCompleter* compleater() const{ return m_compleater;}
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    void resizeEvent(QResizeEvent *event);
private:
    QString textUnderCursor() const;
private slots:
    void insertCompletion(const QString& completion);
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
private:
    QCompleter* m_compleater;
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
