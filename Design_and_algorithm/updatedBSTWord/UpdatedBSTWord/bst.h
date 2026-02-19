#ifndef BST_H
#define BST_H
#include <QString>
#include <word.h>

class BST {
public:
    BST();
    BST(const QString &word, BST* p=nullptr, BST* l=nullptr, BST* r=nullptr);
    ~BST();

    int countNodes() const;
    BST* insert(const QString &word);
    bool search(const QString &src,int &n);
    int balanceFactor() const;
    void printBalances() const;
    int depth() const;
    BST* balance();
    BST* rotateLeft();
    BST* rotateRight();
    void printTree(int depth = 0) const;
    QString getWord() const { return nodeWord ? nodeWord->get() : "NULL"; }

private:
    BST *left, *right, *parent;
    Word *nodeWord;
};

#endif
