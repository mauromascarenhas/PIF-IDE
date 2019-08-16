#ifndef PIFEDITOR_H
#define PIFEDITOR_H

#include <QWidget>
#include <QTextEdit>
#include <QKeyEvent>
#include <QTextBlock>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class PIFEditor : public QTextEdit
{
public:
    explicit PIFEditor(QWidget *parent = nullptr);
    ~PIFEditor();

    QRegularExpression prevTabulation;
    QRegularExpression hasNewBlockE;
    QRegularExpression hasNewBlock;

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void returnPressed();
};

#endif // PIFEDITOR_H
