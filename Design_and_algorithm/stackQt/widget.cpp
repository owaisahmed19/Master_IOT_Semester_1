#include "widget.h"
#include <QPainter>
#include <QMouseEvent>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    for (int i = 0; i < std::size(colors); i++) {
        myStack.push(new MyData((i+1), colors[i].first, colors[i].second));
    }
    setMinimumSize(500, 400);
}

Widget::~Widget()
{
    while (!myStack.isEmpty()) {
        delete myStack.pop();
    }
    qDeleteAll(popped);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    MyData* d = myStack.pop();
    if (d) {
        popped.append(d);
    }
    update();
}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    int margin = 50;
    int w = 160;
    int h = 40;
    int spacing = 10;

    int xStack = margin;
    int yStack = margin;

    QVector<MyData*> drawStack;
    Stack<MyData> temp;

    while (!myStack.isEmpty()) {
        MyData* d = myStack.pop();
        drawStack.append(d);
        temp.push(d);
    }

    for (MyData* d : drawStack) {
        painter.setBrush(d->getColor());
        painter.setPen(Qt::black);
        painter.drawRect(xStack, yStack, w, h);
        painter.drawText(QRect(xStack, yStack, w, h), Qt::AlignCenter, d->print());
        yStack += h + spacing;
    }

    while (!temp.isEmpty()) {
        myStack.push(temp.pop());
    }

    int xPop = width() - w - margin;
    int yPop = margin;

    for (MyData* d : std::as_const(popped)) {
        painter.setBrush(d->getColor());
        painter.setPen(Qt::black);
        painter.drawRect(xPop, yPop, w, h);
        painter.drawText(QRect(xPop, yPop, w, h), Qt::AlignCenter, d->print());
        yPop += h + spacing;
    }
}
