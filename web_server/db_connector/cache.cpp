#include "cache.h"
#include "../config/config.h"

#include "Poco/Logger.h"
#include <ignite/thin/ignite_client.h>
#include <ignite/thin/ignite_client_configuration.h>
#include <ignite/thin/cache/cache_peek_mode.h>
#include <exception>

using Poco::Logger;

static ignite::thin::IgniteClient _client;
static ignite::thin::cache::CacheClient<std::string, std::string> _cache;

namespace database
{
    Cache::Cache()
    {
        ignite::thin::IgniteClientConfiguration cfg;
        cfg.SetEndPoints(Config::get().get_cache_servers());
        cfg.SetPartitionAwareness(true);
        try
        {
            _client = ignite::thin::IgniteClient::Start(cfg);
            _cache = _client.GetOrCreateCache<std::string, std::string>("persons");
        }
        catch (ignite::IgniteError& err){
            std::string err_string = err.what();
            Logger::root().error("Ignite Error: " + err_string);
            throw;
        }
    }

    Cache Cache::get()
    {
        static Cache instance;
        return instance;
    }

    void Cache::put(std::string& login, const std::string& val){
        _cache.Put(login, val);
    }

    void Cache::remove(std::string& login){
        _cache.Remove(login);
    }

    size_t Cache::size(){
        return _cache.GetSize(ignite::thin::cache::CachePeekMode::ALL);
    }

    void Cache::remove_all(){
        _cache.RemoveAll();;
    }

    bool Cache::get(std::string& login, std::string& val){
        try{
            val = _cache.Get(login);
            return true;
        }catch(...){
            throw std::logic_error("key not found in cache");
        }
    }
}