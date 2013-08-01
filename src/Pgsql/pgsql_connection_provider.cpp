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

#include "Pgsql/precomp.h"
#include "pgsql_connection_provider.h"
#include "pgsql_command_provider.h"
#include "pgsql_reader_provider.h"
#include "pgsql_transaction_provider.h"
#include "API/Core/System/databuffer.h"
#include "API/Core/System/datetime.h"
#include "API/Core/System/uniqueptr.h"
#include "API/Core/Text/string_help.h"
#include "API/Core/Text/string_format.h"

#include <memory>

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlConnectionProvider Construction:

CL_PgsqlConnectionProvider::CL_PgsqlConnectionProvider(const Parameters &parameters)
: db(nullptr), active_transaction(nullptr)
{
	const int length = parameters.size() + 1;
	CL_UniquePtr<const char*[]> keywords(new const char*[length]);
	CL_UniquePtr<const char*[]> values(new const char*[length]);

  int i = 0;
  for (auto pair : parameters)
	{
		keywords[i] = pair.first.c_str();
		values[i] = pair.second.c_str();
		i++;
	}
	keywords[i] = nullptr;
	values[i] = nullptr;

	db = PQconnectdbParams(keywords.get(), values.get(), 0);
	if (PQstatus(db) == CONNECTION_BAD)
	{
		PQfinish(db);
		throw CL_Exception("Unable to open database");
	}
}

CL_PgsqlConnectionProvider::CL_PgsqlConnectionProvider(const CL_String &connection_string)
: db(nullptr), active_transaction(nullptr)
{
	db = PQconnectdb(connection_string.c_str());
	if (PQstatus(db) == CONNECTION_BAD)
	{
		PQfinish(db);
		throw CL_Exception("Unable to open database");
	}
}

CL_PgsqlConnectionProvider::~CL_PgsqlConnectionProvider()
{
	if (active_transaction)
		active_transaction->rollback();
	PQfinish(db);
}

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlConnectionProvider Attributes:


/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlConnectionProvider Operations:

CL_DBCommandProvider *CL_PgsqlConnectionProvider::create_command(const CL_StringRef &text, CL_DBCommand::Type type)
{
	if (type != CL_DBCommand::sql_statement)
		throw CL_Exception("DBCommand::sql_statement not yet implemented");
	else
		return new CL_PgsqlCommandProvider(this, text);
}

CL_DBTransactionProvider *CL_PgsqlConnectionProvider::begin_transaction(CL_DBTransaction::Type type)
{
	return new CL_PgsqlTransactionProvider(this, type);
}

CL_DBReaderProvider *CL_PgsqlConnectionProvider::execute_reader(CL_DBCommandProvider *command)
{
	return new CL_PgsqlReaderProvider(this, dynamic_cast<CL_PgsqlCommandProvider*>(command));
}

CL_String CL_PgsqlConnectionProvider::execute_scalar_string(CL_DBCommandProvider *command)
{
	CL_UniquePtr<CL_DBReaderProvider> reader(execute_reader(command));
	if (!reader->retrieve_row())
		throw CL_Exception("Database command statement returned no value");
	CL_String value = reader->get_column_string(0);
	return value;
}

int CL_PgsqlConnectionProvider::execute_scalar_int(CL_DBCommandProvider *command)
{
	CL_UniquePtr<CL_DBReaderProvider> reader(execute_reader(command));
	if (!reader->retrieve_row())
		throw CL_Exception("Database command statement returned no value");
	int value = reader->get_column_int(0);
	return value;
}

void CL_PgsqlConnectionProvider::execute_non_query(CL_DBCommandProvider *command)
{
	CL_UniquePtr<CL_DBReaderProvider> reader(execute_reader(command));
}

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlConnectionProvider Implementation:

CL_String CL_PgsqlConnectionProvider::to_sql_datetime(const CL_DateTime &value)
{
	return value.to_short_datetime_string();
}

CL_DateTime CL_PgsqlConnectionProvider::from_sql_datetime(const CL_String &value)
{
	return CL_DateTime::from_short_date_string(value);
}
