// Stack.hpp -- PROVIDED, COMPLETE. A LIFO stack (own node chain). push (void) / pop (returns top) /
// peek / isEmpty / size. In PA 3,4 the undo history is a Stack of undo records -- the record type is YOUR
// design (it must hold enough to reverse one request of any kind: an add-meal, a set-course, or a remove-lowest).
#ifndef STACK_HPP
#define STACK_HPP

#include <utility>   // std::swap

template<typename T>
class Stack {
    private:
        struct Node { T data; Node* next; Node(T const& d, Node* n) : data(d), next(n) {} };
        Node* _top;
        int   _size;
    public:
        Stack() : _top(nullptr), _size(0) {}
        Stack(Stack const& o) : _top(nullptr), _size(o._size) {
            if (o._top == nullptr) return;
            _top = new Node(o._top->data, nullptr);          // copy top-to-bottom, preserving pop order
            Node* cur = _top;
            for (Node* p = o._top->next; p != nullptr; p = p->next) { cur->next = new Node(p->data, nullptr); cur = cur->next; }
        }
        ~Stack() { while (_top != nullptr) { Node* nx = _top->next; delete _top; _top = nx; } }
        void swap(Stack& o) noexcept { std::swap(_top, o._top); std::swap(_size, o._size); }
        Stack(Stack&& o) noexcept : Stack() { swap(o); }
        Stack& operator=(Stack o) { swap(o); return *this; }

        void     push(T const& x) { _top = new Node(x, _top); _size++; }
        T        pop()            { Node* n = _top; T x = n->data; _top = n->next; delete n; _size--; return x; }
        T const& peek() const     { return _top->data; }
        bool     isEmpty() const  { return _size == 0; }
        int      size()    const  { return _size; }
};

#endif  // STACK_HPP
