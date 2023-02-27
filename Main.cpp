#include <iostream>
#include "Manager.h"

bool isemptyorwhitespace(std::string str) {
    if (str.empty()) {
        return true;
    }
    return false;
}
/**
Potwierdza działanie z użytkownikiem

Ta funkcja wyświetla prompt (pytanie/prośbę o potwierdzenie) i pobiera odpowiedź od użytkownika.
 Jeśli odpowiedź jest pusta lub równa "Y" lub "y", zwraca true. W przeciwnym razie zwraca false.
@param {const std::string&} prompt - Pytanie/prośba o potwierdzenie, które ma zostać wyświetlone
@return {bool} - Zwraca true, jeśli użytkownik potwierdził działanie, false w przeciwnym razie
*/
bool confirm(const std::string& prompt) {
    std::cout << prompt << std::endl;
    std::string confirm;
    std::cin >> confirm;
    if (confirm.empty() || confirm == "Y" || confirm == "y") {
        return true;
    }
    return false;
}

/**

Generuje nowe hasło

Ta funkcja pyta użytkownika o liczbę znaków, jakie powinno mieć hasło (minimum 4), i czy powinno zawierać symbole i/lub litery.
Funkcja następnie korzysta z metody genPasswd managera, aby wygenerować nowe hasło z określonymi parametrami.

@param {Manager*} manager - Wskaźnik do obiektu managera, który wygeneruje hasło
@return {std::string} - Wygenerowane hasło
*/
std::string genPassword(Manager* manager) {
    int count = 0;
    bool letters = false;
    bool symbols = false;
    std::cout << "Enter number of characters: (min 4)" << std::endl;
    while (true) {
        std::cin >> count;
        if(count<4){
            std::cout << "Wrong number of characters, password is too short to be safe. Insert again" << std::endl;
        } else{
            break;
        }
    }
    if (confirm("Do you want symbols to be included [Y/N]")) {
        symbols = true;
    }
    if (confirm("Do you want letters in this password [Y/N]")) {
        letters = true;
    }
    return manager->genPasswd(count, letters, symbols);
}
/**

Dodaje nowe hasło do listy haseł managera

Ta funkcja pyta użytkownika o hasło, nazwę i kategorię nowego hasła. Pozwala również użytkownikowi wybrać opcję automatycznego generowania hasła,
 a także sprawdza, czy hasło jest bezpieczne i czy nie jest powtórzone. Funkcja pozwala również na dodanie strony internetowej i loginu do tego hasła.
 Jeśli zostanie wybrana nowa kategoria, zostanie ona dodana do listy kategorii.

@param {Manager*} manager - Wskaźnik do obiektu managera, do którego zostanie dodane hasło

@return {bool} - Zwraca true, jeśli hasło zostało dodane pomyślnie, false w przeciwnym razie
*/

bool addpasswd(Manager* manager) {
    std::string passwd;
    while (true) {
        if (confirm("Do you want to auto generate a password? [Y/N]")) {
            passwd = genPassword(manager);
        } else {
            std::cout << "Enter a new password:" << std::endl;
            std::cin >> passwd;
        }

        if (!manager->isSafe(passwd) &&
            confirm("This password is weak. Enter a password again? [Y/n]")) {
            continue;
        }

        if (manager->isDuplicate(passwd) &&
            confirm("This password is already used. Enter a password again? [Y/n]")) {
            continue;
        }
        break;
    }

    std::string name;
    while (true) {
        std::cout << "Enter password name:" << std::endl;
        std::cin >> name;
        if (isemptyorwhitespace(name)) {
            std::cout << "Name cannot be empty.";
            continue;
        }
        break;
    }

    std::string category;
    std::string choice;
    while (true) {
        if (confirm("Would you like to add new category? [Y/N]")) {
        std::cout << "Enter password category:" << std::endl;
        std::cin >> category;
        if (isemptyorwhitespace(category)) {
            std::cout << "Category cannot be empty.";
            continue;
        } else {
            manager->addCategory(category);
        }
    } else{
            while (true) {
                std::cout << "Choose category from a list" << std::endl;
                manager->showCategories();
                std::cin >> category;
                if (manager->isCategoryAvailable(category)) {
                    break;
                } else{
                    std::cout << "No category on list. Try again" << std::endl;
                }
            }
        }
        break;
    }
    std::string website;
    std::string login;
    if(confirm("Would you like to add website to this password? [Y/N]")) {
        std::cout << "Insert website." << std::endl;
        std::cin >> website;
    }
    if(confirm("Would you like to add login to this password? [Y/N]")) {
        std::cout << "Insert login." << std::endl;
        std::cin >> login;
    }
    Password password = Password(passwd, name, category, website, login);
    return manager->addPassword(password);
}
/**
Dodaje kategorię do managera

Funkcja pyta użytkownika o nazwę kategorii, a następnie wykorzystuje metodę addCategory managera, aby ją dodać.

@param manager Wskaźnik na obiekt managera, do którego ma być dodana kategoria
@return Wartość logiczna określająca, czy dodanie kategorii się powiodło
*/
bool addCategory(Manager* manager) {
    std::string category;
    std::cout << "Enter category name: " << std::endl;
    std::cin >> category;
    return manager->addCategory(category);
}
/**
Usuwa kategorię z managera

Funkcja wyświetla dostępne kategorie, a następnie pyta użytkownika o nazwę kategorii do usunięcia.
 Jeśli użytkownik poda nazwę istniejącej kategorii, zostanie ona usunięta za pomocą metody removeCategory managera.

@param manager Wskaźnik na obiekt managera, z którego ma być usunięta kategoria
@return Wartość logiczna określająca, czy usunięcie kategorii się powiodło
*/
bool removeCategory(Manager* manager) {
    std::string category;
    std::cout << "Available categories: " << std::endl;
    manager->showCategories();
    std::cout << "Enter category name to remove, or type ANYTHING ELSE to exit this function. " << std::endl;
    std::cin >> category;
    return manager->removeCategory(category);
}

