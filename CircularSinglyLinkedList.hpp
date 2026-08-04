// CircularSinglyLinkedList.hpp -- YOU IMPLEMENT the methods marked TODO. This
// is a singly-linked list bent into a RING: `_rear->next == front`, with NO
// null terminator. Adapt the PROVIDED plain SinglyLinkedList
// (SinglyLinkedList.hpp) into this ring -- most methods are the same idea,
// changed to keep the ring closed.
//
// Two things are different from the plain list:
//   * There is no `nullptr` tail. The "front" is `_rear->next`; the "back" is
//   `_rear`. Insertions must keep
//     `_rear->next == front` true.
//   * A ring has no null to stop at, so the cursor is COUNT-bounded: atEnd()
//   compares its index to the
//     list's current length (which shrinks as you detach). rotate() shifts the
//     ring one hop in O(1).
//
// It uses the SAME SinglyLinkedNode<T> as the plain list, so a node can MOVE
// between the two by relinking. The scaffolding (rule of five, size/isEmpty,
// cursor plumbing, iterator, insertBefore/detachFront which delegate to your
// attach/detach) is given; you fill the ring mechanics. The starter compiles
// as-is (the
// TODO stubs are safe no-ops), but does nothing useful until you implement
// them.
#ifndef CIRCULAR_SINGLY_LINKED_LIST_HPP
#define CIRCULAR_SINGLY_LINKED_LIST_HPP

#include "SinglyLinkedNode.hpp"
#include <utility> // std::swap

