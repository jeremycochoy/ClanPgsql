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
#include "pgsql_reader_provider.h"
#include "pgsql_connection_provider.h"
#include "pgsql_command_provider.h"
#include "ClanLib/Core/System/databuffer.h"
#include "ClanLib/Core/System/datetime.h"
#include "ClanLib/Core/Text/string_help.h"
#include "ClanLib/Core/Text/string_format.h"
#include <libpq-fe.h>
#include <cstdlib>

namespace clan
{
/////////////////////////////////////////////////////////////////////////////
// PgsqlReaderProvider Construction:

PgsqlReaderProvider::PgsqlReaderProvider(PgsqlConnectionProvider *connection, PgsqlCommandProvider *command)
    : connection(connection), command(command), closed(false), current_row(-1)
{
	auto deleter = [](PGresult *ptr) {if (ptr) {PQclear(ptr);} };
	std::unique_ptr<PGresult, decltype(deleter)> result_uniqueptr(command->exec_command(), deleter);
	result = result_uniqueptr.get();

	switch (PQresultStatus(result))
	{
	case PGRES_EMPTY_QUERY:
		throw Exception("Empty query");

	case PGRES_COMMAND_OK:
		type = ResultType::EMPTY_RESULT;
		break;
	case PGRES_TUPLES_OK:
		type = ResultType::TUPLES_RESULT;
		break;

	case PGRES_NONFATAL_ERROR:
		throw Exception("Server gave an unknow answer");

	case PGRES_FATAL_ERROR:
		throw Exception(PQerrorMessage(connection->db));

	default:
		throw Exception(StringHelp::text_to_local8(PQresultErrorMessage(result)));
	}
	nb_rows = PQntuples(result);
	result_uniqueptr.release();
}

PgsqlReaderProvider::~PgsqlReaderProvider()
{
	close();
}

/////////////////////////////////////////////////////////////////////////////
// PgsqlReaderProvider Attributes:

int PgsqlReaderProvider::get_column_count() const
{
	return  PQnfields(result);
}

std::string PgsqlReaderProvider::get_column_name(int index) const
{
	const char *const string = PQfname(result, index);
	if (string == nullptr)
		throw ("Index out of range");
	return std::string(string);
}

int PgsqlReaderProvider::get_name_index(const std::string &name) const
{
	int index = PQfnumber(result, name.c_str());
	if (index < 0)
		throw Exception(string_format("No such column name %1", name));
	return index;
}

std::string PgsqlReaderProvider::get_column_string(int index) const
{
	const char *const str = PQgetvalue(result, current_row, index);
	if (str != nullptr)
		return str;
	else
		throw ("Index out of range");
}

bool PgsqlReaderProvider::get_column_bool(int index) const
{
	const std::string value = get_column_string(index);
	return StringHelp::text_to_bool(value);
}

char PgsqlReaderProvider::get_column_char(int index) const
{
	const std::string value = get_column_string(index);
	return static_cast<int>(StringHelp::text_to_int(value));
}

unsigned char PgsqlReaderProvider::get_column_uchar(int index) const
{
	const std::string value = get_column_string(index);
	return static_cast<unsigned char>(StringHelp::text_to_uint(value));
}

int PgsqlReaderProvider::get_column_int(int index) const
{
	const std::string value = get_column_string(index);
	return StringHelp::text_to_int(value);
}

unsigned int PgsqlReaderProvider::get_column_uint(int index) const
{
	const std::string value = get_column_string(index);
	return StringHelp::text_to_uint(value);
}

double PgsqlReaderProvider::get_column_double(int index) const
{
	const std::string value = get_column_string(index);
	return StringHelp::text_to_double(value);
}

DateTime PgsqlReaderProvider::get_column_datetime(int index) const
{
	return PgsqlConnectionProvider::from_sql_datetime(get_column_string(index));
}

DataBuffer PgsqlReaderProvider::get_column_binary(int index) const
{
	const unsigned char *const str = reinterpret_cast<unsigned char*>(PQgetvalue(result, current_row, index));
	if (str == nullptr)
		throw ("Index out of range");
	size_t length;
	auto deleter = [](void *ptr) {if (ptr) {free(ptr);} };
	std::unique_ptr<unsigned char, decltype(deleter)> value(PQunescapeBytea(
				str,
				&length),
				deleter);
	DataBuffer output(value.get(), length);
	return output;
}

/////////////////////////////////////////////////////////////////////////////
// PgsqlReaderProvider Operations:

bool PgsqlReaderProvider::retrieve_row()
{
	if (1 + current_row >= nb_rows)
		return false;
	++current_row;
	return true;
}

void PgsqlReaderProvider::close()
{
	if (!closed)
	{
		PQclear(result);
		closed = true;
		result = nullptr;
	}
}

/////////////////////////////////////////////////////////////////////////////
// PgsqlReaderProvider Implementation:

}; //namespace clan
