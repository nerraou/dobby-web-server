#pragma once

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include "lib.hpp"

class ContentType
{
public:
    static std::map<std::string, std::string> contentTypes;
    static void loadContentTypes(const std::string &contentTypePath);
};
