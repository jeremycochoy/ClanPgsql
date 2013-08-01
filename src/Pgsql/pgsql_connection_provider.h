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
#include "API/Pgsql/pgsql_connection.h"
#include "API/Database/db_connection_provider.h"

class CL_PgsqlTransactionProvider;
class CL_PgsqlReaderProvider;

/// \brief Sqlite database connection provider
class CL_PgsqlConnectionProvider : public CL_DBConnectionProvider
{
/// \name Construction
/// \{
public:
	typedef CL_PgsqlConnection::Parameters Parameters;

	CL_PgsqlConnectionProvider(const Parameters &parameters);
	CL_PgsqlConnectionProvider(const CL_String &connecton_tring);
	~CL_PgsqlConnectionProvider();
/// \}

/// \name Attributes
/// \{
public:
/// \}

/// \name Operations
/// \{
public:
	CL_DBCommandProvider *create_command(const CL_StringRef &text, CL_DBCommand::Type type);
	CL_DBTransactionProvider *begin_transaction(CL_DBTransaction::Type type);
	CL_DBReaderProvider *execute_reader(CL_DBCommandProvider *command);
	CL_String execute_scalar_string(CL_DBCommandProvider *command);
	int execute_scalar_int(CL_DBCommandProvider *command);
	void execute_non_query(CL_DBCommandProvider *command);
/// \}

/// \name Implementation
/// \{
private:
	static CL_String to_sql_datetime(const CL_DateTime &value);
	static CL_DateTime from_sql_datetime(const CL_String &value);

	PGconn *db;
	CL_PgsqlTransactionProvider *active_transaction;

	friend class CL_PgsqlReaderProvider;
	friend class CL_PgsqlTransactionProvider;
	friend class CL_PgsqlCommandProvider;
/// \}
};


/// \}
