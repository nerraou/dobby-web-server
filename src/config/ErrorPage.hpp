#pragma once

#include <iostream>
#include <vector>

struct ErrorPage
{
    std::vector<int> statusCodes;
    std::string path;
};