#ifndef PROJEKTHASLA2_PASSWORD_H
#define PROJEKTHASLA2_PASSWORD_H

#include <vector>
#include <string>

class Password {
private:
    int id;

private:
    std::string passwd;
    std::string name;
    std::string category;
    std::string website;
    std::string login;
public:
    Password(const std::string &passwd, const std::string &name, const std::string &category,
             const std::string &website, const std::string &login);
    Password();

    void setPasswd(const std::string &passwd);

    void setName(const std::string &name);

    void setCategory(const std::string &category);

    void setWebsite(const std::string &website);

    void setLogin(const std::string &login);

    void setId(int id);

    const std::string &getPasswd() const;

    const std::string &getName() const;

    const std::string &getCategory() const;

    const std::string &getWebsite() const;

    const std::string &getLogin() const;

    int getId() const;
};

#endif //PROJEKTHASLA2_PASSWORD_H
