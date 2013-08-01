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
#include "pgsql_reader_provider.h"
#include "pgsql_connection_provider.h"
#include "pgsql_command_provider.h"
#include "ClanLib/Core/System/databuffer.h"
#include "ClanLib/Core/System/datetime.h"
#include "ClanLib/Core/Text/string_help.h"
#include "ClanLib/Core/Text/string_format.h"
#include <libpq-fe.h>
#include <cstdlib>

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlReaderProvider Construction:

CL_PgsqlReaderProvider::CL_PgsqlReaderProvider(CL_PgsqlConnectionProvider *connection, CL_PgsqlCommandProvider *command)
    : connection(connection), command(command), closed(false), current_row(-1)
{
	auto deleter = [](PGresult *ptr) {if (ptr) {PQclear(ptr);} };
	CL_UniquePtr<PGresult, decltype(deleter)> result_uniqueptr(command->exec_command(), deleter);
	result = result_uniqueptr.get();

	switch (PQresultStatus(result))
	{
	case PGRES_EMPTY_QUERY:
		throw CL_Exception("Empty query");

	case PGRES_COMMAND_OK:
		type = ResultType::EMPTY_RESULT;
		break;
	case PGRES_TUPLES_OK:
		type = ResultType::TUPLES_RESULT;
		break;

	case PGRES_NONFATAL_ERROR:
		throw CL_Exception("Server gave an unknow answer");

	case PGRES_FATAL_ERROR:
		throw CL_Exception(PQerrorMessage(connection->db));

	default:
		throw CL_Exception(CL_StringHelp::text_to_local8(PQresultErrorMessage(result)));
	}
	nb_rows = PQntuples(result);
	result_uniqueptr.release();
}

CL_PgsqlReaderProvider::~CL_PgsqlReaderProvider()
{
	close();
}

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlReaderProvider Attributes:

int CL_PgsqlReaderProvider::get_column_count() const
{
	return  PQnfields(result);
}

CL_String CL_PgsqlReaderProvider::get_column_name(int index) const
{
	const char *const string = PQfname(result, index);
	if (string == nullptr)
		throw ("Index out of range");
	return CL_String(string);
}

int CL_PgsqlReaderProvider::get_name_index(const CL_StringRef &name) const
{
	int index = PQfnumber(result, name.c_str());
	if (index < 0)
		throw CL_Exception(cl_format("No such column name %1", name));
	return index;
}

CL_String CL_PgsqlReaderProvider::get_column_string(int index) const
{
	const char *const str = PQgetvalue(result, current_row, index);
	if (str != nullptr)
		return str;
	else
		throw ("Index out of range");
}

bool CL_PgsqlReaderProvider::get_column_bool(int index) const
{
	const CL_String value = get_column_string(index);
	return CL_StringHelp::text_to_bool(value);
}

char CL_PgsqlReaderProvider::get_column_char(int index) const
{
	const CL_String value = get_column_string(index);
	return static_cast<int>(CL_StringHelp::text_to_int(value));
}

unsigned char CL_PgsqlReaderProvider::get_column_uchar(int index) const
{
	const CL_String value = get_column_string(index);
	return static_cast<unsigned char>(CL_StringHelp::text_to_uint(value));
}

int CL_PgsqlReaderProvider::get_column_int(int index) const
{
	const CL_String value = get_column_string(index);
	return CL_StringHelp::text_to_int(value);
}

unsigned int CL_PgsqlReaderProvider::get_column_uint(int index) const
{
	const CL_String value = get_column_string(index);
	return CL_StringHelp::text_to_uint(value);
}

double CL_PgsqlReaderProvider::get_column_double(int index) const
{
	const CL_String value = get_column_string(index);
	return CL_StringHelp::text_to_double(value);
}

CL_DateTime CL_PgsqlReaderProvider::get_column_datetime(int index) const
{
	return CL_PgsqlConnectionProvider::from_sql_datetime(get_column_string(index));
}

CL_DataBuffer CL_PgsqlReaderProvider::get_column_binary(int index) const
{
	const unsigned char *const str = reinterpret_cast<unsigned char*>(PQgetvalue(result, current_row, index));
	if (str == nullptr)
		throw ("Index out of range");
	size_t length;
	auto deleter = [](void *ptr) {if (ptr) {free(ptr);} };
	CL_UniquePtr<unsigned char, decltype(deleter)> value(PQunescapeBytea(
				str,
				&length),
				deleter);
	CL_DataBuffer output(value.get(), length);
	return output;
}

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlReaderProvider Operations:

bool CL_PgsqlReaderProvider::retrieve_row()
{
	if (1 + current_row >= nb_rows)
		return false;
	++current_row;
	return true;
}

void CL_PgsqlReaderProvider::close()
{
	if (!closed)
	{
		PQclear(result);
		closed = true;
		result = nullptr;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlReaderProvider Implementation:
