#ifndef MAGISTERKA_LINKEDLISTNODE_H
#define MAGISTERKA_LINKEDLISTNODE_H

#include <cstdlib>

class LinkedListNode {
public:
    LinkedListNode *prev, *next;

    LinkedListNode() {
        prev = next = NULL;
    }

    virtual ~LinkedListNode() {}
};

#endif //MAGISTERKA_LINKEDLISTNODE_H
