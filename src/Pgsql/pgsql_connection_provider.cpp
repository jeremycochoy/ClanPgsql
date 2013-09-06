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

#include <memory>

#include "Pgsql/precomp.h"
#include "pgsql_connection_provider.h"
#include "pgsql_command_provider.h"
#include "pgsql_reader_provider.h"
#include "pgsql_transaction_provider.h"
#include "ClanLib/Core/System/databuffer.h"
#include "ClanLib/Core/System/datetime.h"
#include "ClanLib/Core/Text/string_help.h"
#include "ClanLib/Core/Text/string_format.h"

namespace clan
{

/////////////////////////////////////////////////////////////////////////////
// PgsqlConnectionProvider Construction:

PgsqlConnectionProvider::PgsqlConnectionProvider(const Parameters &parameters)
: db(nullptr), active_transaction(nullptr)
{
	const int length = parameters.size() + 1;
	std::unique_ptr<const char*[]> keywords(new const char*[length]);
	std::unique_ptr<const char*[]> values(new const char*[length]);

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
		throw Exception("Unable to open database");
	}
}

PgsqlConnectionProvider::PgsqlConnectionProvider(const std::string &connection_string)
: db(nullptr), active_transaction(nullptr)
{
	db = PQconnectdb(connection_string.c_str());
	if (PQstatus(db) == CONNECTION_BAD)
	{
		PQfinish(db);
		throw Exception("Unable to open database");
	}
}

PgsqlConnectionProvider::~PgsqlConnectionProvider()
{
	if (active_transaction)
		active_transaction->rollback();
	PQfinish(db);
}

/////////////////////////////////////////////////////////////////////////////
// PgsqlConnectionProvider Attributes:


/////////////////////////////////////////////////////////////////////////////
// PgsqlConnectionProvider Operations:

DBCommandProvider *PgsqlConnectionProvider::create_command(const std::string &text, DBCommand::Type type)
{
	if (type != DBCommand::sql_statement)
		throw Exception("DBCommand::sql_statement not yet implemented");
	else
		return new PgsqlCommandProvider(this, text);
}

DBTransactionProvider *PgsqlConnectionProvider::begin_transaction(DBTransaction::Type type)
{
	return new PgsqlTransactionProvider(this, type);
}

DBReaderProvider *PgsqlConnectionProvider::execute_reader(DBCommandProvider *command)
{
	return new PgsqlReaderProvider(this, dynamic_cast<PgsqlCommandProvider*>(command));
}

std::string PgsqlConnectionProvider::execute_scalar_string(DBCommandProvider *command)
{
	std::unique_ptr<DBReaderProvider> reader(execute_reader(command));
	if (!reader->retrieve_row())
		throw Exception("Database command statement returned no value");
	std::string value = reader->get_column_string(0);
	return value;
}

int PgsqlConnectionProvider::execute_scalar_int(DBCommandProvider *command)
{
	std::unique_ptr<DBReaderProvider> reader(execute_reader(command));
	if (!reader->retrieve_row())
		throw Exception("Database command statement returned no value");
	int value = reader->get_column_int(0);
	return value;
}

void PgsqlConnectionProvider::execute_non_query(DBCommandProvider *command)
{
	std::unique_ptr<DBReaderProvider> reader(execute_reader(command));
}

/////////////////////////////////////////////////////////////////////////////
// PgsqlConnectionProvider Implementation:

std::string PgsqlConnectionProvider::to_sql_datetime(const DateTime &value)
{
	return value.to_short_datetime_string();
}

DateTime PgsqlConnectionProvider::from_sql_datetime(const std::string &value)
{
	return DateTime::from_short_date_string(value);
}

}; // namespace clan
