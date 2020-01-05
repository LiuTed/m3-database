#ifndef _SERIALIZEABLE_HH_
#define _SERIALIZEABLE_HH_

#include <istream>
#include <ostream>

namespace src
{

class Serializable;

class Serializable
{
    public:
        virtual void serialize(std::ostream &o) const = 0;
        virtual void deserialize(std::istream &i) const = 0;
        virtual ~Serializable() = default;
};

} // namespace src

#endif
