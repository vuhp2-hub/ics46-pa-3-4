// Queue.hpp -- PROVIDED, COMPLETE. A FIFO queue (own node chain, head + tail). enqueue (void) /
// dequeue (returns front) / peek / isEmpty / size. In PA 3,4 the incoming requests (add a meal, set a
// course, remove the lowest) wait in a Queue and are applied in arrival order.
#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <utility>   // std::swap

template<typename T>
class Queue {
    private:
        struct Node { T data; Node* next; Node(T const& d, Node* n) : data(d), next(n) {} };
        Node* _head;
        Node* _tail;
        int   _size;
    public:
        Queue() : _head(nullptr), _tail(nullptr), _size(0) {}
        Queue(Queue const& o) : _head(nullptr), _tail(nullptr), _size(o._size) {
            for (Node* p = o._head; p != nullptr; p = p->next) {
                Node* n = new Node(p->data, nullptr);
                if (_tail == nullptr) { _head = _tail = n; } else { _tail->next = n; _tail = n; }
            }
        }
        ~Queue() { while (_head != nullptr) { Node* nx = _head->next; delete _head; _head = nx; } }
        void swap(Queue& o) noexcept { std::swap(_head, o._head); std::swap(_tail, o._tail); std::swap(_size, o._size); }
        Queue(Queue&& o) noexcept : Queue() { swap(o); }
        Queue& operator=(Queue o) { swap(o); return *this; }

        void     enqueue(T const& x) { Node* n = new Node(x, nullptr);
                                       if (_tail == nullptr) { _head = _tail = n; } else { _tail->next = n; _tail = n; }
                                       _size++; }
        T        dequeue()           { Node* n = _head; T x = n->data; _head = n->next;
                                       if (_head == nullptr) _tail = nullptr;
                                       delete n; _size--; return x; }
        T const& peek() const        { return _head->data; }
        bool     isEmpty() const     { return _size == 0; }
        int      size()    const     { return _size; }
};

#endif  // QUEUE_HPP
