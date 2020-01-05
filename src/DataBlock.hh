#ifndef _DATA_BLOCK_HH_
#define _DATA_BLOCK_HH_

#include <vector>

namespace src
{

class Datablock;

template<typename T> class less;
template<> class less<Datablock>;

/**
 * class Datablock
 * The basic element for DataFrame
 */
class Datablock
{
    private:
        std::vector<double> cols;

    public:
        /* getters */
        double      get(int index) const;
        Datablock   select(const std::vector<int> &) const;

        /* setters */
        void        set(int index, double value);
        void        add(double value);  // same as push_back

        /* information */
        size_t      size() const;
};

/**
 * less predicate for Datablock 
 */
template<>
class less<Datablock>
{
    private:
        int col;
    public:
        less(int c) : col(c) {}
        bool operator()(const Datablock &l, const Datablock &r)
        {
            return l.get(col) < r.get(col);
        }
};

} // namespace src

#endif
