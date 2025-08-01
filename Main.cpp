#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <map>
#include <cstdio>  // For sprintf, sscanf
#include <algorithm>
#include <limits>

using namespace std;

// ======= Utility Functions and Date Struct =======
struct Date {
    int day, month, year;
    Date() { day = 1; month = 1; year = 2000; }
    Date(int d, int m, int y) { day = d; month = m; year = y; }

    static Date today() {
        time_t t = time(0);
        tm *lt = localtime(&t);
        return Date(lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
    }

    string str() const {
        char buf[11];
        sprintf(buf, "%02d-%02d-%04d", day, month, year);
        return string(buf);
    }

    static Date fromString(const string& s) {
        int d=1,m=1,y=2000;
        sscanf(s.c_str(), "%d-%d-%d", &d, &m, &y);
        return Date(d, m, y);
    }

    int daysSince(const Date& other) const {
        return (year - other.year) * 365 + (month - other.month) * 30 + (day - other.day);
    }
};

void pressEnter() {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ======= Book Class =======
class Book {
public:
    int id;
    string title, author, category;
    bool isIssued;
    string issuedTo;
    Date issuedDate;

    Book() { id=0; isIssued=false; issuedTo=""; }

    void input() {
        cout << "Enter Book ID: ";
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Title: ";
        getline(cin, title);
        cout << "Enter Author: ";
        getline(cin, author);
        cout << "Enter Category: ";
        getline(cin, category);
        isIssued = false;
        issuedTo = "";
    }

    void displayShort() const {
        cout << setw(3) << id << setw(20) << (title.length() > 17 ? title.substr(0,17) + "..." : title)
             << setw(15) << (author.length() > 13 ? author.substr(0,13) + "..." : author)
             << setw(14) << (category.length() > 12 ? category.substr(0,12) + "..." : category)
             << setw(8) << (isIssued ? "Yes" : "No")
             << setw(15) << (isIssued ? issuedTo : "") << endl;
    }

    void displayDetail() const {
        cout << "ID: " << id << "\nTitle: " << title
             << "\nAuthor: " << author << "\nCategory: " << category
             << "\nIssued: " << (isIssued ? "Yes" : "No") << "\n";
        if(isIssued) {
            cout << "Issued To: " << issuedTo << "\nDate: " << issuedDate.str() << endl;
        }
    }
};

// ======= User Class =======
class User {
public:
    string username, password, name, type;
    vector<int> borrowedBooks;

    User() {}

    void input() {
        cout << "Enter Username: ";
        cin >> username;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Name: ";
        getline(cin, name);
        cout << "Enter Password: ";
        cin >> password;
        cout << "Enter Role (admin/student): ";
        cin >> type;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    void display() const {
        cout << "Username: " << username << "\nName: " << name << "\nType: " << type << endl;
        cout << "Borrowed Book IDs: ";
        for(size_t i=0;i<borrowedBooks.size();++i) cout << borrowedBooks[i] << " ";
        cout << "\n";
    }
};

// ======= LibraryDatabase: Handles Data Persistence =======
class LibraryDatabase {
public:
    vector<Book> books;
    vector<User> users;
    map<int, vector< pair<string, Date> > > history;

    void loadBooks() {
        books.clear();
        ifstream f("books.dat");
        if(!f) return;

        int n; f >> n; f.ignore();
        for(int i=0; i<n; i++) {
            Book b;
            f >> b.id; f.ignore();
            getline(f, b.title);
            getline(f, b.author);
            getline(f, b.category);
            f >> b.isIssued; f.ignore();
            getline(f, b.issuedTo);
            string dstr; getline(f, dstr);
            if(dstr != "N/A") b.issuedDate = Date::fromString(dstr);
            else b.issuedDate = Date();
            books.push_back(b);
        }
        f.close();
    }

    void saveBooks() {
        ofstream f("books.dat");
        f << books.size() << "\n";
        for(size_t i=0;i<books.size();++i) {
            Book &b=books[i];
            f << b.id << "\n" << b.title << "\n" << b.author << "\n" << b.category << "\n"
              << b.isIssued << "\n" << b.issuedTo << "\n"
              << (b.isIssued ? b.issuedDate.str() : "N/A") << "\n";
        }
        f.close();
    }

    void loadUsers() {
        users.clear();
        ifstream f("users.dat");
        if(!f) return;

        int n; f >> n; f.ignore();
        for(int i=0; i<n; i++) {
            User u;
            getline(f, u.username);
            getline(f, u.name);
            getline(f, u.password);
            getline(f, u.type);
            int k; f >> k; f.ignore();
            u.borrowedBooks.clear();
            for(int j=0; j<k; j++) {
                int id; f >> id;
                u.borrowedBooks.push_back(id);
            }
            f.ignore(numeric_limits<streamsize>::max(), '\n');
            users.push_back(u);
        }
        f.close();
    }

    void saveUsers() {
        ofstream f("users.dat");
        f << users.size() << "\n";
        for(size_t i=0;i<users.size();++i) {
            User &u=users[i];
            f << u.username << "\n" << u.name << "\n" << u.password << "\n" << u.type << "\n"
              << u.borrowedBooks.size();
            for(size_t j=0;j<u.borrowedBooks.size();++j) f << " " << u.borrowedBooks[j];
            f << "\n";
        }
        f.close();
    }

    void loadHistory() {
        history.clear();
        ifstream f("history.dat");
        if(!f) return;

        int n; f >> n; f.ignore();
        for(int k=0; k<n; k++) {
            int bid, x; f >> bid >> x; f.ignore();
            vector< pair<string, Date> > h;
            for(int j=0; j<x; j++) {
                string u, d;
                getline(f, u);
                getline(f, d);
                h.push_back(make_pair(u, Date::fromString(d)));
            }
            history[bid] = h;
        }
        f.close();
    }

    void saveHistory() {
        ofstream f("history.dat");
        f << history.size() << "\n";
        for(map<int, vector<pair<string, Date> > >::iterator it=history.begin(); it!=history.end(); ++it) {
            f << it->first << " " << it->second.size() << "\n";
            for(size_t i=0;i<it->second.size();++i) {
                f << it->second[i].first << "\n" << it->second[i].second.str() << "\n";
            }
        }
        f.close();
    }

    void ensureAdminExists() {
        loadUsers();
        bool found = false;
        for(size_t i=0;i<users.size();++i)
            if(users[i].type == "admin") { found = true; break; }
        if(!found) {
            User a;
            a.username = "admin";
            a.name = "Administrator";
            a.password = "admin";
            a.type = "admin";
            users.push_back(a);
            saveUsers();
        }
    }
};

// ======= Library Logic / Controller =======
class Library {
    LibraryDatabase db;
    User currentUser;

    int searchBookById(int id) {
        for(size_t i=0; i<db.books.size(); ++i)
            if(db.books[i].id == id) return (int)i;
        return -1;
    }

    int searchUser(const string& uname) {
        for(size_t i=0; i<db.users.size(); ++i)
            if(db.users[i].username == uname) return (int)i;
        return -1;
    }

    void listAllBooks() {
        cout << setw(3) << "ID" << setw(20) << "Title" << setw(15) << "Author" << setw(14) << "Category"
             << setw(8) << "Issued" << setw(15) << "IssuedTo" << "\n";
        for(size_t i=0;i<db.books.size();++i) db.books[i].displayShort();
    }

    void viewBookDetail() {
        cout << "Enter Book ID: ";
        int id; cin >> id;
        int idx = searchBookById(id);
        if(idx == -1) { cout << "Book not found!\n"; return; }
        db.books[idx].displayDetail();

        if(db.history.count(id)) {
            cout << "Borrowing History:\n";
            for(size_t i=0;i<db.history[id].size();++i)
                cout << "  " << db.history[id][i].first << " on " << db.history[id][i].second.str() << "\n";
        }
    }

    void addBook() {
        Book b; b.input();
        if(searchBookById(b.id) != -1) {
            cout << "Book ID already exists!\n";
            return;
        }
        db.books.push_back(b);
        db.saveBooks();
        cout << "Book added successfully.\n";
    }

    void removeBook() {
        cout << "Enter Book ID to remove: ";
        int id; cin >> id;
        int idx = searchBookById(id);
        if(idx == -1) { cout << "Book not found.\n"; return; }
        db.books.erase(db.books.begin() + idx);
        db.saveBooks();
        cout << "Book removed successfully.\n";
    }

    void issueBook() {
        cout << "Enter Book ID: ";
        int id; cin >> id;
        int bidx = searchBookById(id);
        int uidx = searchUser(currentUser.username);
        if(bidx == -1 || uidx == -1) { cout << "Book or user not found!\n"; return;}
        if(db.books[bidx].isIssued) {
            cout << "Book is already issued!\n";
            return;
        }
        if(currentUser.type == "student" && db.users[uidx].borrowedBooks.size() >= 3) {
            cout << "Students may borrow at most 3 books.\n";
            return;
        }
        db.books[bidx].isIssued = true;
        db.books[bidx].issuedTo = currentUser.username;
        db.books[bidx].issuedDate = Date::today();
        db.users[uidx].borrowedBooks.push_back(id);
        db.history[id].push_back(make_pair(currentUser.username, Date::today()));

        db.saveBooks();
        db.saveUsers();
        db.saveHistory();

        cout << "Book issued successfully to " << currentUser.username << ".\n";
    }

    void returnBook() {
        cout << "Enter Book ID: ";
        int id; cin >> id;
        int bidx = searchBookById(id);
        int uidx = searchUser(currentUser.username);
        if(bidx == -1 || uidx == -1) { cout << "Book or user not found!\n"; return;}
        if(!db.books[bidx].isIssued || db.books[bidx].issuedTo != currentUser.username) {
            cout << "This book is not issued to you!\n";
            return;
        }
        Date today = Date::today();
        int daysBorrowed = today.daysSince(db.books[bidx].issuedDate);
        int fine = (daysBorrowed > 15) ? (daysBorrowed - 15)*2 : 0;

        db.books[bidx].isIssued = false;
        db.books[bidx].issuedTo = "";
        db.books[bidx].issuedDate = Date();

        vector<int>& bb = db.users[uidx].borrowedBooks;
        bb.erase(remove(bb.begin(), bb.end(), id), bb.end());

        db.saveBooks();
        db.saveUsers();

        cout << "Book returned successfully. ";
        if(fine > 0)
            cout << "Note: Overdue fine is Rs." << fine << '\n';
        else
            cout << "Thank you for returning on time.\n";
    }

    void viewBorrowedBooks() {
        int uidx = searchUser(currentUser.username);
        if(uidx == -1) {
            cout << "User not found.\n";
            return;
        }
        cout << setw(3) << "ID" << setw(20) << "Title" << setw(12) << "Issued Date"
             << setw(8) << "Overdue" << setw(6) << "Fine\n";
        for(size_t i=0;i<db.users[uidx].borrowedBooks.size();++i) {
            int id = db.users[uidx].borrowedBooks[i];
            int bidx = searchBookById(id);
            if(bidx == -1) continue;
            const Book &b = db.books[bidx];
            int daysBorrowed = Date::today().daysSince(b.issuedDate);
            bool overdue = (daysBorrowed > 15);
            int fine = overdue ? (daysBorrowed - 15) * 2 : 0;

            cout << setw(3) << id << setw(20) << (b.title.length() > 17 ? b.title.substr(0,17) + "..." : b.title)
                 << setw(12) << b.issuedDate.str()
                 << setw(8) << (overdue ? "Yes" : "No")
                 << setw(6) << fine << "\n";
        }
    }

    void listAllUsers() {
        cout << setw(15) << "Username" << setw(15) << "Name" << setw(10) << "Role" << "  Borrowed Books\n";
        for(size_t i=0;i<db.users.size();++i) {
            User &u = db.users[i];
            cout << setw(15) << u.username << setw(15) << u.name << setw(10) << u.type << "  ";
            for(size_t j=0;j<u.borrowedBooks.size();++j) cout << u.borrowedBooks[j] << " ";
            cout << "\n";
        }
    }

    void addUser() {
        User u;
        u.input();
        if(searchUser(u.username) != -1) {
            cout << "Username already exists!\n";
            return;
        }
        db.users.push_back(u);
        db.saveUsers();
        cout << "User registered successfully.\n";
    }

    void removeUser() {
        cout << "Enter username to remove: ";
        string uname; cin >> uname;
        int idx = searchUser(uname);
        if(idx == -1) {
            cout << "User not found.\n";
            return;
        }
        db.users.erase(db.users.begin() + idx);
        db.saveUsers();
        cout << "User removed successfully.\n";
    }

    void logout() {
        cout << "Logged out successfully.\n";
        currentUser = User();
    }

    void studentMenu() {
        while(true) {
            cout << "\nStudent Menu:\n"
                 << "1: List Books\n2: View Book Detail\n3: Borrow Book\n4: Return Book\n"
                 << "5: My Borrowed Books\n0: Logout\nChoice: ";
            int c; cin >> c;
            switch(c) {
                case 1: listAllBooks(); break;
                case 2: viewBookDetail(); break;
                case 3: issueBook(); break;
                case 4: returnBook(); break;
                case 5: viewBorrowedBooks(); break;
                case 0: logout(); return;
                default: cout << "Invalid choice, try again.\n";
            }
            pressEnter();
        }
    }

    void adminMenu() {
        while(true) {
            cout << "\nAdmin Menu:\n"
                 << "1: List All Books\n2: Add Book\n3: Remove Book\n"
                 << "4: View Book Detail\n5: Add User\n6: Remove User\n7: List All Users\n0: Logout\nChoice: ";
            int c; cin >> c;
            switch(c) {
                case 1: listAllBooks(); break;
                case 2: addBook(); break;
                case 3: removeBook(); break;
                case 4: viewBookDetail(); break;
                case 5: addUser(); break;
                case 6: removeUser(); break;
                case 7: listAllUsers(); break;
                case 0: logout(); return;
                default: cout << "Invalid choice, try again.\n";
            }
            pressEnter();
        }
    }

public:
    void run() {
        db.ensureAdminExists();
        db.loadBooks();
        db.loadUsers();
        db.loadHistory();

        cout << "--- Library Management System ---\n";

        while(true) {
            cout << "\n1: Login\n2: Register (Student)\n0: Exit\nChoice: ";
            int c; cin >> c;
            if(c == 0) break;
            if(c == 2) {
                addUser();
                continue;
            }

            // ===== Login Section =====
            string uname, pwd;
            cout << "Username: "; cin >> uname;
            cout << "Password: "; cin >> pwd;

            int idx = -1;
            for(size_t i=0;i<db.users.size();++i) {
                if(db.users[i].username == uname && db.users[i].password == pwd) {
                    idx = (int)i;
                    break;
                }
            }

            if(idx == -1) {
                cout << "Login failed: Invalid username or password.\n";
                pressEnter();
                continue;
            }

            currentUser = db.users[idx];
            cout << "Welcome, " << currentUser.name << "! (" << currentUser.type << ")\n";

            if(currentUser.type == "admin") adminMenu();
            else studentMenu();
        }
    }
};

// ======= main function =======
int main() {
    Library lib;
    lib.run();
    return 0;
} // <-- THIS must exist as last line

