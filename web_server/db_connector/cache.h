#ifndef ARCHI_CACHE_H
#define ARCHI_CACHE_H


#include <string>

namespace database
{
    class Cache
    {
    private:
        Cache();

    public:
        static Cache get();
        void put(std::string& login, const std::string& val);
        bool get(std::string& login, std::string& val);
        size_t size();
        void remove(std::string& login);
        void remove_all();
    };
}

#endif //ARCHI_CACHE_H
