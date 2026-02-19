#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "StackFile.h"
#include "MDataFile.h"
#include "MyColorFile.h"

class Widget : public QWidget
{
    Q_OBJECT
    Stack<MyData> myStack;
    QVector<MyData*> popped;

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // WIDGET_H
