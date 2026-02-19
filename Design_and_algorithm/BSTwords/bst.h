#ifndef BST_H
#define BST_H
#include <QString>
#include <word.h>
class BST {
public:
    BST();
    ~BST();
    /**
     * @brief insert a new word in the tree
     * @param word to insert
     */
    BST *insert(const QString &word);
    BST* balance();
    int count() const;
    /**
     * @brief search if word src string in the tree.
     * @param src: word searched.
     * @return true if found.
     */
    bool search(const QString &src,int &n);

    QString print();



    QString getWord() const { return (nodeWord==nullptr?"NULL":nodeWord->get());}
private:
    BST(const QString &word, BST* p, BST* l, BST* r);
    BST *left,*right,*parent;
    Word *nodeWord;
};

#endif // BST_H
