#ifndef _PROGRESS_HH_
#define _PROGRESS_HH_

#include <iostream>
#include <string>
namespace common
{

class ProgressBar;


/**
 * class ProgressBar
 * show the progress bar to the output stream
 */
class ProgressBar
{
    private:
        std::ostream &out;
        std::string PBSTR;
        std::string prefix;
        double full;
        double current;         // real progress
        double show_percent;    // current percent showing in the terminal
    private:
        
        void _construct_pbstr(int width)
        {
            PBSTR.resize(width);
            std::fill(PBSTR.begin(), PBSTR.end(), '|');
        }

        /* show the bar */
        void _print()
        {
            auto width = PBSTR.length();
            char *buf = new char[width + 30];
            std::fill(buf, buf+width, 0);
            auto val = current / full * 100.;
            int lpad = (int)(current / full * width);
            int rpad = width - lpad;
            std::printf("\r%s: %5.1f%% [%.*s%*s]", prefix.c_str(), val, lpad, PBSTR.c_str(), rpad, ""); 
            this->show_percent = val;
            out<<buf;
            out.flush();
            delete []buf;
            if(current == full) out<<"Done !"<<std::endl;
        }

        bool _can_redraw(double new_val);

    public:
        /**
         * constructor
         * construct the bar using an output stream and full range,
         * default bar width will be 50
         */
        ProgressBar(std::ostream &out, double full_range, const std::string &prefix = "", int wid = 50);

        /**
         * method setWidth 
         * set the width of the bar
         */
        ProgressBar &setWidth(int width);

        void increase(double);
        void set(double);
        void redraw() {this->_print();}
};

} //namespace common



#endif
