#ifndef _DATAFRAME_HH_
#define _DATAFRAME_HH_

#include <vector>
#include <iostream>
#include "../common/common.hh"
#include "DataBlock.hh"
#include "Serializeable.hh"

namespace src
{
class Label;
class DataFrame;

/**
 * class Label
 * predefined const string labels for every column in DataFrame
 */
class Label
{
    public:
        const static std::string TIME;          /* timestamp, 0~86400 */
        const static std::string LONGTITUDE;    /* longtitude */
        const static std::string LATITIDE;      /* latitude */
        const static std::string SPEED;         /* speed, km/h */
        const static std::string THROUGHPUT;    /* thp, bytes/sec */
        const static std::string RTT;           /* rtt, seconds */
        const static std::string LOSS;          /* loss, 0~1 */
        const static std::string RSRP;          /* rsrp, -140 ~ -50 */
        const static std::string HANDOVER;      /* handover, 0~4, 0 means no handover */
        const static std::string CELLID;        /* cell id, should be an int */
        const static std::string INDEX;         /* index of the entry */
};

/**
 * class DataFrame
 * the main data structure for database
 */
class DataFrame : public Serializable
{
    private:
        std::vector<std::string> label;
        std::vector<Datablock> data;

    public:
        DataFrame() = default;
        /* read the data file and construct the DataFrame */
        DataFrame(const std::string &filename); 

        DataFrame(DataFrame &&rhs): label(std::move(rhs.label)), data(std::move(rhs.data)) {}

        DataFrame(const DataFrame&) = default;
        DataFrame& operator = (const DataFrame& rhs)
        {
            label = rhs.label;
            data = rhs.data;
        }

        /* return column count */
        size_t columns() const { return label.size(); }
        /* return row count */
        size_t rows() const {return data.size(); }

        /**
         * method setLabels. 
         * Set the labels of each column
         */
        DataFrame &setLabels(const std::vector<std::string> &label);

        /**
         * method setLabel
         * Set the label of a specified column 
         */
        DataFrame &setLabel(const std::string &label, int index);

        /**
         * method extend. 
         * Extend the current DataFrame.
         * Throw std::runtime_error, if headers not match
         */
        DataFrame &extend(const DataFrame &df);

        /**
         * method addColumn. 
         * add a new column into this dataframe
         */
        DataFrame &addColumn(const std::string &col_name, double default_val = 0);

        /**
         * method addRow. 
         * Add a new row initialized with all 0
         */
        DataFrame &addRow();

        /* clear all the rows*/
        DataFrame &clear() {this->data.clear(); return *this;}

        /**
         * method select. 
         * Select the data by the given column index. 
         * returns a new DataFrame
         */
        DataFrame select(const std::vector<int> &) const; 

        /**
         * method select. 
         * Select the data by the given column names. 
         * Throws std::runtime_error if the given name is not found.
         * returns a new DataFrame
         */
        DataFrame select(const std::vector<std::string> &) const;

        /**
         * method where. 
         * Filter the data by the given unary predicate on column "col". 
         * The data will be filtered out if p(data[i].get(col)) == false.
         */
        template<typename Pred> DataFrame where(int col, Pred p) const;

        /**
         * method where(2). 
         * Filter the data by the given unary predicate on the whole Datablock. 
         * The data will be filtered out if p(data[i]) == false.
         */
        template<typename Pred> DataFrame where(Pred p) const;

        /**
         * method where(3). 
         * Filter the data by the given unary predicates.
         * Used to speed up mutiple filters
         */
        template<typename Pred> DataFrame where(const std::vector<int> &, const std::vector<Pred> &ps) const;

        /* return the label of a specified column */
        std::string getLabel(int index) const;
        std::vector<std::string> getLabels() const {return this->label;}

        /* return the raw data */
        std::vector<Datablock> &getData() {return this->data;}
        const std::vector<Datablock> &getData() const {return this->data;}

        /**
         * method getColumn. 
         * find the column by the label.
         * Return -1 if not found
         */
        int getColumn(const std::string &) const;

        /* print the Frame */
        std::string to_string() const;

        virtual void serialize(std::ostream &o) const override;
        virtual void deserialize(std::istream &i) const override;
};

} // namespace src

/* ================ Template implementation ================== */
namespace src
{

template<typename Pred> 
DataFrame DataFrame::where(int col, Pred p) const
{
    DataFrame ret;
    ret.setLabels(this->label);
    for(auto db: this->data)
        if(p(db.get(col))) ret.data.push_back(db);
    return ret;
}

template<typename Pred> 
DataFrame DataFrame::where(Pred p) const
{
    DataFrame ret;
    ret.setLabels(this->label);
    for(auto db: this->data)
        if(p(db)) ret.data.push_back(db);
    return ret;
}

template<typename Pred> 
DataFrame DataFrame::where(const std::vector<int> &cols, const std::vector<Pred> & ps) const
{
    DataFrame ret;
    ret.setLabels(this->label);
    for(auto db: this->data)
    {
        bool add = true;
        for(size_t i = 0; i < cols.size(); i++)
        {
            if(!ps[i](db.get(cols[i]))) 
            {
                add = false;
                break;
            }
        }
        if(add) ret.data.push_back(db);
    }
    return ret;
}

} // namespace src

#endif
