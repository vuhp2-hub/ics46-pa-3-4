// Shortlist.hpp -- YOU DESIGN this class (one of the two files you submit; the
// other is CircularSinglyLinkedList.hpp).
//
// A Shortlist starts EMPTY and grows as requests arrive. It OWNS its active
// list, its undo history, and any bookkeeping it allocates. The ACTIVE
// shortlist is a CircularSinglyLinkedList<Meal> (given). Requests come in three
// kinds (see Request.hpp): add a meal, set a course, or remove the k lowest --
// and each SUCCESSFUL one must be undoable. undoLast reverses the MOST RECENT
// successful request, so your undo history is HETEROGENEOUS: what one record
// must remember depends on which kind it reverses. THAT DESIGN IS YOURS --- so
// is how you track which courses are set.
//
// MOVE meal nodes between the active list and your set-aside storage (detach a
// node here, attach it there) --- never COPY a Meal. Undoing an ADD-MEAL is the
// exception: it DESTROYS that meal (no redo). You CHOOSE how to order the
// active list; correctness does not depend on it, and the graded measured
// checks reward a wise choice. Because the Shortlist OWNS everything it sets
// aside, free it (rule of five) --- any leak fails the whole assignment.
//
// The method signatures and the read-only view (size / isEmpty / undoDepth /
// begin / end) are the fixed contract the autograder calls -- keep them.
// Everything private is your design.
#ifndef SHORTLIST_HPP
#define SHORTLIST_HPP

#include "CircularSinglyLinkedList.hpp"
#include "Meal.hpp"
#include "MenuModel.hpp"
#include "Queue.hpp"
#include "Request.hpp"
#include "SinglyLinkedList.hpp"
#include "SinglyLinkedNode.hpp"
#include "Stack.hpp"
#include "mealScore.hpp"

class Shortlist {
  private:
    CircularSinglyLinkedList<Meal>
        _active; // the active shortlist -- YOU order it

    // TODO --- DESIGN YOUR UNDO HISTORY
    // -------------------------------------------------------------------
    // undoLast must reverse whichever kind of request is on top, so one record
    // must remember enough to undo ONE request of its kind: an add-meal (find &
    // destroy the meal you added), a set-course (unset it and put its removed
    // meals back), or a remove-lowest (put its removed meals back). Design a
    // record that covers all three, then make `_undo` a Stack of that.

    class Record {
      private:
        Request::Kind reqKind;
        SinglyLinkedList<Meal> set_aside;
        void unadd() {}
        void unset() {}
        void putBackKLowest() {}

      public:
        Record(const Request::Kind reqKind, Meal meal) {
            set_aside.append(meal);
        }
        Record(const Request::Kind reqKind, SinglyLinkedList<Meal> &set_aside)
            : reqKind{reqKind}, set_aside{set_aside} {}
        void reverse() {
            if (reqKind == Request::Kind::AddMeal) {
                unadd();
            } else if (reqKind == Request::Kind::SetCourse) {
                unset();
            } else {
                putBackKLowest();
            }
        }
    };
    Stack<Record> _undo; // <-- replace `int` with your record type

    // DONE --- TRACK WHICH COURSES ARE SET
    // ---------------------------------------------------------------- You need
    // to know, on each add, whether a course is fixed and to which dish. Add
    // whatever member(s) you want here (an array, a list, ...).

    struct SettedCourse {
        int idx;
        int dish;
    };
    SinglyLinkedList<SettedCourse> coursesSet;

  public:
    Shortlist() = default;

    // TODO: free everything you still own here -- pending set-aside storage,
    // plus any bookkeeping you
    //       allocated (only needed if it lives on the heap).
    ~Shortlist() {
        // coursesSet should be freed with its own destructor
    }

    // ---- read-only view of the active shortlist (used by the driver +
    // autograder) -- PROVIDED, keep as-is ----
    int size() const { return _active.size(); }
    bool isEmpty() const { return _active.isEmpty(); }
    int undoDepth() const { return _undo.size(); }
    CircularSinglyLinkedList<Meal>::Iterator begin() const {
        return _active.begin();
    }
    CircularSinglyLinkedList<Meal>::Iterator end() const {
        return _active.end();
    }

    // 1. Add `meal` in score order, UNLESS it disagrees with a course already
    // fixed; return whether it was added.
    //    Walk a cursor to the spot, then attach the meal's node. Use
    //    mealScore(model, m) for a meal's score.
    bool addMeal(Meal const &meal, MenuModel const &model) {
        // First see if a fixed course disagrees
        for (const SettedCourse &courseSet : coursesSet) {
            if (meal.dishFor(courseSet.idx) != courseSet.dish) {
                return false;
            }
        }

        // Insertion into correct place
        double score = mealScore(model, meal);
        auto cursor = _active.begin_cursor();
        if (_active.isEmpty()) {
            _active.insertBefore(meal, cursor);
        } else {
            bool added = false;
            for (; !cursor.atEnd(); ++cursor) {
                if (score < mealScore(model, *cursor)) {
                    _active.insertBefore(meal, cursor);
                    added = true;
                    break;
                }
            }
            if (!added) {
                _active.insertBefore(meal, cursor);
            }
            _undo.push(Record{Request::Kind::AddMeal, meal});
        }
        return true;
    }

    // 2. Fix `course` to `dish`: if it is ALREADY set, do nothing and return
    // false. Otherwise record it,
    //    MOVE out every meal that picks a different dish for `course` (keep
    //    them for undo), and return true.
    bool setCourse(int course, int dish, MenuModel const &model) {
        // TODO
        (void)course;
        (void)dish;
        (void)model;
        return false;
    }

    // 3. MOVE the `k` lowest-scoring meals out (keep them for undo). If `k` is
    // more than the current size,
    //    remove ALL of them. A call that changes nothing (`k <= 0`, or an empty
    //    shortlist) records NO undo entry.
    void removeLowest(int k, MenuModel const &model) {
        // TODO
        (void)k;
        (void)model;
    }

    // 4. Process a queue of requests IN ARRIVAL ORDER, dispatching each by its
    // kind to the methods above.
    void applyRequests(Queue<Request> &requests, MenuModel const &model) {
        // TODO
        (void)requests;
        (void)model;
    }

    // 5. Undo the MOST RECENT successful request. Pop the top record and
    // reverse whatever kind it is: an
    //    add-meal (find that meal, remove it from the active list, and delete
    //    it); a set-course (unset the course and move its removed meals back
    //    into the active list in score order); a remove-lowest (move its
    //    removed meals back). Free anything that record owned. With an EMPTY
    //    undo history, do nothing. How much work your undo does is graded (see
    //    the instructions) -- what a record remembers is your design.
    void undoLast(MenuModel const &model) {
        // TODO
        (void)model;
    }

    // 6. THE FINALE (once the shortlist is set in stone): the caller features
    // one meal at a time --
    //    featuredMeal() returns the meal currently in the spotlight (whichever
    //    end of the ring your rotation exposes) and rotate() advances one hop,
    //    so a lap of size() turns visits every meal exactly once. Return a
    //    REFERENCE -- featuredMeal() must not copy the meal. Like the ring's
    //    front()/back(), it may assume the shortlist is not empty.
    Meal const &featuredMeal() const {
        static Meal _stub{};
        return _stub;
    } // <-- TODO: the featured meal
    void rotate() { /* TODO: advance the ring one hop */ }
};

#endif // SHORTLIST_HPP
