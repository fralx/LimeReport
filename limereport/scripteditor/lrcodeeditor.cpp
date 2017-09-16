#include "lrcodeeditor.h"

#include <QAbstractItemView>
#include <QWidget>
#include <QCompleter>
#include <QKeyEvent>
#include <QScrollBar>
#include <QPainter>
#include <QTextBlock>
#include <QDebug>

#include "lrscripthighlighter.h"

namespace LimeReport{

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent), m_compleater(0)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    (void) new ScriptHighlighter(document());
}

void CodeEditor::setCompleter(QCompleter *value)
{
    if (value) disconnect(value,0,this,0);
    m_compleater = value;
    if (!m_compleater) return;
    m_compleater->setWidget(this);
    m_compleater->setCompletionMode(QCompleter::PopupCompletion);
    m_compleater->setCaseSensitivity(Qt::CaseInsensitive);
    connect(m_compleater,SIGNAL(activated(QString)),this,SLOT(insertCompletion(QString)));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QPalette().background().color());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QPalette().text().color());
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = fontMetrics().width(QLatin1Char('9'))*2 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    if (m_compleater && m_compleater->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
    if (!m_compleater || !isShortcut) QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!m_compleater || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;

    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        m_compleater->popup()->hide();
        return;
    }

    if (completionPrefix != m_compleater->completionPrefix()) {
        m_compleater->setCompletionPrefix(completionPrefix);
        m_compleater->popup()->setCurrentIndex(m_compleater->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();
    cr.setWidth(m_compleater->popup()->sizeHintForColumn(0)
                + m_compleater->popup()->verticalScrollBar()->sizeHint().width());
    m_compleater->complete(cr);

}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    if (m_compleater) m_compleater->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CodeEditor::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

QString CodeEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::insertCompletion(const QString &completion)
{
    if (m_compleater->widget() != this)
             return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_compleater->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(QPalette().background().color()).darker(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

} //namespace LimeReport
