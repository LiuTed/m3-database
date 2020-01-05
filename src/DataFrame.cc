#include <sstream>
#include <iomanip>
#include <algorithm>
#include "DataFrame.hh"

namespace src
{

DataFrame::DataFrame(const std::string &filename)
{
    common::input_helper helper(filename,' ');
    size_t maxcols = 0;
    while(helper.hasNext())
    {
        auto vec = helper.next();
        if(vec.empty()) continue;
        this->data.emplace_back();
        Datablock &curr_data = this->data.back();
        curr_data.add(this->data.size()); // this is index!
        for(auto v : vec)
        {
            try
            {
                double d_v = std::stod(v);
                curr_data.add(d_v);
            }catch(...)
            {
                curr_data.add(std::nan("0"));
            }
        }
        maxcols = std::max(curr_data.size(), maxcols);
    }
    this->label.resize(maxcols);    // resize for label
    this->label[0] = Label::INDEX;
    for(size_t i=1;i<maxcols;i++)
    {
        label[i] = "Column " + std::to_string(i);
    }

}

DataFrame &DataFrame::setLabels(const std::vector<std::string> &labels)
{
    this->label = labels;
    return *this;
}

DataFrame &DataFrame::setLabel(const std::string &label, int index)
{
    if((unsigned)index > this->label.size())
        throw std::out_of_range("DataFrame::setLabel: index out of range");
    this->label[index] = label;
    return *this;
}

DataFrame &DataFrame::extend(const DataFrame &f)
{
    if(f.label.size() != this->label.size() ||
            !std::equal(label.begin(), label.end(), f.label.begin()))
        throw std::runtime_error("DataFrame::extend: two frames have different headers!");
    this->data.insert(data.end(), f.data.begin(), f.data.end());
    return *this;
}

DataFrame &DataFrame::addColumn(const std::string &name, double default_val)
{
    this->label.push_back(name);
    for(auto &db : this->data)
        db.add(default_val);
    return *this;
}

DataFrame &DataFrame::addRow()
{
    this->data.emplace_back();
    for(auto &l : this->label) this->data.back().add(0.0);
    return *this;
}

DataFrame DataFrame::select(const std::vector<int> &cols) const
{
    DataFrame ret;
    for(auto col : cols) ret.label.push_back(this->label[col]);
    for(auto db : this->data)
        ret.data.push_back(db.select(cols));
    return ret;
}

DataFrame DataFrame::select(const std::vector<std::string> &cols) const
{
    std::vector<int> indexes;
    for(auto col : cols)
    {
        auto it = std::find(label.begin(), label.end(), col);
        if(it == label.end())
            throw std::runtime_error("DataFrame::select: column " + col + " not found!");
        indexes.push_back(std::distance(label.begin(), it));
    }
    return this->select(indexes);
}

std::string DataFrame::getLabel(int index) const
{
    if((unsigned)index > this->label.size())
        throw std::out_of_range("DataFrame::getLabel: index out of range");
    return this->label[index];
}

std::string DataFrame::to_string() const
{ 
    std::stringstream sout;
    sout.setf(std::ios::fixed);
    int precision = 5;
    int width = 10;
    for(auto &l : this->label) width = std::max((size_t)width, l.length() + 1);
    if(data.size() > 0)
        for(size_t i = 0; i < this->data[0].size(); i++) 
            width = std::max((size_t)width, std::to_string(data[0].get(i)).length());
    for(auto &l : this->label)
        sout<<std::setw(width)<<l;
    sout<<std::endl;
    for(auto &db : this->data)
    {
        for(size_t i = 0; i < db.size(); i++)
        {
            sout.precision(precision);
            if(std::floor(db.get(i)) == db.get(i))
                sout.precision(0);
            sout<<std::setw(width)<<db.get(i);
        }
        sout<<std::endl;
    }
    return sout.str();
}

int DataFrame::getColumn(const std::string &col) const
{
    auto it = std::find(label.begin(), label.end(), col);
    if(it == label.end()) return -1;
    return std::distance(label.begin(), it);
}


void DataFrame::serialize(std::ostream &o) const
{
    throw std::runtime_error("Not implemented!");
}

void DataFrame::deserialize(std::istream &i) const
{
    throw std::runtime_error("Not implemented!");
}


/* ========================== predefined constants in class Label =========================== */

const std::string Label::TIME       = "time";
const std::string Label::LONGTITUDE = "longtitude";
const std::string Label::LATITIDE   = "latitude";
const std::string Label::SPEED      = "speed";
const std::string Label::THROUGHPUT = "throughput";
const std::string Label::RTT        = "RTT";
const std::string Label::LOSS       = "loss";
const std::string Label::RSRP       = "RSRP";
const std::string Label::HANDOVER   = "handover";
const std::string Label::CELLID     = "cell-id";
const std::string Label::INDEX      = "index";

} // namespace src
