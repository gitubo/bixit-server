#include "http_router.hpp"

void HttpRouter::prepare_base_response(http::response<http::string_body>& res, const http::request<http::string_body>& req) {
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.keep_alive(req.keep_alive());
    auto originHeader = req.find(boost::beast::http::field::origin);
    std::string origin(originHeader->value().data(), originHeader->value().size());
    res.set(http::field::access_control_allow_origin, origin);
    res.set(http::field::access_control_allow_headers, "Origin, X-Requested-With, Content-Type, Accept, Authorization, Access-Control-Allow-Origin");
    res.set(http::field::access_control_allow_methods, "POST, GET, HEAD");
    res.set(http::field::access_control_allow_credentials, "true");
}

http::response<http::string_body> HttpRouter::bad_request(const http::request<http::string_body>& req, beast::string_view why) {
    http::response<http::string_body> res{http::status::bad_request, req.version()};
    prepare_base_response(res, req);
    res.set(http::field::content_type, "text/html");
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
}

http::response<http::string_body> HttpRouter::not_found(const http::request<http::string_body>& req, beast::string_view target) {
    http::response<http::string_body> res{http::status::not_found, req.version()};
    prepare_base_response(res, req);
    res.set(http::field::content_type, "text/html");
    res.body() = "The resource '" + std::string(target) + "' was not found.";
    res.prepare_payload();
    return res;
}

http::response<http::string_body> HttpRouter::server_error(const http::request<http::string_body>& req, beast::string_view what) {
    http::response<http::string_body> res{http::status::internal_server_error, req.version()};
    prepare_base_response(res, req);
    res.set(http::field::content_type, "text/html");
    res.body() = "An error occurred: '" + std::string(what) + "'";
    res.prepare_payload();
    return res;
}

http::response<http::string_body> HttpRouter::handle_request(
    const http::request<http::string_body>& req, 
    std::shared_ptr<bixit::catalog::SchemaCatalog> catalog
//    bixit::catalog::SchemaCatalog* catalog
) {
    // Accepted methods
    if( req.method() != http::verb::get &&
        req.method() != http::verb::post &&
        req.method() != http::verb::options &&
        req.method() != http::verb::head)
        return HttpRouter::bad_request(req, "Unsupported HTTP-method");
    
    if(req.method() == http::verb::options){
        http::response<http::string_body> res{http::status::ok, req.version()};
        prepare_base_response(res, req);
        return res;
    }
    
    // Routing to the right handler
    auto it = routes_.find({req.method(), req.target()});
    if (it != routes_.end()) {
        http::response<http::string_body> res{http::status::ok, req.version()};
        prepare_base_response(res, req);
        return it->second(req, catalog, res);
    }
    
    // Not found default response
    http::response<http::string_body> res{http::status::not_found, req.version()};
    prepare_base_response(res, req);
    res.body() = "404 Not Found";
    res.prepare_payload();
    return res;
}
       
    