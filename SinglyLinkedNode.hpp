// SinglyLinkedNode.hpp -- PROVIDED. One node of a singly-linked chain: a value and a pointer to the next
// node. BOTH lists (SinglyLinkedList and CircularSinglyLinkedList) use this same node type -- which is
// exactly what lets you MOVE a node from one list into the other by relinking, with no copy of the value.
#ifndef SINGLY_LINKED_NODE_HPP
#define SINGLY_LINKED_NODE_HPP

template<typename T>
struct SinglyLinkedNode {
    T                    data;
    SinglyLinkedNode<T>* next;
    SinglyLinkedNode(T const& d, SinglyLinkedNode<T>* n = nullptr) : data(d), next(n) {}

    // Walk a null-terminated chain and delete every node (used by list destructors).
    static void deleteChain(SinglyLinkedNode<T>* head) {
        while (head != nullptr) { SinglyLinkedNode<T>* nx = head->next; delete head; head = nx; }
    }
};

#endif  // SINGLY_LINKED_NODE_HPP
