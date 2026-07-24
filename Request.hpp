// Request.hpp -- PROVIDED. One request the queue carries. Three kinds:
//   AddMeal      -- add `meal` to the active shortlist (in score order), UNLESS it disagrees with a fixed course.
//   SetCourse    -- fix `course` to `dish`: drop the meals already there that disagree, and reject later add-meals
//                   that disagree. Setting a course that is ALREADY set does nothing.
//   RemoveLowest -- drop the `k` lowest-scoring meals (if `k` exceeds the current size, ALL are dropped).
//
// A request is aggregate-initialized. A non-add request has no meal, so pass a default one -- spell it out
// (rather than leaving the field off) and the line stays warning-free under -Wall -Wextra:
//   {Request::AddMeal,      0, 0, 0, someMeal}   // add someMeal
//   {Request::SetCourse,    2, 1, 0, Meal{}}     // fix course 2 to dish 1
//   {Request::RemoveLowest, 0, 0, 3, Meal{}}     // drop the 3 lowest
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Meal.hpp"

struct Request {
    enum Kind { AddMeal, SetCourse, RemoveLowest } kind;
    int  course;   // SetCourse: the course being fixed
    int  dish;     // SetCourse: the dish it is fixed to
    int  k;        // RemoveLowest: how many lowest-scoring meals to drop
    Meal meal;     // AddMeal: the meal to add (default/unused for the other kinds)
};

#endif  // REQUEST_HPP
