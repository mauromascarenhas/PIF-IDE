#include "pifeditor.h"

PIFEditor::PIFEditor(QWidget *parent) : QTextEdit(parent),
    prevTabulation("^(\\t*)\\.*"),
    hasNewBlockE("^\\t*\\b(se|senão se|senão|enquanto|faça|programa)\\b(.*)"),
    hasNewBlock("^\\t*\\b(se|senão se|senão|enquanto|faça|programa)\\b\\s(.*)")
{

}

PIFEditor::~PIFEditor(){

}

void PIFEditor::keyPressEvent(QKeyEvent *event){
    QTextEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Return){
        QTextCursor textCursor = this->textCursor();
        textCursor.beginEditBlock();
        QRegularExpressionMatch match = hasNewBlockE.match(this->document()->findBlockByNumber(textCursor.blockNumber() - 1).text());
        if (match.hasMatch() && match.captured(2).isEmpty() && this->textCursor().block().text().isEmpty()) {
            QString structure = match.captured(1);
            QRegularExpressionMatch matchTabs = prevTabulation.match(this->document()->findBlockByNumber(this->textCursor().blockNumber() - 1).text());
            if (structure == "faça"){
                textCursor.deletePreviousChar();
                textCursor.insertText(QString(" \n"
                                              "%1\t\n"
                                              "%1enquanto CONDIÇÃO").arg(matchTabs.hasMatch() ? matchTabs.captured(1) : ""));
                textCursor.select(QTextCursor::WordUnderCursor);
            }
            else if (structure == "programa"){
                textCursor.deletePreviousChar();
                textCursor.insertText(QString(" NOME_DO_PROGRAMA\n"
                                              "%1\t\n"
                                              "%1fim-programa").arg(matchTabs.hasMatch() ? matchTabs.captured(1) : ""));
                textCursor.movePosition(QTextCursor::Up);
                textCursor.movePosition(QTextCursor::EndOfLine);
            }
            else if (structure == "se") {
                textCursor.deletePreviousChar();
                textCursor.insertText(QString(" CONDIÇÃO então\n"
                                              "%1\t\n"
                                              "%1fim-se").arg(matchTabs.hasMatch() ? matchTabs.captured(1) : ""));
                textCursor.movePosition(QTextCursor::Up);
                textCursor.movePosition(QTextCursor::EndOfLine);
            }
            else if (structure == "enquanto") {
                textCursor.deletePreviousChar();
                textCursor.insertText(QString(" CONDIÇÃO faça\n"
                                              "%1\t\n"
                                              "%1fim-enquanto").arg(matchTabs.hasMatch() ? matchTabs.captured(1) : ""));
                textCursor.movePosition(QTextCursor::Up);
                textCursor.movePosition(QTextCursor::EndOfLine);
            }
            else if (structure == "senão"){
                textCursor.insertText(matchTabs.hasMatch() ? matchTabs.captured(1) : "");
                if (matchTabs.hasMatch() && matchTabs.captured(1).split("\t").size()){
                    textCursor.movePosition(QTextCursor::Up);
                    textCursor.movePosition(QTextCursor::StartOfLine);
                    textCursor.deleteChar();
                    textCursor.movePosition(QTextCursor::Down);
                    textCursor.movePosition(QTextCursor::EndOfLine);
                }
            }
            else {
                textCursor.insertText("\t");
                match = prevTabulation.match(this->document()->findBlockByNumber(textCursor.blockNumber() - 1).text());
                textCursor.insertText(match.hasMatch() ? match.captured(1) : "");
            }
        }
        else if (match.hasMatch()){
            textCursor.insertText("\t");
            match = prevTabulation.match(this->document()->findBlockByNumber(textCursor.blockNumber() - 1).text());
            textCursor.insertText(match.hasMatch() ? match.captured(1) : "");
        }
        else {
            match = prevTabulation.match(this->document()->findBlockByNumber(textCursor.blockNumber() - 1).text());
            textCursor.insertText(match.hasMatch() ? match.captured(1) : "");
        }
        textCursor.endEditBlock();
        this->setTextCursor(textCursor);
    }
    else if (event->key() == Qt::Key_Space){
        QRegularExpressionMatch match = hasNewBlock.match(this->document()->findBlockByNumber(this->textCursor().blockNumber()).text());
        if (match.hasMatch() && match.captured(2).isEmpty()){
            QRegularExpressionMatch matchTabs = prevTabulation.match(this->document()->findBlockByNumber(this->textCursor().blockNumber()).text());

            QTextCursor textCursor = this->textCursor();
            textCursor.beginEditBlock();
            QString structure = match.captured(1);
            if (structure == "programa"){
                textCursor.insertText(QString("NOME_DO_PROGRAMA\n"
                                              "%1\t\n"
                                              "%1fim-programa").arg(matchTabs.hasMatch() ? matchTabs.captured(1) : ""));
                textCursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, 2);
                textCursor.movePosition(QTextCursor::EndOfLine);
                textCursor.select(QTextCursor::WordUnderCursor);
            }
            else if (structure == "se") {
                textCursor.insertText(QString("CONDIÇÃO então\n"
                                              "%1\t\n"
                                              "%1fim-se").arg(matchTabs.hasMatch() ? matchTabs.captured(1) : ""));
                textCursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, 2);
                textCursor.movePosition(QTextCursor::EndOfLine);
                textCursor.movePosition(QTextCursor::WordLeft, QTextCursor::MoveAnchor, 2);
                textCursor.select(QTextCursor::WordUnderCursor);
            }
            else if (structure == "enquanto") {
                textCursor.insertText(QString("CONDIÇÃO faça\n"
                                              "%1\t\n"
                                              "%1fim-enquanto").arg(matchTabs.hasMatch() ? matchTabs.captured(1) : ""));
                textCursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, 2);
                textCursor.movePosition(QTextCursor::EndOfLine);
                textCursor.movePosition(QTextCursor::WordLeft, QTextCursor::MoveAnchor, 2);
                textCursor.select(QTextCursor::WordUnderCursor);
            }
            else if (structure == "faça") {
                textCursor.insertText(QString("\n"
                                              "%1\t\n"
                                              "%1enquanto CONDIÇÃO").arg(matchTabs.hasMatch() ? matchTabs.captured(1) : ""));
                textCursor.select(QTextCursor::WordUnderCursor);
            }
            else if (structure == "senão") {
                textCursor.insertText(QString("se CONDIÇÃO então\n"
                                              "%1").arg(matchTabs.hasMatch() ? matchTabs.captured(1) : ""));
                textCursor.movePosition(QTextCursor::Up);
                if (matchTabs.hasMatch() && matchTabs.captured(1).size()){
                    textCursor.movePosition(QTextCursor::StartOfLine);
                    textCursor.deleteChar();
                }
                textCursor.movePosition(QTextCursor::EndOfLine);
                textCursor.movePosition(QTextCursor::WordLeft, QTextCursor::MoveAnchor, 2);
                textCursor.select(QTextCursor::WordUnderCursor);
            }
            textCursor.endEditBlock();
            this->setTextCursor(textCursor);
        }
    }
}
