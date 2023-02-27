#include "Password.h"

Password::Password(const std::string &passwd, const std::string &name, const std::string &category,
                   const std::string &website, const std::string &login) : passwd(passwd), name(name),
                                                                           category(category), website(website),
                                                                           login(login) {


}
int Password::getId() const {
    return id;
}
const std::string &Password::getPasswd() const {
    return passwd;
}

const std::string &Password::getName() const {
    return name;
}

const std::string &Password::getCategory() const {
    return category;
}

const std::string &Password::getWebsite() const {
    return website;
}

const std::string &Password::getLogin() const {
    return login;
}

void Password::setId(int id) {
    Password::id = id;
}

void Password::setPasswd(const std::string &passwd) {
    Password::passwd = passwd;
}

void Password::setName(const std::string &name) {
    Password::name = name;
}

void Password::setCategory(const std::string &category) {
    Password::category = category;
}

void Password::setWebsite(const std::string &website) {
    Password::website = website;
}

void Password::setLogin(const std::string &login) {
    Password::login = login;
}