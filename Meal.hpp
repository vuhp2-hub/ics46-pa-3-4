// Meal.hpp -- PROVIDED. A complete MEAL: one chosen dish for every course (a full assignment). It owns a
// raw array of ChosenCourse (one per course), so it follows the rule of five. This file also defines the
// out-of-line MenuModel::MealIterator::operator* (the meal-decode), which needs both MenuModel and Meal.
#ifndef MEAL_HPP
#define MEAL_HPP

#include <utility>   // std::swap
#include "Course.hpp"
#include "ChosenCourse.hpp"
#include "MenuModel.hpp"

// A counter bumped on every DEEP COPY of a Meal. Because Meal uses copy-and-swap, EVERY deep copy funnels
// through the copy constructor below, while a MOVE just swaps pointers and is NOT counted -- so this one
// counter distinguishes "moved the meal's node" from "copied the meal". It lives in this PROVIDED header
// (which a submission cannot override or edit) so it is tamper-proof.
namespace ics46 { inline long long mealCopyCount = 0; }

class Meal {
    private:
        int _numCourses;
        ChosenCourse* _choices;   // _choices[c] = { courseIdx = c, dish = the dish picked for course c }

    public:
        Meal() : _numCourses(0), _choices(nullptr) {}

        explicit Meal(int numCourses)
            : _numCourses(numCourses),
              _choices(numCourses > 0 ? new ChosenCourse[numCourses] : nullptr) {
            for (int c = 0; c < _numCourses; ++c) { _choices[c].courseIdx = c; _choices[c].dish = -1; }
        }

        ~Meal() { delete[] _choices; }

        Meal(Meal const& other)
            : _numCourses(other._numCourses),
              _choices(other._numCourses > 0 ? new ChosenCourse[other._numCourses] : nullptr) {
            ics46::mealCopyCount++;                     // deep copy (a move swaps instead -- see below)
            for (int c = 0; c < _numCourses; ++c) _choices[c] = other._choices[c];
        }

        friend void swap(Meal& a, Meal& b) noexcept {
            std::swap(a._numCourses, b._numCourses);
            std::swap(a._choices, b._choices);
        }
        Meal(Meal&& other) noexcept : Meal() { swap(*this, other); }
        Meal& operator=(Meal other) { swap(*this, other); return *this; }

        void setDish(int courseIdx, int dish) { _choices[courseIdx].dish = dish; }
        int dishFor(int courseIdx) const { return _choices[courseIdx].dish; }
        int size() const { return _numCourses; }
        ChosenCourse choiceAt(int i) const { return _choices[i]; }

        // Do two meals pick the same dish on EVERY course? (order-independent). In C++20 this also gives !=.
        bool operator==(Meal const& other) const {
            if (size() != other.size()) return false;
            for (int c = 0; c < size(); ++c)
                if (dishFor(c) != other.dishFor(c)) return false;
            return true;
        }
};

// The meal at position `pos`: the chosen courses are fixed, and the FREE courses are a mixed-radix decode
// of `pos` (first free course slowest -- peel the last course's digit first).
inline Meal MenuModel::MealIterator::operator*() const {
    Meal meal(_model->numCourses());
    long long remaining = _pos;
    for (int c = _model->numCourses() - 1; c >= 0; --c) {
        if (_model->isChosen(c)) {
            meal.setDish(c, _model->chosenDish(c));
        } else {
            int d = _model->dishes(c);
            meal.setDish(c, static_cast<int>(remaining % d));
            remaining /= d;
        }
    }
    return meal;
}

#endif  // MEAL_HPP
