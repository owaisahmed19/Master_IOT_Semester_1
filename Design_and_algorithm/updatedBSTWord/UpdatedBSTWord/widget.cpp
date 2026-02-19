#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget) {
    ui->setupUi(this);

    root = new BST();
    loadFile("../../words.txt");

    /*
    // Initialize BST root
    root = new BST();

    // Insert some test words
    root->insert("apple");
    root->insert("bear");
    root->insert("cat");
    root->insert("dog");

    // Example search
    int steps = 0;
    bool found = root->search("dog", steps);
    qDebug() << "Found dog?" << found << "in" << steps << "steps";
    qDebug() << root->getWord();
    */


/*
    root = new BST();
    root = root->insert("apple");
    root = root->insert("bear");
    root = root->insert("cat");
    root = root->insert("dog");
    root = root->insert("elephant");

    root->printBalances();

    qDebug() << "Balanced BST:";
    root->printTree();

*/

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
    int n = 0;

    bool found = root->search(ui->entry->text().toUpper(), n);
    ui->txt_answer->setText(found ? "YES" : "NO");

    ui->txt_steps->setText(QString::number(n));

    int totalWords = root->countNodes();
    int treeDepth = root->depth();

    ui->label_3->setText(
        QString("Total words: %1\nBST Depth: %2").arg(totalWords).arg(treeDepth)
        );
}



void Widget::on_entry_returnPressed() {
    on_pb_search_clicked();
}

