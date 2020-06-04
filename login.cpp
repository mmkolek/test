#include <iostream>
#include <string>
#include <vector>

#include <cgicc/Cgicc.h>
//#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTTPRedirectHeader.h>
#include <cgicc/HTTPCookie.h>
#include <sodium.h>

#include "userdb.hpp"

using namespace std;
using namespace cgicc;
using namespace fnc;

int main( int argc, char* argv[] )
{
	try
	{
//		cout << HTTPHTMLHeader() << endl;

		Cgicc cgicc;

		if (cgicc("email").empty() || cgicc("password").empty())
		{
			cout << HTTPRedirectHeader( "index.cgi?info=dataError" ) << endl;
			return EXIT_SUCCESS;
		}

		string email = cgicc("email");
		string password = cgicc("password");
		
		IPv4 dbIP;
		uint16_t dbPort = 3306;
		string dbUser = "root";
		string dbPassword = "p";
		string dbName = "fncloud";
		users_t accounts;
		
		UserDB uDB;
		uDB.setProt(db_protocol_TCP);
		uDB.setHost(dbIP);
		uDB.setPort(dbPort);
		uDB.setUser(dbUser);
		uDB.setPassword(dbPassword);
		uDB.setDBName(dbName);
		uDB.connect();

		accounts = uDB.getUserByEmail(email);

		if (accounts.size() != 1)
		{
			cout << HTTPRedirectHeader("index.cgi?info=systemError") << endl;
			return EXIT_SUCCESS;
		}
		
		if (sodium_init() != 0)
		{
			cout << HTTPRedirectHeader("index.cgi?info=systemError") << endl;
			return EXIT_SUCCESS;
		}

		if (crypto_pwhash_str_verify(accounts[0].getPassword().c_str(),
									  password.c_str(),
									  password.size())
			)
		{
			cout << HTTPRedirectHeader("index.cgi?info=dataError") << endl;
			return EXIT_SUCCESS;
		}
		
		HTTPCookie loged( "userID", accounts[0].getGuid() );
		loged.setMaxAge( 3600 * 24 * 30 );
		cout << HTTPRedirectHeader( "dashboard.cgi" ).setCookie( loged ) << endl;
	}
	catch (sql::SQLException &e)
	{
		cerr << e.what() << endl;
	}
	catch(exception &e)
	{
		cerr << e.what() << endl;
	}

	return EXIT_SUCCESS;
}