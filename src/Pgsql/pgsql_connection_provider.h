/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Jeremy Cochoy
*/

/// \addtogroup clanPgsql_System clanPgsql System
/// \{


#pragma once


#include <libpq-fe.h>
#include "ClanLib/Pgsql/pgsql_connection.h"
#include "ClanLib/Database/db_connection_provider.h"

namespace clan
{

class PgsqlTransactionProvider;
class PgsqlReaderProvider;

/// \brief Sqlite database connection provider
class PgsqlConnectionProvider : public DBConnectionProvider
{
/// \name Construction
/// \{
public:
	typedef PgsqlConnection::Parameters Parameters;

	PgsqlConnectionProvider(const Parameters &parameters);
	PgsqlConnectionProvider(const std::string &connecton_tring);
	~PgsqlConnectionProvider();
/// \}

/// \name Attributes
/// \{
public:
/// \}

/// \name Operations
/// \{
public:
	DBCommandProvider *create_command(const std::string &text, DBCommand::Type type);
	DBTransactionProvider *begin_transaction(DBTransaction::Type type);
	DBReaderProvider *execute_reader(DBCommandProvider *command);
	std::string execute_scalar_string(DBCommandProvider *command);
	int execute_scalar_int(DBCommandProvider *command);
	void execute_non_query(DBCommandProvider *command);
/// \}

/// \name Implementation
/// \{
private:
	static std::string to_sql_datetime(const DateTime &value);
	static DateTime from_sql_datetime(const std::string &value);

	PGconn *db;
	PgsqlTransactionProvider *active_transaction;

	friend class PgsqlReaderProvider;
	friend class PgsqlTransactionProvider;
	friend class PgsqlCommandProvider;
/// \}
};

}; // namespace clan

/// \}
