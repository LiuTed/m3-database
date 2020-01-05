#include "Progress.hh"

namespace common
{

bool ProgressBar::_can_redraw(double new_val)
{
    const double THRESHOLD = 0.1;
    auto delta = new_val  / full * 100 - show_percent;
    if(delta > THRESHOLD) return true;
    return false;
}

ProgressBar::ProgressBar(std::ostream &ou, double full_range, const std::string &pref, int wid)
    : out(ou), prefix(pref), full(full_range), show_percent(0)
{
    this->setWidth(wid);
}

ProgressBar &ProgressBar::setWidth(int width)
{
    this->_construct_pbstr(width);
    return *this;
}

void ProgressBar::increase(double delta)
{
    bool flag = this->_can_redraw(current + delta);
    this->current += delta;
    if(flag) redraw();
}

void ProgressBar::set(double val)
{
    bool flag = this->_can_redraw(val);
    this->current = val;
    if(flag) redraw();
}

} // namespace common
