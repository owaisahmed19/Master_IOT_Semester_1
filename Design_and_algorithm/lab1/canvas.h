#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);

private slots:
    void onButtonClicked();

private:
    QLabel *titleLabel;
    QPushButton *button;
    QLabel *counterLabel;
    QWidget *redBox;
    int counter = 0;
};

#endif // CANVAS_H
