#include "AuthService.h"
#include <algorithm>

std::string fixPhone(const std::string& input) {
    std::string result;
    for(char c : input) {
        if(c >= 0x30 && c <= 0x39) result += c;
        else if(c >= 0x06F0 && c <= 0x06F9) result += (c - 0x06F0 + '0');
    }
    return result;
}

bool AuthService::validatePhone(const std::string& phone)
{
    std::string cleaned = fixPhone(phone);
    if(cleaned.length() != 11) return false;
    return std::all_of(cleaned.begin(), cleaned.end(), [](char c){ return c >= '0' && c <= '9'; });
}

bool AuthService::validateEmail(const std::string& email) {
    return email.find("@gmail.com") != std::string::npos;
}

bool AuthService::validatePassword(const std::string& password) {
    if(password.length() < 8) return false;
    int letters = 0;
    for(char c : password)
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) letters++;
    return letters >= 2;
}

std::string AuthService::hashPassword(const std::string& password) {
    std::string hash = "";
    for(char c : password) hash += c + 1;
    return hash;
}

User AuthService::signupUser(const std::string& name, const std::string& username,
                             const std::string& email, const std::string& phone,
                             const std::string& password) {
    std::string hash = hashPassword(password);
    int id = users.size() + 1;
    User u(id, name, username, email, phone, hash, Role::USER);
    users.push_back(u);
    return u;
}

bool AuthService::loginUser(const std::string& username, const std::string& password) {
    std::string hash = hashPassword(password);
    for(User &u : users)
        if(u.getUsername() == username && u.getPasswordHash() == hash) return true;
    return false;
}

bool AuthService::loginAdmin(const std::string& password) {
    return password == "1111";
}
