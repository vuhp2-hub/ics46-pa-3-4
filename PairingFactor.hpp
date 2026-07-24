// PairingFactor.hpp -- PROVIDED. A pairing table: the courses it covers (its "scope") and its table of
// scores. Both live in dynamically-allocated arrays that this object owns, so it follows the rule of five.
#ifndef PAIRING_FACTOR_HPP
#define PAIRING_FACTOR_HPP

#include <utility>   // std::swap
#include "Course.hpp"

class PairingFactor {
    private:
        int _scopeSize;
        Course const** _scope;   // const pointers to the courses this table covers (owned by MenuModel)
        int _numEntries;
        double* _scores;         // the score for every combination (length _numEntries)

    public:
        PairingFactor()
            : _scopeSize(0), _scope(nullptr), _numEntries(0), _scores(nullptr) {}

        // ---- two-step construction (matches the file: scopes first, then the score tables) ----
        void allocateScope(int scopeSize) {
            _scopeSize = scopeSize;
            _scope = new Course const*[scopeSize];
        }
        void setCourse(int index, Course const* course) { _scope[index] = course; }

        void allocateScores(int numEntries) {
            _numEntries = numEntries;
            _scores = new double[numEntries];
        }
        void setScore(int index, double score) { _scores[index] = score; }

        // ---- the rule of five (this object owns two heap arrays) ----
        ~PairingFactor() {
            delete[] _scope;
            delete[] _scores;
        }

        PairingFactor(PairingFactor const& other)
            : _scopeSize(other._scopeSize), _scope(nullptr),
              _numEntries(other._numEntries), _scores(nullptr) {
            if (_scopeSize > 0) {
                _scope = new Course const*[_scopeSize];
                for (int i = 0; i < _scopeSize; ++i) _scope[i] = other._scope[i];
            }
            if (_numEntries > 0) {
                _scores = new double[_numEntries];
                for (int i = 0; i < _numEntries; ++i) _scores[i] = other._scores[i];
            }
        }

        friend void swap(PairingFactor& a, PairingFactor& b) noexcept {
            std::swap(a._scopeSize, b._scopeSize);
            std::swap(a._scope, b._scope);
            std::swap(a._numEntries, b._numEntries);
            std::swap(a._scores, b._scores);
        }

        PairingFactor(PairingFactor&& other) noexcept : PairingFactor() { swap(*this, other); }
        PairingFactor& operator=(PairingFactor other) { swap(*this, other); return *this; }

        // ---- read-only accessors ----
        int scopeSize() const { return _scopeSize; }
        int courseAt(int index) const { return _scope[index]->courseIdx; }
        int numEntries() const { return _numEntries; }
        double scoreAt(int index) const { return _scores[index]; }
};

#endif  // PAIRING_FACTOR_HPP
