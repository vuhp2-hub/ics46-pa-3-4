// driver.cpp -- PROVIDED for local testing. DO NOT MODIFY (and you do not
// submit it).
//
// Builds a MenuModel from a .menu file (and optional .chosen), makes a small
// subset of candidate meals, and exercises your Shortlist: it starts EMPTY,
// grows as ADD requests come, has a course SET (dropping the meals that
// disagree), the k lowest removed, those two changes undone one at a time,
// every meal featured in turn, and finally one ADD undone (which destroys that
// meal). It then replays the same kinds of change through applyRequests off a
// QUEUE.
//
// Each stage prints the list's CONTENTS in a canonical (dish-order) sort -- so
// the report is the SAME whatever internal order you keep and however you store
// your undo history, exactly how the autograder compares. That makes it
// diff-able against the samples/expected_*.txt goldens:
//
//     g++ -std=c++20 -Wall -Wextra driver.cpp -o refine
//     ./refine samples/menu3.menu samples/menu3.chosen
//     diff <(./refine samples/menu3.menu samples/menu3.chosen)
//     samples/expected_menu3.txt   # prints nothing if correct
#include "Shortlist.hpp"
#include <iostream>

static void printMeal(Meal const &m, int n) {
    for (int c = 0; c < n; ++c)
        std::cout << (c ? " " : "") << m.dishFor(c);
}
static int canon(Meal const &a, Meal const &b, int n) {
    for (int c = 0; c < n; ++c) {
        int d = a.dishFor(c) - b.dishFor(c);
        if (d)
            return d;
    }
    return 0;
}
template <class L>
static void printSorted(char const *label, L &list, MenuModel const &model) {
    int n = model.numCourses(), m = list.size();
    Meal *arr = new Meal[m > 0 ? m : 1];
    int i = 0;
    for (Meal const &meal : list)
        arr[i++] = meal;
    for (int a = 0; a < m - 1; ++a)
        for (int b = 0; b < m - 1 - a; ++b)
            if (canon(arr[b], arr[b + 1], n) > 0) {
                Meal t = arr[b];
                arr[b] = arr[b + 1];
                arr[b + 1] = t;
            }
    std::cout << label << " (size " << m << "):\n";
    for (int a = 0; a < m; ++a) {
        std::cout << "    ";
        printMeal(arr[a], n);
        std::cout << "   score=" << mealScore(model, arr[a]) << "\n";
    }
    delete[] arr;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <file.menu> [file.chosen]\n";
        return 1;
    }
    MenuModel model(argv[1]);
    if (argc >= 3)
        model.readChosen(argv[2]);

    const int CAP = 8;
    Meal subset[CAP];
    double seen[CAP];
    int S = 0;
    for (Meal const &meal : model.completeMeals()) {
        if (S >= CAP)
            break;
        double sc = mealScore(model, meal);
        bool dup = false;
        for (int i = 0; i < S; ++i)
            if (seen[i] == sc) {
                dup = true;
                break;
            }
        if (dup)
            continue;
        subset[S] = meal;
        seen[S] = sc;
        S++;
    }
    std::cout << "courses: " << model.numCourses()
              << "   candidate meals: " << S << "\n\n";
    int d0 = (S > 0) ? subset[0].dishFor(0) : 0;

    long long copies =
        0; // Meals copied by the transfer ops (setCourse / removeLowest / undo)
    Shortlist sl;

    // The shortlist starts EMPTY and grows as add requests arrive.
    int added = 0;
    for (int i = 0; i < S; ++i)
        added += sl.addMeal(subset[i], model) ? 1 : 0;
    std::cout << "Added " << added << " of " << S << " meals.\n";
    printSorted("After adds", sl, model);

    std::cout << "\nsetCourse(0, " << d0 << ") returned "
              << ([&] {
                     ics46::mealCopyCount = 0;
                     bool r = sl.setCourse(0, d0, model);
                     copies += ics46::mealCopyCount;
                     return r;
                 }()
                      ? "true"
                      : "false")
              << "  (drops meals that pick a different dish for course 0)\n";
    printSorted("After setCourse", sl, model);

    std::cout << "\nremoveLowest(" << (S / 3 > 0 ? S / 3 : 1) << "):\n";
    ics46::mealCopyCount = 0;
    sl.removeLowest((S / 3 > 0 ? S / 3 : 1), model);
    copies += ics46::mealCopyCount;
    printSorted("After removeLowest", sl, model);

    // Undo one request at a time -- LIFO, and each kind reverses differently.
    ics46::mealCopyCount = 0;
    sl.undoLast(model);
    copies += ics46::mealCopyCount; // reverses the remove-lowest
    printSorted("After undo #1 (reverses remove-lowest)", sl, model);
    ics46::mealCopyCount = 0;
    sl.undoLast(model);
    copies += ics46::mealCopyCount; // reverses the set-course
    printSorted("After undo #2 (reverses set-course)", sl, model);

    return 0;
}
