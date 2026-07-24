// mealScore.hpp -- PROVIDED. A meal's TOTAL SCORE: multiply, over every pairing table, the one score for
// the dishes this meal picks on that table's courses (row-major index over the table's ascending scope).
// Higher is better. This is how the shortlist ranks candidate meals.
#ifndef MEAL_SCORE_HPP
#define MEAL_SCORE_HPP

#include "MenuModel.hpp"
#include "Meal.hpp"

// A global counter bumped on every mealScore() call. The performance check reads it to measure how much
// SCORING WORK a solution does over a workload: an efficient organization scores each meal only a handful
// of times, while a scan-based one scores O(N) meals per removal. It lives in this PROVIDED header (which a
// submission cannot override) so it is tamper-proof. It never changes any answer.
namespace ics46 { inline long long scoreCallCount = 0; }

inline double mealScore(MenuModel const& model, Meal const& meal) {
    ics46::scoreCallCount++;
    double score = 1.0;
    for (int t = 0; t < model.numTables(); ++t) {
        PairingFactor const& f = model.factorAt(t);
        int idx = 0;
        for (int j = 0; j < f.scopeSize(); ++j) {
            int c = f.courseAt(j);
            idx = idx * model.dishes(c) + meal.dishFor(c);
        }
        score *= f.scoreAt(idx);
    }
    return score;
}

#endif  // MEAL_SCORE_HPP
