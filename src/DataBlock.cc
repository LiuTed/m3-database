#include <stdexcept>
#include "DataBlock.hh"

namespace src
{

/* ================ class Datablock ================ */
double Datablock::get(int index) const
{
    if((unsigned)index > cols.size()) 
        throw std::out_of_range("Datablock::get: index out of range");
    return cols[index];
}

Datablock Datablock::select(const std::vector<int> &c) const
{
    Datablock ret;
    for(auto col : c)
        ret.add(this->get(col));
    return ret;
}

void Datablock::set(int index, double value)
{
    if((unsigned)index > cols.size()) 
        throw std::out_of_range("Datablock::get: index out of range");
    this->cols[index] = value;
}

void Datablock::add(double value)
{
    cols.push_back(value);
}

size_t Datablock::size() const
{
    return this->cols.size();
}


} // namespace src
