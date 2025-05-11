#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <nlohmann/json.hpp>

#include <bixit.hpp>


namespace beast = boost::beast;
namespace http = beast::http; 

namespace std {
    template <>
    struct hash<std::pair<boost::beast::http::verb, boost::core::basic_string_view<char>>> {
        size_t operator()(const std::pair<boost::beast::http::verb, boost::core::basic_string_view<char>>& p) const {
            size_t hash_verb = std::hash<int>()(static_cast<int>(p.first)); // boost::beast::http::verb è un enum
            size_t hash_string_view = std::hash<std::string_view>()(std::string_view(p.second));
            return hash_verb ^ (hash_string_view << 1); // Combina gli hash
        }
    };
}

using Handler = std::function<http::response<http::string_body>(
    const http::request<http::string_body>&, 
    std::shared_ptr<bixit::catalog::SchemaCatalog>,
    //bixit::catalog::SchemaCatalog*,
    http::response<http::string_body>&
)>;

class HttpRouter {
    private:
        std::unordered_map<std::pair<http::verb, boost::core::string_view>, Handler> routes_;

    public:
        HttpRouter() {};

        void add_route(http::verb method, const boost::core::string_view& path, Handler handler) {
            routes_[{method, path}] = handler;
        };
        http::response<http::string_body> handle_request(const http::request<http::string_body>&, std::shared_ptr<bixit::catalog::SchemaCatalog>);
        static http::response<http::string_body> bad_request(const http::request<http::string_body>&, beast::string_view);
        static http::response<http::string_body> not_found(const http::request<http::string_body>&, beast::string_view);
        static http::response<http::string_body> server_error(const http::request<http::string_body>&, beast::string_view);

    private:
        static void prepare_base_response(http::response<http::string_body>&, const http::request<http::string_body>&);

};