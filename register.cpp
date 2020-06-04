#include <iostream>
#include <string>
#include <vector>

#include <cgicc/Cgicc.h>
#include <cgicc/HTTPRedirectHeader.h>
#include <sodium.h>
#include <crossguid/guid.hpp>

#include "userdb.hpp"
#include "fs.hpp"

using namespace std;
using namespace cgicc;
using namespace fnc;
using namespace xg;

int main( int argc, char *argv[] )
{
	try
	{
		Cgicc cgicc;

		if ( cgicc( "email" ).empty() || cgicc( "password" ).empty() )
		{
			cout << HTTPRedirectHeader( "index.cgi?info=dataError" ) << endl;
			return EXIT_SUCCESS;
		}

		if (sodium_init() != 0)
		{
			cout << HTTPRedirectHeader("index.cgi?info=systemError") << endl;
			return EXIT_FAILURE;
		}

		string email = cgicc("email");
		string password = cgicc("password");

		const auto opsLimit = crypto_pwhash_OPSLIMIT_MODERATE;
		const int memLimit = 256 * 1024 * 1024;
		char out[crypto_pwhash_STRBYTES];

		if (crypto_pwhash_str(out, password.c_str(), password.size(), opsLimit, memLimit) != 0)
		{
			cout << HTTPRedirectHeader("index.cgi?info=systemError") << endl;
			return EXIT_FAILURE;
		}

		string hashed = out;
		
		IPv4 dbIP;
		uint16_t dbPort = 3306;
		string dbUser = "root";
		string dbPassword = "p";
		string dbName = "fncloud";

		UserDB uDB;
		uDB.setProt(db_protocol_TCP);
		uDB.setHost(dbIP);
		uDB.setPort(dbPort);
		uDB.setUser(dbUser);
		uDB.setPassword(dbPassword);
		uDB.setDBName(dbName);
		uDB.connect();

		users_t accounts = uDB.getUserByEmail(email);
		if (accounts.size() != 0)
		{
			cout << HTTPRedirectHeader("index.cgi?info=systemError") << endl;
			return EXIT_SUCCESS;
		}

		string guid = newGuid();

		uDB.setUserByEmail(guid, email, hashed);

		FS fs("C:\\xampp\\htdocs\\fncloud\\users");
		fs.newDir(guid);

		cout << HTTPRedirectHeader("index.cgi?info=registerSuccess") << endl;
	}
	catch (sql::SQLException& e)
	{
		cerr << e.what() << endl;
	}
	catch( exception &e)
	{
		cerr << e.what() << endl;
	}

	return EXIT_SUCCESS;
}
