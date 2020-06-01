#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>

using namespace std;

class PasswordFile
{
 public:
 PasswordFile(string filename);// opens the file and reads the names/passwords in the vectors user and password.
 void addpw(string newuser, string newpassword); //this adds a new user/password to the vectors and writes the vectors to the file filename
 bool checkpw(string usr, string passwd); // returns true if user exists and password matches

 private:
 string filename; // the file that contains password information
 vector<string> user; // the list of usernames
 vector<string> password; // the list of passwords
 void synch(); // writes the user/password vectors to the password file
};

PasswordFile::PasswordFile( string filename ) {

        string users_str, passwords_str;

        ifstream readfile;
        readfile.open( filename.c_str() );
        readfile >> users_str >> passwords_str;

        while( readfile.good() ) {
                user.push_back(users_str);
                password.push_back(passwords_str);
                readfile >> users_str >> passwords_str;
        }

        readfile.close();
}

void PasswordFile::addpw( string newuser, string newpassword ) {

        bool chk_user_exist = false; 

        int i = 0;

        while ( i < user.size() )
        {
                if ( user[i] == newuser )
                {
                        chk_user_exist = true;
                        cout << newuser << " ID already exists" << endl;
                        i++;
                }
                else 
                {
                        i++;
                }
        }

        if ( chk_user_exist == false )
        {
                user.push_back(newuser);
                password.push_back(newpassword);
        }

        synch();

}


bool PasswordFile::checkpw( string usr, string passwd ) {

        for ( int i = 0; i < user.size(); i++ ) {
                if ( user[i] == usr) {
                        if ( password[i] == passwd ) {
                                return true;
                        }
                }
        }
        return false;
}

void PasswordFile::synch() {

        ofstream writefile;
        writefile.open(filename);

        for ( int i = 0; i < user.size(); i++ ) {
                writefile << user[i] << " " << password[i] << endl;
        }

        writefile.close();
}

int main() {

        PasswordFile passfile("test.txt");
        
        passfile.addpw("dbotting","123qwe");
        passfile.addpw("egomez","qwerty");
        passfile.addpw("tongyu","liberty");

        // write some lines to see if passwords match users
        bool test = passfile.checkpw("dbotting","123qwe");

        if (test)
        {
                cout << "match" << endl;
        }
        else{
                cout << "do not match" << endl;
        }
        
}