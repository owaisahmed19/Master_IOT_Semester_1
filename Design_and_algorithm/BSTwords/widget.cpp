#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget) {
    ui->setupUi(this);

    root = new BST();
//  loadFile("../../words.txt");

    root=root->insert("apple");
    root=root->insert("bear");
    root=root->insert("cat");
    root=root->insert("dog");
    root=root->insert("elephant");
    root=root->balance();
    qDebug() << root-> print();


}

Widget::~Widget() {
    delete ui;
    delete root;
}

bool Widget::loadFile(const QString &fileName) {
    QFile file(fileName);
    int i=0;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString word=in.readLine().toUpper();
            if (word!="") {
                root = root->insert(word.toUpper());
                i++;
            }
        }
    } else {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","File "+fileName+" not reachable!");
        return false;
    }
    qDebug() << "words:" << i;
    file.close();
    return true;
}

void Widget::on_pb_search_clicked() {
    int n=0;
    if (root->search(ui->entry->text().toUpper(),n)) {
        ui->txt_answer->setText("YES");
    } else {
        ui->txt_answer->setText("NO");
    }
    ui->txt_steps->setText(QString::number(n));
}

void Widget::on_entry_returnPressed() {
    on_pb_search_clicked();
}

