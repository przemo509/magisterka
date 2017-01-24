#ifndef MAGISTERKA_LINKEDLIST_H
#define MAGISTERKA_LINKEDLIST_H

#include <cstdlib>
#include "LinkedListNode.h"

class LinkedList {
private:
    LinkedListNode *first, *last;
    int currentCount, createdCount, deletedCount;

    void removeAll() {
        while (last != NULL) {
            remove(last);
        }
    }

protected:
    LinkedList() {
        first = last = NULL;
        currentCount = createdCount = deletedCount = 0;
    }

    virtual ~LinkedList() {
        removeAll();
    }

    LinkedListNode *getFirst() const {
        return first;
    }

    void add(LinkedListNode *node) {
        if (first == NULL) {
            first = node;
        } else { // <=> last != NULL
            last->next = node;
        }
        last = node;
        currentCount++;
        createdCount++;
    }

    void remove(LinkedListNode *node) {
        if (node->prev != NULL) node->prev->next = node->next;
        if (node->next != NULL) node->next->prev = node->prev;
        if (node == first) first = node->next;
        if (node == last) last = node->prev;
        delete node;
        deletedCount++;
        currentCount--;
    }

public:
    int getCurrentCount() {
        return currentCount;
    }

    int getCreatedCount() const {
        return createdCount;
    }

    int getDeletedCount() const {
        return deletedCount;
    }
};

#endif //MAGISTERKA_LINKEDLIST_H
