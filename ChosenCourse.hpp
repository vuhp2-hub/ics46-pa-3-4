// ChosenCourse.hpp -- PROVIDED. One course fixed to a specific dish -- read from a .chosen file, and
// also the way a Meal records the dish it picks for each course. A plain data struct.
#ifndef CHOSEN_COURSE_HPP
#define CHOSEN_COURSE_HPP

struct ChosenCourse {
    int courseIdx;
    int dish;
};

#endif  // CHOSEN_COURSE_HPP
