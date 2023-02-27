#ifndef PROJEKTHASLA2_MANAGER_H
#define PROJEKTHASLA2_MANAGER_H

#include "Password.h"
#include <vector>

class Manager {
public:
    Manager();
    const std::string &encrypt(std::string &passwd) const;
    const std::string &decrypt(std::string &passwd) const;
    static bool isSafe(std::string &passwd) ;
    bool isDuplicate(std::string &passwd) const;
    static std::string genPasswd(int count, bool upperCaseAndLower, bool symbols);
    bool addPassword(Password passwd, int id = -1);
    bool removePassword();
    bool addCategory(const std::string &category);
    bool removeCategory(std::string &category);
    void searchPass();
    bool removeAll(std::string &category);
    bool save() const;
    bool load();
    bool init(const std::string &path, const std::string &password);
    void showCategories();
    void showPasswords();
    void editData();
    int passwordCount();
    void sort();
    bool isCategoryAvailable(std::string &category) const;

private:
    std::vector<std::string> categories;
    std::vector<Password> passwords;
    int latestId;
    std::string path;
    std::size_t filePasswordHash;
    char* timestamp;

};
#endif //PROJEKTHASLA2_MANAGER_H