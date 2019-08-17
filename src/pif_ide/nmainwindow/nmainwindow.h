/*-------------------------------------------------
#
# Project developed by Nintersoft team
# Developer: Mauro Mascarenhas de Araújo
# Contact: mauro.mascarenhas@nintersoft.com
# License: Nintersoft Open Source Code Licence
# Date: 31 of May of 2018
#
------------------------------------------------- */

#ifndef NMAINWINDOW_H
#define NMAINWINDOW_H

#include <QPoint>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMouseEvent>

#include "titlebar.h"

namespace Ui {
class NMainWindow;
}

class NMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NMainWindow(QWidget *parent = nullptr);
    ~NMainWindow();

    void setCustomWidgets(QWidget *newCentralWidget, QStatusBar *newStatusBar);
    void setNewCentralWidget(QWidget *newCentralWidget);
    void setCustomStatusBar(QStatusBar *newStatusBar);

    void setCloseButtonEnabled(bool enable);
    void setMaximizeButtonEnabled(bool enable);
    void setMinimizeButtonEnabled(bool enable);

protected:
    void closeEvent(QCloseEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void undefMouseMoveEvent(QObject *object, QMouseEvent* event);
    bool eventFilter(QObject *watched, QEvent *event);
    
    enum LockMoveType{
        Left,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        None
    };

private:
    Ui::NMainWindow *ui;
    const int RESIZE_LIMIT;

    QPoint posCursor;
    LockMoveType locked;
};

#endif // NMAINWINDOW_H
