#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include "Manager.h"
#include <ctime>
Manager::Manager() {}

/**
Inicjalizuje managera

Funkcja ustawia ścieżkę do pliku z hasłami oraz hasło pliku (jego skrót) w polach klasy Manager, a także inicjalizuje
 zmienną latestId na 0. Następnie wywołuje metodę load, która odpowiada za odczytanie danych z pliku.

@param path ścieżka do pliku z hasłami
@param password hasło pliku
@return Wartość logiczna określająca, czy operacja się powiodła
*/
bool Manager::init(const std::string &path, const std::string &password) {
    this->path = path;
    this->filePasswordHash = std::hash<std::string>{}(password);
    latestId = 0;
    return load();
}

/**
Dzieli ciąg znaków na fragmenty

Funkcja dzieli ciąg znaków na fragmenty, korzystając z podanego separatora.
Każdy fragment jest zapisywany jako element wektora std::vectorstd::string.

@param str ciąg znaków do podzielenia
@param delimiter separator użyty do podzielenia ciągu znaków
@return Wektor zawierający podzielony ciąg znaków
*/
std::vector<std::string> splitString(const std::string& str,
                                     const std::string& delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.size();
    }

    strings.push_back(str.substr(prev));

    return strings;
}

/**

Zapisuje dane do pliku

Funkcja tworzy ciąg znaków zawierający dane managera, w tym id ostatniego hasła, liczbę haseł, hasła, nazwy, kategorie,
 adresy stron oraz loginy zapisane w managerze. Następnie dane są szyfrowane. Funkcja zapisuje także timestamp.
 Następnie dane są zapisywane do pliku pod ścieżką zapisaną w polu klasy Manager.

@return Wartość logiczna określająca, czy operacja się powiodła
*/
bool Manager::save() const {
    try {
        std::string data;
        data.append(std::to_string(latestId) + '\n');
        data.append(std::to_string(passwords.size()) + '\n');
        for (Password p : passwords) {
            data.append(std::to_string(p.getId()) + '\n');
            data.append(p.getPasswd() + '\n');
            data.append(p.getName() + '\n');
            data.append(p.getCategory() + '\n');
            data.append(p.getWebsite() + '\n');
            data.append(p.getLogin() + '\n');
        }
        data.append(std::to_string(categories.size()) + '\n');
        for (std::string c : categories) {
            data.append(c + '\n');
        }
        // Encrypt
        data = encrypt(data);
        // Save timestamp
        time_t now = time(0);
        // Save to file
        char* dt = ctime(&now);
        data.append("\n");
        data.append(dt);
        std::fstream file;
        file.open(path, std::fstream::out);
        file << data;
        file.close();
    } catch (...) {
        return false;
    }
    return true;
}
/**
Wczytuje dane z pliku

Funkcja otwiera plik pod ścieżką zapisaną w polu klasy Manager i wczytuje z niego dane.
 Następnie dane są deszyfrowane. Funkcja zapisuje także timestamp. Następnie dane są deserializowane,
 a ich wartości są ustawiane w polach klasy Manager. Jeśli plik nie istnieje, tworzy nowy plik z danymi z managera.

@return Wartość logiczna określająca, czy operacja się powiodła
*/
bool Manager::load() {
    int counter = 1;
    // Load from file
    std::fstream file;
    file.open(path, std::fstream::in);
    if (!file.is_open()) {
        std::cout << "Creating a new file." << std::endl;
        return save();
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string data(buffer.str());
    file.close();
    // Decrypt
    data = decrypt(data);
    time_t now = time(0);
    timestamp = ctime(&now);
    // Deserialize
    try {
        std::vector<std::string> strings = splitString(data, "\n");
        latestId = std::stoi(strings[0]);
        int password_count = std::stoi(strings[1]);
        for (int i = 0; i < password_count; i++) {
            // Skip latestId, count and 'i' passwords
            int offset = 2 + i * 6;
            int id = std::stoi(strings[offset]);
            std::string password = strings[offset + 1];
            std::string name = strings[offset + 2];
            std::string category = strings[offset + 3];
            std::string website = strings[offset + 4];
            std::string login = strings[offset + 5];
            addPassword(Password(password, name, category, website, login), id);
        }

        int category_count = std::stoi(strings[2 + password_count * 6]);
        for (int i = 0, j = categories.size()-2; i < category_count; i++) {
            addCategory(strings[2 + password_count * 6 + (j++)]);
        }
    } catch(...) {
        std::cout << "File password is incorrect or file is corrupted." << std::endl;
        save();
        return false;
    }
    save();
    return true;
}
/**
Sprawdza bezpieczeństwo hasła

Funkcja przyjmuje jako argument hasło, a następnie oblicza wskaźnik bezpieczeństwa hasła na podstawie jego składni
 (liczba znaków specjalnych, wielkich i małych liter oraz cyfr). Jeśli wskaźnik bezpieczeństwa jest większy niż 1.6,
 funkcja zwraca true, w przeciwnym razie zwraca false.

@param passwd hasło do sprawdzenia
@return Wartość logiczna określająca, czy hasło jest bezpieczne
*/
bool Manager::isSafe(std::string &passwd) {
    double safetyIndicator = 0;
    bool hasSpecialChar = false;
    bool hasUpperCase = false;
    bool hasLowerCase = false;
    bool hasNum = false;

    for (char c : passwd) {
        if (!hasSpecialChar && !isalnum(c)) {
            safetyIndicator += 3;
        }
    }
    for (char c : passwd) {
        if (!hasUpperCase && isupper(c)) {
            safetyIndicator += 1;
        }
    }
    for (char c : passwd) {
        if (!hasLowerCase && islower(c)) {
            safetyIndicator += 1;
        }
    }
    for(char c : passwd){
        if (!hasNum && isdigit(c)){
            safetyIndicator += 2;
        }
    }
    if (safetyIndicator/passwd.length() >= 1.6) {

        return true;
    }
    return false;
}
/**
Sprawdza czy hasło jest duplikatem

Funkcja przyjmuje jako argument hasło, a następnie sprawdza czy jest ono już zapisane w managerze w wektorze passwords.
Jeśli tak, funkcja zwraca true, w przeciwnym razie zwraca false.

@param passwd hasło do sprawdzenia
@return Wartość logiczna określająca, czy hasło jest już zapisane w managerze
*/
bool Manager::isDuplicate(std::string &passwd) const {
    for(int i = 0; i<passwords.size(); i++){
        if(passwd == passwords[i].getPasswd()){
            return true;
        }
    }
    return false;
}
/**
Sprawdza, czy podana kategoria istnieje w liście kategorii.

Funkcja przyjmuje jako parametr nazwę kategorii i porównuje ją z każdym elementem znajdującym się w wektorze kategorii.
Jeśli znajdzie taką samą nazwę, zwraca true, w przeciwnym razie false.

@param category nazwa kategorii, którą chcemy sprawdzić
@return true jeśli kategoria istnieje, false w przeciwnym razie
*/
bool Manager::isCategoryAvailable(std::string &category) const{
    for(int i = 0; i<categories.size(); i++){
        if(category == categories[i]){
            return true;
        }
    }
    return false;
}
/**
Generuje losowe hasło

Funkcja przyjmuje jako argument liczbę znaków, które ma zawierać hasło oraz dwie wartości logiczne określające, czy ma zawierać litery oraz znaki specjalne.
Funkcja generuje losowe hasło przy użyciu generatora liczb losowych.
Jeśli hasło jest bezpieczne, zwraca je jako std::string.

@param count liczba znaków w hasle
@param letters określa czy ma zawierać litery
@param symbols określa czy ma zawierać znaki specjalne
@return losowe hasło jako std::string
*/

std::string Manager::genPasswd(int count, bool letters, bool symbols) {
    std::string password;
    std::mt19937 rng;
    char c;
    std::uniform_int_distribution<std::mt19937::result_type> dist(33, 126);
    std::uniform_int_distribution<std::mt19937::result_type> dist_lettersAndNums(48, 122);
    std::uniform_int_distribution<std::mt19937::result_type> dist_symbAndNum(33, 64);
    std::uniform_int_distribution<std::mt19937::result_type> dist_nums(48, 57);
    std::uniform_int_distribution<std::mt19937::result_type> randomSubtract(6, 15);
    rng.seed(std::random_device()());
    while (true) {
        password = "";
        for (int i = 0; i < count; i++) {
            if(letters && symbols) {
                c = (char) dist(rng);
            }
            else if(!letters && symbols){
                c = (char) dist_symbAndNum(rng);

            } else if(letters && !symbols){
                c = (char) dist_lettersAndNums(rng);
                if((c>=91 && c<=96) || (c>=58 && c<=64)){
                    int randomNumber = (int) randomSubtract(rng);
                    int charAsInt = c;
                    int resultAsInt = charAsInt + randomNumber;
                    c = (char) resultAsInt;
                }
            } else if(!letters && !symbols){
                c = (char) dist_nums(rng);
            }
            password += c;
        }
        if (isSafe(password)) {
            std::cout << password << std::endl;
            return password;
        }
    }
}
/**
Dodaje hasło do managera

Funkcja przyjmuje jako argument hasło w postaci obiektu Password oraz id tego hasła.
Funkcja przypisuje nowe id hasłu (latestId+1).Następnie hasło jest dodawane do wektora passwords w managerze i zapisywane do pliku.
 Jeśli id jest równe -1, funkcja wraca do menu.

@param passwd hasło w postaci obiektu Password
@param id id hasła
@return Wartość logiczna określająca, czy operacja się powiodła
 */

bool Manager::addPassword(Password passwd, int id) {
    if (id == -1) {
        passwd.setId(latestId++);
    } else {
        passwd.setId(id);
    }
    passwords.push_back(passwd);
    addCategory(passwd.getCategory());
    return save();
}
/**
Usuwa hasło z managera

Funkcja pyta użytkownika o id hasła do usunięcia, a następnie usuwa je z wektora passwords w managerze.
 Jeśli id jest równe -1, funkcja wraca do menu. Gdy wektor passwords jest pusty, funkcja wyświetla komunikat na ekranie
 i wraca do menu.

@return Wartość logiczna określająca, czy operacja się powiodła
*/

bool Manager::removePassword() {
    if(passwords.empty()) {
        std::cout << "No available passwords to edit. Returning to menu" << std::endl;
        return false;
    }
    showPasswords();
    std::cout << "Enter password id to remove, or type '-1' to exit this function. " << std::endl;
    int id;
    std::cin >> id;
        if(id == -1){
            std::cout << "Returning to menu" << std::endl;
            return false;
        }
    std::erase_if(passwords, [&](Password& password) -> bool {
        return password.getId() == id;
    });
    save();
    return true;
}

/**
Dodaje kategorię do wektora categories w managerze

Funkcja przyjmuje jako argument nazwę kategorii.
Sprawdza czy kategoria już istnieje w wektorze categories w managerze.
Jeśli nie, dodaje kategorię do wektora i zapisuje dane do pliku.

@param category nazwa kategorii
@return Wartość logiczna określająca, czy operacja się powiodła
*/
bool Manager::addCategory(const std::string &category) {
    for (int i = 0; i < categories.size(); i++) {
        if (categories[i] == category) {
            return false;
        }
    }
    categories.push_back(category);
    save();
    return true;
}
/**
Usuwa kategorię z managera

Funkcja przyjmuje jako argument nazwę kategorii.
Usuwa wszystkie hasła z wektora passwords, które należą do tej kategorii.
Następnie usuwa kategorię z wektora categories w managerze i zapisuje dane do pliku.

@param category nazwa kategorii
@return Wartość logiczna określająca, czy operacja się powiodła
*/

bool Manager::removeCategory(std::string &category) {
    removeAll(category);
    std::erase(categories, category);
    return save();
}
/**

Szyfruje dane

Funkcja przyjmuje jako argument referencję do std::string z danymi do zaszyfrowania.
Funkcja szyfruje dane przez przesunięcie każdego znaku o wartość hasha hasła pliku.

@param data referencja do std::string z danymi do zaszyfrowania
@return referencja do zaszyfrowanych danych
*/
const std::string &Manager::encrypt(std::string &data) const {
    for (int i = 0; i < data.size(); i++) {
        data[i] += filePasswordHash;
    }
    return data;
}
/**

Deszyfruje dane

Funkcja przyjmuje jako argument referencję do std::string z zaszyfrowanymi danymi.
Funkcja deszyfruje dane przez przesunięcie każdego znaku o wartość negatywną hasha hasła pliku.

@param data referencja do std::string z zaszyfrowanymi danymi
@return referencja do odszyfrowanych danych
*/
const std::string &Manager::decrypt(std::string &data) const {
    for (int i = 0; i < data.size(); i++) {
        data[i] -= filePasswordHash;
    }
    return data;
}
/**
Wyszukuje hasła

Funkcja wyszukuje hasła w wektorze passwords na podstawie stringa wprowadzonego przez użytkownika.
Jeśli zostanie znalezione hasło o nazwie zawierającej podany string, to zostanie ono wyświetlone na ekranie.
Jeśli hasło nie zostanie znalezione, użytkownik zostanie poinformowany i będzie mógł spróbować ponownie lub zakończyć wyszukiwanie.
 Gdy wektor passwords jest pusty, funkcja wyświetla komunikat na ekranie i wraca do menu.
*/
void Manager::searchPass() {
    if(passwords.empty()) {
        std::cout << "No available passwords to search. Returning to menu" << std::endl;
        return;
    }
        std::string search;
        std::cout << "Enter the search string: ";
        while (true) {
        std::cin >> search;
        if(search == "0"){
            return;
        }
        auto it = std::find_if(passwords.begin(), passwords.end(), [search](const Password &pass) {
            return pass.getName().find(search) != std::string::npos;
        });

        if (it != passwords.end()) {
            std::cout << "Password: " << it->getPasswd() << std::endl;
            std::cout << "Name: " << it->getName() << std::endl;
            std::cout << "Category: " << it->getCategory() << std::endl;
            std::cout << "Website: " << it->getWebsite() << std::endl;
            std::cout << "Login: " << it->getLogin() << std::endl;
            return;
        } else {
            std::cout << "No password found with " << search << " in the name. Please try again or EXIT by typing '0'" << std::endl;
        }
    }
}

/**
Usuwa wszystkie hasła z danej kategorii

Funkcja przyjmuje jako argument referencję do std::string z nazwą kategorii.
Funkcja usuwa wszystkie hasła z wektora passwords, które posiadają podaną kategorię.

@param category referencja do std::string z nazwą kategorii
@return false
*/
bool Manager::removeAll(std::string &category) {
    std::erase_if(passwords, [&](const auto &item) -> bool {
        return item.getCategory() == category;
    });
    return false;
}

/**
Edytuje dane hasła na podstawie podanego ID.
*
Funkcja pozwala na edycję nazwy hasła, samego hasła, strony internetowej,
loginu oraz kategorii hasła na podstawie podanego ID. Jeśli nie ma hasła o podanym ID,
funkcja wyświetla komunikat o błędzie.
 */
void Manager::editData() {
    std::vector<std::string> options;
    options.emplace_back("0 - Exit editing");
    options.emplace_back("1 - Edit name of password");
    options.emplace_back("2 - Edit password");
    options.emplace_back("3 - Edit website of password");
    options.emplace_back("4 - Edit login of password");
    options.emplace_back("5 - Edit category");
    if(passwords.empty()){
        std::cout << "No available passwords to edit. Returning to menu" << std::endl;
        return;
    } else {
        std::cout << "Available passwords, insert id of password" << std::endl;
        showPasswords();
        int id = 0;
        while (true) {
            std::cin >> id;
            if (id < passwords.size()) {
                Password &password = passwords[id];
                int choice;
                std::string input;
                while (true) {
                    for (auto &option: options) {
                        std::cout << option << std::endl;
                    }
                    std::cin >> choice;
                    switch (choice) {
                        case 0:
                            save();
                            return;
                        case 1:
                            std::cout << "Insert new password name" << std::endl;
                            std::cin >> input;
                            password.setName(input);
                            break;
                        case 2:
                            std::cout << "Insert new password" << std::endl;
                            while (true) {
                                std::cin >> input;
                                password.setPasswd(input);
                                if (isSafe(input)) {
                                    break;
                                }
                                else{
                                    std::cout << "This password is too weak. Insert password again" << std::endl;
                                }
                            }
                        case 3:
                            std::cout << "Insert new website" << std::endl;
                            std::cin >> input;
                            password.setWebsite(input);
                            break;
                        case 4:
                            std::cout << "Insert new login" << std::endl;
                            std::cin >> input;
                            password.setLogin(input);
                            break;
                        case 5:
                            std::cout << "Insert new category" << std::endl;
                            std::cin >> input;
                            password.setCategory(input);
                            addCategory(input);
                    }
                }
            } else {
                std::cout << "No password with that id. Try again." << std::endl;
            }
        }
    }
}
/**
Funkcja sortująca hasła według wybranego parametru

Umożliwia posortowanie listy haseł według dwóch parametrów: nazwy i kategorii.
Przed sortowaniem wyświetla listę dostępnych parametrów do sortowania.
Jeżeli nie ma dostępnych haseł do posortowania, zwraca informację o braku haseł.
*/
void Manager::sort() {
    if(passwords.empty()){
        std::cout << "No available passwords to edit. Returning to menu" << std::endl;
        return;
    }
    std::cout << "Sort by: 1. name, 2. category" << std::endl;
    std::vector<Password> copyOfPasswords = passwords;
    int choose = 0;
    std::cin >> choose;
    if(choose == 1) {
        std::sort(copyOfPasswords.begin(), copyOfPasswords.end(), [](const Password& p1, const Password& p2) -> bool {
            return p1.getName().compare(p2.getName()) < 0;
        });
    }
    else if(choose == 2) {
        std::sort(copyOfPasswords.begin(), copyOfPasswords.end(), [](const Password& p1, const Password& p2) -> bool {
            return p1.getCategory().compare(p2.getCategory()) < 0;
        });
    }
    else {
        std::cout << "Invalid input. Returning to menu." << std::endl;
        return;
    }
    for (auto & password : copyOfPasswords) {
        std::cout << password.getId() << ": " << password.getName() << std::endl;
    }
}

void Manager::showPasswords() {
    std::string data;
    for (auto & password : passwords) {
        data.append(std::to_string(password.getId()) + ',' + password.getPasswd() + ',' +
                    password.getName() + ',' + password.getCategory() + ',' + password.getWebsite() + ',' + password.getLogin() + '\n');
    }
    std::cout << data;
}

int Manager::passwordCount() {
    return passwords.size();
}

void Manager::showCategories() {
    for(auto & category : Manager::categories){
        std::cout << category << std::endl;
    }
}