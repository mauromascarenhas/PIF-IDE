#ifndef PIFEDITOR_H
#define PIFEDITOR_H

#include <QRect>
#include <QWidget>
#include <QTextEdit>
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>
#include <QScrollBar>
#include <QFocusEvent>
#include <QAbstractItemView>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class PIFEditor : public QTextEdit
{
    Q_OBJECT

public:
    explicit PIFEditor(QWidget *parent = nullptr);
    ~PIFEditor();

    QRegularExpression prevTabulation;
    QRegularExpression hasNewBlockE;
    QRegularExpression hasNewBlock;

    void setCompleter(QCompleter *completer, bool deleteOld = true);
    inline QCompleter *completer() { return this->cplt; }

private:
    QCompleter *cplt;
    bool removeCompleter;

private slots:
    void insertCompletion(const QString &text);

protected:
    void focusInEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void returnPressed();
};

#endif // PIFEDITOR_H
