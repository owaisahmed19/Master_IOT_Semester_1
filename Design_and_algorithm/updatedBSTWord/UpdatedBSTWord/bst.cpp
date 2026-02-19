

#include "bst.h"
#include <QDebug>

BST::BST(): left(nullptr), right(nullptr), parent(nullptr), nodeWord(nullptr) {}
BST::BST(const QString &word, BST* p, BST* l, BST* r): left(l), right(r), parent(p) {
    nodeWord = new Word(word);
}
BST::~BST() {
    delete nodeWord;
    delete left;
    delete right;
}

int BST::countNodes() const {
    if (!nodeWord) return 0;
    int leftCount = left ? left->countNodes() : 0;
    int rightCount = right ? right->countNodes() : 0;
    return 1 + leftCount + rightCount;
}



/*
BST* BST::insert(const QString &word) {
    if (nodeWord==nullptr) {
        nodeWord = new Word(word);
    } else if (!(*nodeWord==word)) {
        parent = new BST(word,nullptr,this,nullptr);
        return parent;
    }
    return this;
}
*/

/*
BST* BST::insert(const QString &word) {
    if (nodeWord == nullptr) {
        nodeWord = new Word(word);
        return this;
    }

    if (*nodeWord == word) {
        // already exists, do nothing
        return this;
    } else if (*nodeWord < word) {
        // go right
        if (right == nullptr) {
            right = new BST(word, this, nullptr, nullptr);
        } else {
            right->insert(word);
        }
    } else {
        // go left
        if (left == nullptr) {
            left = new BST(word, this, nullptr, nullptr);
        } else {
            left->insert(word);
        }
    }
    return this;
}

*/





/*
void BST::printTree(int depth) const {
    if (nodeWord == nullptr) return;

    // Print right subtree first
    if (right != nullptr) {
        right->printTree(depth + 1);
    }

    // Indentation based on depth
    for (int i = 0; i < depth; i++) {
        qDebug().noquote() << "    ";  // 4 spaces per level
    }
    qDebug().noquote() << nodeWord->get();

    // Print left subtree
    if (left != nullptr) {
        left->printTree(depth + 1);
    }
}
*/

/*
bool BST::search(const QString &src,int &n) {
    n++;
    auto ptr=this;
    while (ptr!=nullptr && !(*(ptr->nodeWord)==src)) {
        ptr=ptr->left;
        n++;
    }
    return (ptr!=nullptr);
}
*/



int BST::balanceFactor() const {
    int lh = left ? left->depth() : 0;
    int rh = right ? right->depth() : 0;
    return rh - lh;
}


int BST::depth() const {
    int leftDepth = left ? left->depth() : 0;
    int rightDepth = right ? right->depth() : 0;
    return 1 + std::max(leftDepth, rightDepth);
}

BST* BST::balance() {
    int leftDepth = left ? left->depth() : 0;
    int rightDepth = right ? right->depth() : 0;
    int b = rightDepth - leftDepth;

    if (b > 1) {
        if (right && right->left && (right->left->depth() > (right->right ? right->right->depth() : 0))) {
            right = right->rotateRight();
        }
        return rotateLeft();
    }

    if (b < -1) {
        if (left && left->right && (left->right->depth() > (left->left ? left->left->depth() : 0))) {
            left = left->rotateLeft();
        }
        return rotateRight();
    }

    return this;
}

BST* BST::rotateLeft() {
    BST* newRoot = right;
    right = newRoot->left;
    if (newRoot->left) newRoot->left->parent = this;
    newRoot->left = this;

    newRoot->parent = parent;
    parent = newRoot;

    return newRoot;
}

BST* BST::rotateRight() {
    BST* newRoot = left;
    left = newRoot->right;
    if (newRoot->right) newRoot->right->parent = this;
    newRoot->right = this;

    newRoot->parent = parent;
    parent = newRoot;

    return newRoot;
}


BST* BST::insert(const QString &word) {
    if (nodeWord == nullptr) {
        nodeWord = new Word(word);
        return this;
    }

    if (*nodeWord == word) return this; // no duplicates

    if (nodeWord->operator<(word)) {
        if (right == nullptr) {
            right = new BST(word, this, nullptr, nullptr);
        } else {
            right = right->insert(word);
        }
    } else {
        if (left == nullptr) {
            left = new BST(word, this, nullptr, nullptr);
        } else {
            left = left->insert(word);
        }
    }

    return balance();
}


bool BST::search(const QString &src, int &n) {
    n++;
    if (!nodeWord) return false;
    if (*nodeWord == src) return true;
    if (*nodeWord < src) return right ? right->search(src, n) : false;
    return left ? left->search(src, n) : false;
}

void BST::printTree(int depth) const {
    if (!nodeWord) return;
    if (right) right->printTree(depth + 1);
    QString indent;
    for (int i=0;i<depth;i++) indent += "    ";
    qDebug().noquote() << indent + nodeWord->get();
    if (left) left->printTree(depth + 1);
}

void BST::printBalances() const {
    if (!nodeWord) return;

    qDebug() << "\"" << getWord() << "\" (balance:" << balanceFactor() << ")";

    if (left) left->printBalances();
    if (right) right->printBalances();
}

