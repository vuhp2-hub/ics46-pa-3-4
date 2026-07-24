// SinglyLinkedList.hpp -- PROVIDED, COMPLETE. A plain singly-linked list (head + tail + length) with a
// ghost-pointer cursor. STUDY THIS FILE: it is the template you adapt into CircularSinglyLinkedList.hpp.
//
// It also exposes NODE-LEVEL transfer -- detachAt/detachFront (unlink a node and hand it back, WITHOUT
// deleting it) and attachBack/attachBefore (take ownership of an existing node). Because both lists use
// the same SinglyLinkedNode<T>, these let you MOVE a meal between the (circular) active list and a (plain)
// set-aside list by relinking one node -- no copy of the Meal.
//
// In PA 3,4 this plain list is used for the SET-ASIDE / undo lists (the ones that do not need rotate()).
#ifndef SINGLY_LINKED_LIST_HPP
#define SINGLY_LINKED_LIST_HPP

#include <utility>   // std::swap
#include "SinglyLinkedNode.hpp"

template<typename T>
class SinglyLinkedList {
    private:
        SinglyLinkedNode<T>* _head;
        SinglyLinkedNode<T>* _tail;
        int                  _length;

    public:
        // A position: the current node AND its predecessor (the "ghost pointer"), so a splice/remove can
        // relink through _prev in O(1). Clients use *c, ++c, c.atEnd(), c != d.
        class Cursor {
            private:
                SinglyLinkedNode<T>* _prev;
                SinglyLinkedNode<T>* _curr;
                friend class SinglyLinkedList;
                Cursor(SinglyLinkedNode<T>* p, SinglyLinkedNode<T>* c) : _prev(p), _curr(c) {}
            public:
                T&      operator*() const { return _curr->data; }
                Cursor& operator++()      { _prev = _curr; _curr = _curr->next; return *this; }
                bool    operator!=(Cursor const& o) const { return _curr != o._curr; }
                bool    operator==(Cursor const& o) const { return _curr == o._curr; }
                bool    atEnd() const { return _curr == nullptr; }
        };

        // ---- rule of five ----
        SinglyLinkedList() : _head(nullptr), _tail(nullptr), _length(0) {}
        SinglyLinkedList(SinglyLinkedList const& o) : _head(nullptr), _tail(nullptr), _length(0) {
            for (SinglyLinkedNode<T>* p = o._head; p != nullptr; p = p->next) append(p->data);
        }
        ~SinglyLinkedList() { SinglyLinkedNode<T>::deleteChain(_head); }
        void swap(SinglyLinkedList& o) noexcept {
            std::swap(_head, o._head); std::swap(_tail, o._tail); std::swap(_length, o._length);
        }
        SinglyLinkedList(SinglyLinkedList&& o) noexcept : SinglyLinkedList() { swap(o); }
        SinglyLinkedList& operator=(SinglyLinkedList o) { swap(o); return *this; }

        // ---- size + O(1) ends ----
        bool     isEmpty() const { return _length == 0; }
        int      size()    const { return _length; }
        T const& front()   const { return _head->data; }
        T const& back()    const { return _tail->data; }

        // ---- O(1) end insertion (COPIES x into a new node) ----
        void append(T const& x)  { SinglyLinkedNode<T>* n = new SinglyLinkedNode<T>(x, nullptr);
                                   if (_tail == nullptr) { _head = _tail = n; } else { _tail->next = n; _tail = n; }
                                   _length++; }
        void prepend(T const& x) { SinglyLinkedNode<T>* n = new SinglyLinkedNode<T>(x, _head);
                                   _head = n; if (_tail == nullptr) _tail = n; _length++; }

        // ---- cursor factories ----
        Cursor begin_cursor() { return Cursor(nullptr, _head); }
        Cursor end_cursor()   { return Cursor(_tail, nullptr); }

        // ---- cursor-based COPY insert / delete ----
        void insertBefore(T const& x, Cursor c) {            // copies x into a new node before c
            SinglyLinkedNode<T>* n = new SinglyLinkedNode<T>(x, c._curr);
            if (c._prev == nullptr) _head = n; else c._prev->next = n;
            if (n->next == nullptr) _tail = n;
            _length++;
        }
        bool removeAt(Cursor c) {                             // deletes the node at c
            if (c._curr == nullptr) return false;
            SinglyLinkedNode<T>* gone = c._curr, * after = gone->next;
            if (c._prev == nullptr) _head = after; else c._prev->next = after;
            if (after == nullptr) _tail = c._prev;
            delete gone; _length--; return true;
        }

        // ---- NODE-LEVEL transfer (no copy): unlink / take ownership of an existing node ----
        // detachAt: unlink the node at `c`, DON'T delete it, advance `c` to the following node, and RETURN
        // the node so the caller can attach it into another list.
        SinglyLinkedNode<T>* detachAt(Cursor& c) {
            SinglyLinkedNode<T>* n = c._curr;
            if (n == nullptr) return nullptr;
            SinglyLinkedNode<T>* after = n->next;
            if (c._prev == nullptr) _head = after; else c._prev->next = after;
            if (after == nullptr) _tail = c._prev;
            _length--;
            c._curr = after;                                 // advance; c._prev unchanged
            n->next = nullptr;
            return n;
        }
        SinglyLinkedNode<T>* detachFront() { Cursor c = begin_cursor(); return detachAt(c); }

        void attachBack(SinglyLinkedNode<T>* n) {            // append an EXISTING node (no copy)
            n->next = nullptr;
            if (_tail == nullptr) { _head = _tail = n; } else { _tail->next = n; _tail = n; }
            _length++;
        }
        void attachBefore(Cursor c, SinglyLinkedNode<T>* n) {// splice an EXISTING node in before c (no copy)
            n->next = c._curr;
            if (c._prev == nullptr) _head = n; else c._prev->next = n;
            if (n->next == nullptr) _tail = n;
            _length++;
        }

        // ---- read-only forward iterator (range-for / inspection) ----
        class Iterator {
            private:
                SinglyLinkedNode<T>* _p;
            public:
                explicit Iterator(SinglyLinkedNode<T>* p) : _p(p) {}
                T const&  operator*() const { return _p->data; }
                Iterator& operator++()      { _p = _p->next; return *this; }
                bool      operator!=(Iterator const& o) const { return _p != o._p; }
        };
        Iterator begin() const { return Iterator(_head); }
        Iterator end()   const { return Iterator(nullptr); }
};

#endif  // SINGLY_LINKED_LIST_HPP