template <typename T> class CircularSinglyLinkedList {
  private:
    SinglyLinkedNode<T> *_rear; // _rear->next == front; nullptr if empty
    int _length;

    SinglyLinkedNode<T> *_front() const {
        return _rear == nullptr ? nullptr : _rear->next;
    }

  public:
    // Count-bounded cursor: predecessor + current node + logical index. atEnd()
    // asks the owning list its current length (which shrinks as you detach), so
    // the walk stops after visiting every node once.
    class Cursor {
      private:
        SinglyLinkedNode<T> *_prev;
        SinglyLinkedNode<T> *_curr;
        int _idx;
        CircularSinglyLinkedList *_owner;
        friend class CircularSinglyLinkedList;
        Cursor(SinglyLinkedNode<T> *p, SinglyLinkedNode<T> *c, int i,
               CircularSinglyLinkedList *o)
            : _prev(p), _curr(c), _idx(i), _owner(o) {}

      public:
        T &operator*() const { return _curr->data; }
        Cursor &operator++() {
            _prev = _curr;
            _curr = _curr->next;
            ++_idx;
            return *this;
        }
        bool operator!=(Cursor const &o) const { return _idx != o._idx; }
        bool operator==(Cursor const &o) const { return _idx == o._idx; }
        // DONE: a ring has no null end. Return true once this cursor has passed
        // every node -- i.e. when its
        //       index has reached the list's current length (_owner->_length).
        //       (The plain list stops at a null node; you stop by COUNT.)
        bool atEnd() const {
            // Why would it be ever > _length?
            // Isn't == enough?
            return _idx >= _owner->_length;
        } // <-- replace: return _idx >= (the list's length)
    };

    // ---- rule of five ----
    CircularSinglyLinkedList() : _rear(nullptr), _length(0) {}
    CircularSinglyLinkedList(CircularSinglyLinkedList const &o)
        : _rear(nullptr), _length(0) {
        if (o._rear == nullptr)
            return;
        SinglyLinkedNode<T> *cur = o._rear->next;
        for (int i = 0; i < o._length; ++i, cur = cur->next)
            append(cur->data);
    }
    ~CircularSinglyLinkedList() {
        if (_rear == nullptr)
            return;
        SinglyLinkedNode<T> *front = _rear->next;
        _rear->next = nullptr; // break the ring, then delete linearly
        SinglyLinkedNode<T>::deleteChain(front);
    }
    void swap(CircularSinglyLinkedList &o) noexcept {
        std::swap(_rear, o._rear);
        std::swap(_length, o._length);
    }
    CircularSinglyLinkedList(CircularSinglyLinkedList &&o) noexcept
        : CircularSinglyLinkedList() {
        swap(o);
    }
    CircularSinglyLinkedList &operator=(CircularSinglyLinkedList o) {
        swap(o);
        return *this;
    }

    // ---- size + O(1) ends ----
    bool isEmpty() const { return _length == 0; }
    int size() const { return _length; }
    // DONE: the front is _rear->next; the back is _rear. (Both O(1). Assume the
    // list is non-empty.)
    T const &front() const {
        return _rear->next->data;
    } // <-- replace with the ring's front
    T const &back() const {
        return _rear->data;
    } // <-- replace with the ring's back

    // DONE rotate(): advance the ring one hop -- the old front becomes the new
    // rear -- in O(1), by moving
    //      _rear forward one node. (No node is allocated, moved, or copied.)
    void rotate() { _rear = _rear->next; }

    // ---- O(1) end insertion (COPIES x). Keep the ring closed: _rear->next
    // must stay == front. ----
    // DONE append(): add a new node holding x as the new REAR (so it becomes
    // the back, its next = front).
    //      Empty case (the FIRST append): the new node is the only node --
    //      n->next = n (both front and rear).
    void append(T const &x) {
        if (_length) {
            _rear = (_rear->next = new SinglyLinkedNode<T>{x, _rear->next});
        } else {
            _rear = new SinglyLinkedNode<T>{x, nullptr};
            _rear->next = _rear;
        }
        ++_length;
    }
    // DONE prepend(): add a new node holding x as the new FRONT (rear stays the
    // same; its next = new node).
    //      Empty case: n is the only node -- n->next = n.
    void prepend(T const &x) {
        if (_length == 0) {
            _rear = new SinglyLinkedNode<T>{x, nullptr};
            _rear->next = _rear;
        } else {
            _rear->next = new SinglyLinkedNode<T>{x, _rear->next};
        }
        ++_length;
    }

    // ---- cursor factories ----
    Cursor begin_cursor() { return Cursor(_rear, _front(), 0, this); }
    Cursor end_cursor() { return Cursor(nullptr, nullptr, _length, this); }

    // ---- cursor-based COPY insert (used to build the list) ----
    void insertBefore(T const &x, Cursor c) {
        attachBefore(c, new SinglyLinkedNode<T>(x, nullptr));
    }

    // ---- NODE-LEVEL transfer (no copy) -- the SAME currency as the plain list
    // (see its detachAt/attach). ----
    // DONE detachAt(): unlink the node at `c` from the ring (splice `c._prev`
    // around it; if it was `_rear`,
    //      the new rear is `c._prev`; the single-node case empties the list).
    //      DON'T delete it. Decrement _length, advance `c` to the next node
    //      (set c._curr; leave c._prev and c._idx), set the node's next to
    //      nullptr, and RETURN it. (Mirror the plain list's detachAt, kept
    //      ring-correct.)
    SinglyLinkedNode<T> *detachAt(Cursor &c) {
        if (_length == 0 || c._curr == nullptr) {
            return nullptr;
        }

        SinglyLinkedNode<T> *detached = c._curr;

        if (_length == 1) {
            // The only node points to itself.
            _rear = nullptr;
            _length = 0;

            c._curr = nullptr;
            c._prev = nullptr;

            detached->next = nullptr;
            return detached;
        }

        SinglyLinkedNode<T> *next = detached->next;

        c._prev->next = next;

        if (detached == _rear) {
            _rear = c._prev;
        }

        --_length;
        c._curr = next;
        detached->next = nullptr;
        return detached;
    }
    SinglyLinkedNode<T> *detachFront() {
        if (_length == 0) {
            return nullptr;
        }
        Cursor c = begin_cursor();
        return detachAt(c);
    }

    // DONE attachBack(): splice the EXISTING node `n` in as the new rear (its
    // next = front). Empty case:
    //      it becomes the only node (n->next = n). Increment _length.
    void attachBack(SinglyLinkedNode<T> *n) {
        if (n == nullptr) {
            return;
        }
        if (_length) {
            n->next = _rear->next;
            _rear->next = n;
            _rear = n;
        } else {
            n->next = n;
            _rear = n;
        }
        ++_length;
    }
    // DONE attachBefore(): splice the EXISTING node `n` in just before position
    // `c`. Cases: empty list
    //      (n is the only node); c at the end (== attach as the new rear);
    //      otherwise link it between c._prev and c._curr. Increment _length.
    void attachBefore(Cursor c, SinglyLinkedNode<T> *n) {
        if (n == nullptr) {
            return;
        }
        if (_length && !c.atEnd()) {
            c._prev->next = n;
            n->next = c._curr;
            ++_length;
        } else {
            attachBack(n);
        }
    }

    // ---- read-only forward iterator (count-bounded) ----
    class Iterator {
      private:
        SinglyLinkedNode<T> *_p;
        int _idx;

      public:
        Iterator(SinglyLinkedNode<T> *p, int i) : _p(p), _idx(i) {}
        T const &operator*() const { return _p->data; }
        Iterator &operator++() {
            _p = _p->next;
            ++_idx;
            return *this;
        }
        bool operator!=(Iterator const &o) const { return _idx != o._idx; }
    };
    Iterator begin() const { return Iterator(_front(), 0); }
    Iterator end() const { return Iterator(nullptr, _length); }
};

#endif // CIRCULAR_SINGLY_LINKED_LIST_HPP
