// MenuModel.hpp -- PROVIDED. The whole menu-design model: courses (with dish counts), the pairing
// tables, and any already-chosen courses. Owns several dynamically-allocated arrays (rule of five).
// It also provides a LAZY iterator over the complete meals consistent with the chosen courses --
// iterate model.completeMeals() to visit them one at a time (no giant array).
//
// The meal-decode (MealIterator::operator*) needs the Meal type, so it is defined in Meal.hpp, which
// includes this file. Standard library is used ONLY for input.
#ifndef MENU_MODEL_HPP
#define MENU_MODEL_HPP

#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>   // std::swap

#include "Course.hpp"
#include "ChosenCourse.hpp"
#include "PairingFactor.hpp"

class Meal;   // forward declaration (defined in Meal.hpp) so MealIterator can name it

class MenuModel {
    private:
        int _numCourses;
        Course* _courses;
        int _numFactors;
        PairingFactor* _factors;
        int _numChosen;
        ChosenCourse* _chosen;

    public:
        MenuModel()
            : _numCourses(0), _courses(nullptr),
              _numFactors(0), _factors(nullptr),
              _numChosen(0), _chosen(nullptr) {}

        explicit MenuModel(std::string const& menuPath) : MenuModel() { _loadMenu(menuPath); }

        ~MenuModel() {
            delete[] _courses;
            delete[] _factors;
            delete[] _chosen;
        }

        MenuModel(MenuModel const& other)
            : _numCourses(other._numCourses), _courses(nullptr),
              _numFactors(other._numFactors), _factors(nullptr),
              _numChosen(other._numChosen), _chosen(nullptr) {
            if (_numCourses > 0) {
                _courses = new Course[_numCourses];
                for (int i = 0; i < _numCourses; ++i) _courses[i] = other._courses[i];
            }
            if (_numFactors > 0) {
                _factors = new PairingFactor[_numFactors];
                for (int i = 0; i < _numFactors; ++i) {
                    _factors[i] = other._factors[i];
                    for (int j = 0; j < _factors[i].scopeSize(); ++j)
                        _factors[i].setCourse(j, &_courses[_factors[i].courseAt(j)]);
                }
            }
            if (_numChosen > 0) {
                _chosen = new ChosenCourse[_numChosen];
                for (int i = 0; i < _numChosen; ++i) _chosen[i] = other._chosen[i];
            }
        }

        friend void swap(MenuModel& a, MenuModel& b) noexcept {
            std::swap(a._numCourses, b._numCourses);
            std::swap(a._courses, b._courses);
            std::swap(a._numFactors, b._numFactors);
            std::swap(a._factors, b._factors);
            std::swap(a._numChosen, b._numChosen);
            std::swap(a._chosen, b._chosen);
        }

        MenuModel(MenuModel&& other) noexcept : MenuModel() { swap(*this, other); }
        MenuModel& operator=(MenuModel other) { swap(*this, other); return *this; }

        void readChosen(std::string const& chosenPath) {
            std::ifstream in(chosenPath);
            if (!in) throw std::runtime_error("cannot open " + chosenPath);
            int k;
            if (!(in >> k)) throw std::runtime_error("bad .chosen header");
            delete[] _chosen;
            _numChosen = k;
            _chosen = new ChosenCourse[k];
            for (int i = 0; i < k; ++i) in >> _chosen[i].courseIdx >> _chosen[i].dish;
        }

        // ---- structure accessors ----
        int numCourses() const { return _numCourses; }
        int dishes(int courseIdx) const { return _courses[courseIdx].numDishes; }
        int numTables() const { return _numFactors; }
        PairingFactor const& factorAt(int i) const { return _factors[i]; }

        long long totalMealCount() const {
            long long product = 1;
            for (int i = 0; i < _numCourses; ++i) product *= _courses[i].numDishes;
            return product;
        }
        int chosenCount() const { return _numChosen; }
        int freeCourses() const { return _numCourses - _numChosen; }
        int chosenCourseAt(int i) const { return _chosen[i].courseIdx; }
        int chosenDishAt(int i) const { return _chosen[i].dish; }

        bool isChosen(int courseIdx) const {
            for (int i = 0; i < _numChosen; ++i) if (_chosen[i].courseIdx == courseIdx) return true;
            return false;
        }
        int chosenDish(int courseIdx) const {
            for (int i = 0; i < _numChosen; ++i) if (_chosen[i].courseIdx == courseIdx) return _chosen[i].dish;
            return -1;
        }
        long long consistentMealCount() const {
            long long product = 1;
            for (int c = 0; c < _numCourses; ++c) if (!isChosen(c)) product *= dishes(c);
            return product;
        }

        // ---- a LAZY iterator over the complete meals consistent with the chosen courses ----
        class MealIterator {
            public:
                MealIterator(MenuModel const* model, long long pos) : _model(model), _pos(pos) {}
                Meal operator*() const;                          // defined in Meal.hpp (needs the Meal type)
                MealIterator& operator++() { ++_pos; return *this; }
                bool operator!=(MealIterator const& other) const { return _pos != other._pos; }
            private:
                MenuModel const* _model;
                long long _pos;
        };
        class CompleteMeals {
            public:
                explicit CompleteMeals(MenuModel const* model) : _model(model) {}
                MealIterator begin() const { return MealIterator(_model, 0); }
                MealIterator end() const { return MealIterator(_model, _model->consistentMealCount()); }
            private:
                MenuModel const* _model;
        };
        CompleteMeals completeMeals() const { return CompleteMeals(this); }

    private:
        void _loadMenu(std::string const& menuPath) {
            std::ifstream in(menuPath);
            if (!in) throw std::runtime_error("cannot open " + menuPath);

            std::string tag;
            in >> tag;                       // "MENU" -- a format marker; skip it

            in >> _numCourses;
            _courses = new Course[_numCourses];
            for (int i = 0; i < _numCourses; ++i) {
                _courses[i].courseIdx = i;
                in >> _courses[i].numDishes;
            }

            in >> _numFactors;
            _factors = new PairingFactor[_numFactors];
            for (int i = 0; i < _numFactors; ++i) {
                int scopeSize;
                in >> scopeSize;
                _factors[i].allocateScope(scopeSize);
                for (int j = 0; j < scopeSize; ++j) {
                    int courseIdx;
                    in >> courseIdx;
                    _factors[i].setCourse(j, &_courses[courseIdx]);
                }
            }
            for (int i = 0; i < _numFactors; ++i) {
                int numEntries;
                in >> numEntries;
                _factors[i].allocateScores(numEntries);
                for (int j = 0; j < numEntries; ++j) {
                    double score;
                    in >> score;
                    _factors[i].setScore(j, score);
                }
            }
        }
};

#endif  // MENU_MODEL_HPP
