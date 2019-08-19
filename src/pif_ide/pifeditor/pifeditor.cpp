#include "pifeditor.h"

PIFEditor::PIFEditor(QWidget *parent) : QTextEdit(parent),
    prevTabulation("^(\\t*)\\.*"),
    hasNewBlockE("^\\t*\\b(se|senão se|senão|enquanto|faça|programa)\\b(.*)"),
    hasNewBlock("^\\t*\\b(se|senão se|senão|enquanto|faça|programa)\\b\\s(.*)")
{
    cplt = nullptr;
    removeCompleter = true;
}

PIFEditor::~PIFEditor(){
    if (cplt && removeCompleter) delete cplt;
}

void PIFEditor::setCompleter(QCompleter *completer, bool deleteOld){
    if (cplt) {
        disconnect(cplt, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
        if (removeCompleter = deleteOld) delete cplt;
    }

    if(!completer) return;

    cplt = completer;
    cplt->setWidget(this);
    cplt->popup()->setFont(this->font());
    cplt->setCaseSensitivity(Qt::CaseInsensitive);
    cplt->setCompletionMode(QCompleter::PopupCompletion);
    connect(cplt, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void PIFEditor::insertCompletion(const QString &text){
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::Left);
    cursor.movePosition(QTextCursor::EndOfWord);
    cursor.insertText(text.right(text.length() - cplt->completionPrefix().length()));
    this->setTextCursor(cursor);
}

void PIFEditor::focusInEvent(QFocusEvent *event){
    if (cplt) cplt->setWidget(this);
    QTextEdit::focusInEvent(event);
}

void PIFEditor::keyPressEvent(QKeyEvent *event){

    if (cplt && cplt->popup()->isVisible()){
        switch (event->key()) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                event->ignore();
                return;
            default:
                break;
        }
    }

    bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Space);
    if (!cplt || !isShortcut){
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
                    if (matchTabs.hasMatch() && matchTabs.captured(1).size()){
                        if (this->textCursor().blockNumber() > 1){
                            QRegularExpressionMatch matchPrevTabs = prevTabulation.match(this->document()->findBlockByNumber(this->textCursor().blockNumber() - 2).text());
                            if (matchPrevTabs.hasMatch() && matchPrevTabs.captured(1).size() >= matchTabs.captured(1).size()){
                                textCursor.insertText("\t");
                            }
                            else {
                                textCursor.movePosition(QTextCursor::Up);
                                textCursor.movePosition(QTextCursor::StartOfLine);
                                textCursor.deleteChar();
                                textCursor.movePosition(QTextCursor::Down);
                                textCursor.movePosition(QTextCursor::EndOfLine);
                            }
                        }
                        else textCursor.insertText("\t");
                    }
                    else textCursor.insertText("\t");
                }
                else {
                    textCursor.insertText("\t");
                    match = prevTabulation.match(this->document()->findBlockByNumber(textCursor.blockNumber() - 1).text());
                    textCursor.insertText(match.hasMatch() ? match.captured(1) : "");
                }
            }
            else if (match.hasMatch()){
                textCursor.movePosition(QTextCursor::Up);
                textCursor.movePosition(QTextCursor::EndOfLine);
                textCursor.insertText(" ");
                textCursor.movePosition(QTextCursor::Down);
                textCursor.movePosition(QTextCursor::EndOfLine);
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
                    if (matchTabs.hasMatch() && matchTabs.captured(1).size()){
                        if (this->textCursor().blockNumber() > 1){
                            QRegularExpressionMatch matchPrevTabs = prevTabulation.match(this->document()->findBlockByNumber(this->textCursor().blockNumber() - 2).text());
                            if (matchPrevTabs.hasMatch() && matchPrevTabs.captured(1).size() >= matchTabs.captured(1).size()){
                                textCursor.insertText("\t");
                                textCursor.movePosition(QTextCursor::Up);
                            }
                            else {
                                textCursor.movePosition(QTextCursor::Up);
                                textCursor.movePosition(QTextCursor::StartOfLine);
                                textCursor.deleteChar();
                            }
                        }
                        else {
                            textCursor.movePosition(QTextCursor::Up);
                            textCursor.movePosition(QTextCursor::StartOfLine);
                            textCursor.deleteChar();
                        }
                    }
                    else {
                        textCursor.insertText("\t");
                        textCursor.movePosition(QTextCursor::Up);
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

    const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!cplt || (ctrlOrShift && event->text().isEmpty())) return;

    bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QTextCursor cursor = this->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString completionPrefix = cursor.selectedText();

    if (!isShortcut && (hasModifier || event->text().isEmpty()|| completionPrefix.length() < 3
                        || event->text().right(1).contains(QRegularExpression("[^\\p{L}]")))) {
        cplt->popup()->hide();
        return;
    }

    if (completionPrefix != cplt->completionPrefix()) {
        cplt->setCompletionPrefix(completionPrefix);
        cplt->popup()->setCurrentIndex(cplt->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();
    cr.setWidth(cplt->popup()->sizeHintForColumn(0)
                + cplt->popup()->verticalScrollBar()->sizeHint().width());
    cplt->complete(cr);
}
