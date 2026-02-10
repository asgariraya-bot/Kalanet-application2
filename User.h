#pragma once
#include <string>
using namespace std;

enum class Role { USER, ADMIN };

class User
{
private:
    int userId;
    string name;
    string username;
    string email;
    string phone;
    string passwordHash;
    Role role;

public:
    User(int id, const string& n, const string& u, const string& e, const string& p, const string& hash, Role r) : userId(id), name(n), username(u), email(e),phone(p), passwordHash(hash), role(r) {}

    User() {}

    string getUsername() const { return username; }
    string getPasswordHash() const { return passwordHash; }
    Role getRole() const { return role; }
    string getName() const { return name; }
    string getEmail() const { return email; }
    string getPhone() const { return phone; }

    void setPasswordHash(const string& hash) { passwordHash = hash; }
};