/**
Wybiera plik z hasłami

Funkcja pyta użytkownika o ścieżkę do pliku z hasłami i zwraca ją jako std::string.

@return std::string zawierający ścieżkę do pliku z hasłami
*/
std::string chooseFile() {
    std::cout << "Enter path to passwords file: " << std::endl;
    std::string path;
    std::cin >> path;
    return path;
}

/**
Wprowadza hasło pliku

Funkcja pyta użytkownika o hasło pliku i zwraca je jako std::string.

@return std::string zawierający hasło pliku
*/
std::string enterFilePassword() {
    std::cout << "Enter file password: " << std::endl;
    std::string password;
    std::cin >> password;
    return password;
}
/**
W funkcji znajduje się pętla główna programu oraz inicjalizacja obiektu manager.
Program pobiera od użytkownika ścieżkę do pliku i hasło do pliku, a następnie inicjalizuje obiekt manager.
W pętli głównej programu użytkownik ma możliwość wyboru opcji z menu, które pozwala na wyszukanie, sortowanie, dodawanie, edycję,
 usuwanie haseł oraz dodawanie i usuwanie kategorii.
Jeśli katalog haseł jest pusty, zostaną dodane domyślne hasła.
*/

int main() {
    Manager* manager;

    for (int i = 0; i < 3; i++) {
        manager = new Manager();
        std::string path = chooseFile();
        std::string filePassword = enterFilePassword();
        if (manager->init(path, filePassword)) {
            break;
        }
        if (i == 2) {
            return 1;
        }
    }

    std::vector <std::string> options;
    options.emplace_back("1 - Search password");
    options.emplace_back("2 - Sort passwords");
    options.emplace_back("3 - Add password");
    options.emplace_back("4 - Edit password");
    options.emplace_back("5 - Delete password");
    options.emplace_back("6 - Add category");
    options.emplace_back("7 - Delete category");
    options.emplace_back("Anything else - Exit program");

    if (manager->passwordCount() == 0) {
        manager->addPassword(Password("lol12312", "Google account password", "Easy", "google.com", "pawel09@gmail.com"));
        manager->addPassword(Password("password123", "Fortnite account password", "Easy", "epicgames.com", "marcinek321"));
        manager->addPassword(Password("klc75Xcd", "Steam account password", "Medium", "Steamcommunity.com", "ginold3"));
        manager->addPassword(Password("C456,98KlMn$", "Bank account password", "Hard", "PKOBP.pl", "688112334"));
        manager->addPassword(Password("Kdasg56x.,%5", "Bank account password", "Hard", "PKOBP.pl", "623511661"));
    }

    while (true) {
        std::cout << "Welcome to password manager, choose option that you want to do" << std::endl;
        for(auto & option : options){
            std::cout << option << std::endl;
        }
        int choice;
        std::cin >> choice;
        std::string pass;
        switch (choice)
        {
            case 0:
                std::cout << "Exit program" << std::endl;
                exit(0);
            case 1: //search
                manager->searchPass();
                break;
            case 2: //sort
                manager->sort();
                break;
            case 3: //addpassword
                addpasswd(manager);
                break;
            case 4: //edit password
                manager->editData();
                break;
            case 5: //delete password
                manager->removePassword();
                break;
            case 6: //addPassword category
                addCategory(manager);
                break;
            case 7: //delete category
                removeCategory(manager);
                break;
        }
    }
}




