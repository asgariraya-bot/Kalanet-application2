#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <string>
#include <vector>

enum class Role { USER, ADMIN };

struct User {
    int id;
    std::string name;
    std::string username;
    std::string email;
    std::string phone;
    std::string passwordHash;
    Role role;

    User(int id_, const std::string& name_, const std::string& username_,
         const std::string& email_, const std::string& phone_,
         const std::string& hash_, Role role_)
        : id(id_), name(name_), username(username_), email(email_),
        phone(phone_), passwordHash(hash_), role(role_) {}

    std::string getUsername() const { return username; }
    std::string getPasswordHash() const { return passwordHash; }
};

class AuthService {
public:
    bool validatePhone(const std::string& phone);
    bool validateEmail(const std::string& email);
    bool validatePassword(const std::string& password);
    std::string hashPassword(const std::string& password);

    User signupUser(const std::string& name, const std::string& username,
                    const std::string& email, const std::string& phone,
                    const std::string& password);

    bool loginUser(const std::string& username, const std::string& password);
    bool loginAdmin(const std::string& password);

private:
    std::vector<User> users;
};

#endif // AUTHSERVICE_H
