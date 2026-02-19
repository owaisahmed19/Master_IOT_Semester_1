#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    line1.fill(Qt::gray, nbColors);
    line1[0] = QColor(144, 238, 144);
    line1[1] = QColor(0, 100, 0);

    line2.fill(Qt::gray, nbColors);
    line2[0] = QColor(255, 165, 0);
    line2[1] = QColor(255, 255, 0);
    line2[2] = Qt::red;

    line3.fill(Qt::gray, nbColors);
    line3[0] = Qt::cyan;
    line3[1] = Qt::blue;
    line3[2] = Qt::magenta;
    line3[3] = Qt::black;
    line3[4] = QColor(255, 255, 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    const int circleSize = 40;
    const int spacingY   = 60;
    const int topMargin  = 50;
    const int paddingX   = 20;

    auto drawRow = [&](const QVector<QColor> &line, int rowIndex) {
        int rowWidth = line.size() * circleSize;
        int startX = paddingX + (width() - 2 * paddingX - rowWidth) / 2;
        int y = topMargin + rowIndex * spacingY;

        for (int i = 0; i < line.size(); ++i) {
            int x = startX + i * circleSize;
            painter.setBrush(line[i]);
            painter.setPen(Qt::black);
            painter.drawEllipse(x, y, circleSize, circleSize);
        }
    };

    drawRow(line1, 0);
    drawRow(line2, 1);
    drawRow(line3, 2);
}
