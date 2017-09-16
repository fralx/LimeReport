#include "lrscripthighlighter.h"
#include <QDebug>

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
enum States {MayBeKeyWord, Code, MayBeComment, Comment, Comment2, MayBeComment2End, String, String2, MayBeNumber, Separator, StatesCount};

void ScriptHighlighter::highlightBlock(const QString& text)
{
    int literal = -1;
    bool lastWasBackSlash = false;
    int state = previousBlockState() != -1 ? previousBlockState() : MayBeKeyWord ;
    int oldState = -1;
    int stateMaschine[StatesCount][LiteralsCount] = {
        {Separator, MayBeKeyWord, Code, Separator, MayBeComment, Separator, Separator, String, String2, Separator, Separator},
        {Separator, Code, Code, Separator, Separator, Separator, Separator, String, String2, Separator, Separator},
        {Separator, Code, Code, Code, Comment, Comment2, Code, String, String2, Separator, Code},
        {Comment, Comment, Comment, Comment, Comment, Comment, Comment, Comment, Comment, Comment, Comment},
        {Comment2, Comment2, Comment2, Comment2, Comment2, MayBeComment2End, Comment2, Comment2, Comment2, Comment2, Comment2},
        {Comment2, Comment2, Comment2, Comment2, Separator, Comment2, Comment2, Comment2, Comment2, Comment2, Comment2},
        {String, String, String, String, String, String, String, Separator, String, String, String},
        {String2, String2, String2, String2, String2, String2, String2, String2, Separator, String2, String2},
        {Separator, Code, MayBeNumber, Separator, MayBeComment, Separator, Separator, String, String2, Separator, Code},
        {Separator, MayBeKeyWord, MayBeNumber, Separator, MayBeComment, String, String2, Separator, Separator }
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

        if (oldState != state){
            switch( state ){
                case MayBeComment:
                    buffer.clear();
                    buffer += currentChar;
                    break;
                case Comment2:
                    buffer += currentChar;
                    qDebug()<<buffer;
                    break;
                case Separator:
                    switch(oldState){
                        case Comment2:
                            qDebug()<<buffer;
                            buffer.clear();
                            break;
                    }
                default:
                    break;
            }
        } else {
            buffer += currentChar;
        }

        i++;
        if ( i>= text.length()) break;
    }
}

} // namespace LimeReport
