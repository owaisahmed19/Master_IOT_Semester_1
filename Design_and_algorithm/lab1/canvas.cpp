#include "canvas.h"
#include <QPalette>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent)
{
    // Create widgets
    titleLabel = new QLabel("Counter App", this);
    button = new QPushButton("Click Me!", this);
    counterLabel = new QLabel("0", this);

    // Create the red box widget
    redBox = new QWidget(this);
    redBox->setFixedHeight(50); // give it some height
    redBox->setStyleSheet("background-color: red;");

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    layout->addWidget(button, 0, Qt::AlignHCenter);
    layout->addWidget(counterLabel, 0, Qt::AlignHCenter);
    layout->addWidget(redBox);  // add the red box at the bottom

    setLayout(layout);

    // Connect button signal to slot
    connect(button, &QPushButton::clicked, this, &Canvas::onButtonClicked);
}

void Canvas::onButtonClicked()
{
    counter++;
    counterLabel->setText(QString::number(counter));
}
