#include "utilities.h"

#include <stdexcept>

#ifdef _WIN32
std::string getCurrentUser(){
    char user_name[UNLEN+1];
    DWORD user_name_size = sizeof(user_name);
    if(GetUserName(user_name, &user_name_size)) return user_name;
    return "default";
}
#elifdef __APPLE__
std::string getCurrentUser(){
    return getenv("USER");
}
#elifdef __linux__
std::string getCurrentUser(){
    return getenv("USERNAME");
}
#else
std::string getCurrentUser(){
    return "default"
}
#endif

