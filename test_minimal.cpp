#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

// Include your actual library headers here:
// #include "bixit/logger/logger.h"
// #include "bixit/abstract_chain/chain_node.h"
// #include "bixit/catalog/schema.h"
// #include "bixit/catalog/chain_factory.h"
// #include "bixit/catalog/schema_catalog.h"
#include <bixit.hpp>


int main() {
    auto catalog = std::make_shared<bixit::catalog::SchemaCatalog>(bixit::logger::Logger::Level::DEBUG);
    return 0;
}