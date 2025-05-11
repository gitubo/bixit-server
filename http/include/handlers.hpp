#pragma once

#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>

#include "http_router.hpp"

namespace http = boost::beast::http;

http::response<http::string_body> handle_get_root(const http::request<http::string_body>&, std::shared_ptr<bixit::catalog::SchemaCatalog>, http::response<http::string_body>&);
http::response<http::string_body> handle_post_bitstream_to_json(const http::request<http::string_body>&, std::shared_ptr<bixit::catalog::SchemaCatalog>, http::response<http::string_body>&);
http::response<http::string_body> handle_post_bitstream_to_json_with_schema(const http::request<http::string_body>&, std::shared_ptr<bixit::catalog::SchemaCatalog>, http::response<http::string_body>&);
http::response<http::string_body> handle_post_json_to_bitstream(const http::request<http::string_body>&, std::shared_ptr<bixit::catalog::SchemaCatalog>, http::response<http::string_body>&);
http::response<http::string_body> handle_post_json_to_bitstream_with_schema(const http::request<http::string_body>&, std::shared_ptr<bixit::catalog::SchemaCatalog>, http::response<http::string_body>&);

