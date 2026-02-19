#include "stack.h"
#include <QString>
#include <QColor>
#include <QPair>
#include <iostream>

// Predefined color array
const QPair<QString,QColor> colors[12] = {
    {"red",0xFF0000}, {"orange",0xFFA500},{"yellow",0xFFFF00},
    {"green",0x00FF00}, {"blue",0x0000FF}, {"indigo",0x00FFFF},
    {"pink",0xFFC0CB}, {"brown",0xA52A2A},{"black",0x000000}, {"white",0xFFFFFF},
    {"magenta",0xFF00FF}, {"grey",0x777777}
};

template<class T>
class Element {
public:
    T data;
    Element<T> *next;

    Element(T &_data, Element<T> *_next = nullptr)
        : data(_data), next(_next) {}

    QString print() { return data.print(); }
};

template<class T>
class Stack {
    Element<T> *top;

public:
    Stack() : top(nullptr) {}

    ~Stack() {
        while (top) {
            pop();
        }
    }

    void push(T &value) {
        top = new Element<T>(value, top);
    }

    T pop() {
        if (!top) {
            throw std::runtime_error("Stack underflow!");
        }
        Element<T>* node = top;
        T value = node->data;
        top = top->next;
        delete node;
        return value;
    }

    int size() {
        int count = 0;
        Element<T>* current = top;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }

    void print() {
        Element<T>* current = top;
        while (current) {
            std::cout << current->data.print().toStdString() << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};
