#include "bst.h"
#include <QDebug>
BST::BST():left(nullptr),right(nullptr),parent(nullptr),nodeWord(nullptr) {
}

BST::BST(const QString &word, BST* p, BST*l, BST*r):
    left(l),right(r),parent(p) {

    nodeWord = new Word(word);
};

BST::~BST() {
    delete nodeWord;
    delete left;
    delete right;
}

BST* BST::insert(const QString &word) {
    /*if (nodeWord==nullptr) {
        nodeWord = new Word(word);
    } else if (!(*nodeWord==word)) {
        parent = new BST(word,nullptr,this,nullptr);
        return parent;
    }
*/

    if(nodeWord==nullptr){
        nodeWord=new Word(word);
        return this;
    }

    if(*nodeWord<word){
        if(left!=nullptr){
            left=insert(word);
        }else{
            left=new BST(word, this, nullptr,nullptr);
        }
    }
    else{
        if(right!=nullptr){
            right=insert(word);
        }else{
            if(left == nullptr){
                auto newNode=new BST(word,parent,this, nullptr);
                parent=newNode;
                return newNode;
            }else{
                right =new BST(word, this, nullptr, nullptr);
            }
        }
    }
    return this;
}


QString BST::print(){
    return nodeWord -> get()+"|" +
           (left ? left->print() : "-") + "|" +
           (right ? right->print() : "-");
}


BST* BST::balance() {
    if (right->count() < left->count() + 2) return this;
    BST* newRoot = right;
    newRoot->parent = this;
    right = newRoot->left;
    return newRoot;
}


int BST::count() const{
    return 1+(left?left->count():0)+(right?right->count():0);
}




bool BST::search(const QString &src,int &n) {
    n++;
    auto ptr=this;
    while (ptr!=nullptr && !(*(ptr->nodeWord)==src)) {
        ptr=ptr->left;
        n++;
    }
    return (ptr!=nullptr);
}
