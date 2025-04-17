#include "handlers.hpp"

http::response<http::string_body> handle_get_root(
    const http::request<http::string_body>& req, 
    datacarder::SchemaCatalog* catalog,
    http::response<http::string_body>& res) 
{

    res.set(http::field::content_type, "application/json");
    res.keep_alive(req.keep_alive());
    res.body() = R"({"message": "Welcome to the server!", "status": "ok"})";
    res.prepare_payload();
    return res;
}

http::response<http::string_body> handle_post_json_to_bitstream(
    const http::request<http::string_body>& req, 
    datacarder::SchemaCatalog* catalog,
    http::response<http::string_body>& res
) {
    if (req.body().empty())
    {
        return HttpRouter::bad_request(req, "Empty body in POST request");
    }
    try
    {
        // Parse the JSON content of the body
        nlohmann::json payload = nlohmann::json::parse(req.body());
        if(!payload.contains("message") || !payload["message"].is_object()){
            return HttpRouter::bad_request(req, "Field 'message' is missing or it is not an object");
        } else if(!payload.contains("message_type") || !payload["message_type"].is_string()){
            return HttpRouter::bad_request(req, "Field 'message_type' is missing or it is not a string");
        } 
        auto start = std::chrono::high_resolution_clock::now();
        auto rn_b2j = catalog->getAbstractChain(payload["message_type"]);
        if(!rn_b2j){
            return HttpRouter::not_found(req, "Schema '"+payload["message_type"].get<std::string>()+"' has not been found");
        }
        nlohmann::ordered_json inputJson = payload["message"].flatten();
        datacarder::BitStream result; 
        int retVal = rn_b2j->json_to_bitstream(inputJson, result);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        nlohmann::ordered_json responseBody;
        responseBody["metadata"] = {};
        responseBody["metadata"]["elapsed_time_ns"] = duration.count();
        nlohmann::ordered_json data;
        data["string"] = result.to_string();
        data["base64"] = result.to_base64();
        responseBody["data"] = data;       

        // Return a success response with the parsed data (you can format the response as needed)
        res.set(http::field::content_type, "application/json");
        res.body() = responseBody.dump();
        res.prepare_payload();
        return res;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        return HttpRouter::bad_request(req, "Invalid JSON format");
    }

}

http::response<http::string_body> handle_post_json_to_bitstream_with_schema(
    const http::request<http::string_body>& req, 
    datacarder::SchemaCatalog* catalog,
    http::response<http::string_body>& res
) {
    if (req.body().empty())
    {
        return HttpRouter::bad_request(req, "Empty body in POST request");
    }
    
    try
    {
        // Parse the JSON content of the body
        nlohmann::json payload = nlohmann::json::parse(req.body());

        if(!payload.contains("message") || !payload["message"].is_object()){
            return HttpRouter::bad_request(req, "Field 'message' is missing or it is not an object");
        } else if(!payload.contains("schema") || !payload["schema"].is_object()){
            return HttpRouter::bad_request(req, "Field 'schema' is missing or it is not an object");
        }

        auto start = std::chrono::high_resolution_clock::now();
        auto rn_b2j = catalog->parseSchemaOnTheFly(payload["schema"]);
        if(rn_b2j == nullptr){
            return HttpRouter::server_error(req, "Schema has not been correctly parsed");
        }
        nlohmann::ordered_json inputJson = payload["message"].flatten();
        datacarder::BitStream result; 
        int retVal = rn_b2j->json_to_bitstream(inputJson, result);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        nlohmann::ordered_json responseBody;
        responseBody["metadata"] = {};
        responseBody["metadata"]["elapsed_time_ns"] = duration.count();
        nlohmann::ordered_json data;
        data["string"] = result.to_string();
        data["base64"] = result.to_base64();
        responseBody["data"] = data;        
        res.set(http::field::content_type, "application/json");
        res.body() = responseBody.dump();
        res.prepare_payload();
        return res;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        return HttpRouter::bad_request(req, "Invalid JSON format");
    }
}

http::response<http::string_body> handle_post_bitstream_to_json_with_schema(
    const http::request<http::string_body>& req, 
    datacarder::SchemaCatalog* catalog,
    http::response<http::string_body>& res
) {
    if (req.body().empty())
    {
        return HttpRouter::bad_request(req, "Empty body in POST request");
    }
    
    try
    {
        // Parse the JSON content of the body
        nlohmann::json payload = nlohmann::json::parse(req.body());

        if(!payload.contains("message_base64") || !payload["message_base64"].is_string()){
            return HttpRouter::bad_request(req, "Field 'message_base64' is missing or it is not a string");
        } else if(!payload.contains("schema") || !payload["schema"].is_object()){
            return HttpRouter::bad_request(req, "Field 'schema' is missing or it is not an object");
        }
    
        auto start = std::chrono::high_resolution_clock::now();
        auto rn_b2j = catalog->parseSchemaOnTheFly(payload["schema"]);
        if(rn_b2j == nullptr){
            return HttpRouter::server_error(req, "Schema has not been correctly parsed");
        }
        auto bs_b64 = datacarder::BitStream(payload["message_base64"]);
        nlohmann::ordered_json result;
        int retVal = rn_b2j->bitstream_to_json(bs_b64, result);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        nlohmann::ordered_json responseBody;
        responseBody["metadata"] = {};
        responseBody["metadata"]["elapsed_time_ns"] = duration.count();
        responseBody["data"] = result.unflatten();
        
        // Return a success response with the parsed data (you can format the response as needed)
        res.set(http::field::content_type, "application/json");
        res.body() = responseBody.dump();
        res.prepare_payload();
        return res;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        return HttpRouter::bad_request(req, "Invalid JSON format");
    }

}

http::response<http::string_body> handle_post_bitstream_to_json(
    const http::request<http::string_body>& req, 
    datacarder::SchemaCatalog* catalog,
    http::response<http::string_body>& res
) {
    if (req.body().empty())
    {
        return HttpRouter::bad_request(req, "Empty body in POST request");
    }
    
    try
    {
        // Parse the JSON content of the body
        nlohmann::json payload = nlohmann::json::parse(req.body());

        if(!payload.contains("message_base64") || !payload["message_base64"].is_string()){
            return HttpRouter::bad_request(req, "Field 'message_base64' is missing or it is not a string");
        } else if(!payload.contains("message_type") || !payload["message_type"].is_string()){
            return HttpRouter::bad_request(req, "Field 'message_type' is missing or it is not a string");
        } 
    
        auto start = std::chrono::high_resolution_clock::now();
        auto rn_b2j = catalog->getAbstractChain(payload["message_type"]);
        if(!rn_b2j){
            return HttpRouter::not_found(req, "Schema '"+payload["message_type"].get<std::string>()+"' has not been found");
        }
        auto bs_b64 = datacarder::BitStream(payload["message_base64"]);
        nlohmann::ordered_json result;
        int retVal = rn_b2j->bitstream_to_json(bs_b64, result);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        nlohmann::ordered_json responseBody;
        responseBody["metadata"] = {};
        responseBody["metadata"]["elapsed_time_ns"] = duration.count();
        responseBody["data"] = result.unflatten();
        
        
        // Return a success response with the parsed data (you can format the response as needed)
        res.set(http::field::content_type, "application/json");
        res.body() = responseBody.dump();
        res.prepare_payload();
        return res;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        return HttpRouter::bad_request(req, "Invalid JSON format");
    }

    
}