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
#include "API/Database/db_reader_provider.h"

class CL_PgsqlCommandProvider;
class CL_PgsqlConnectionProvider;

/// \brief Pgsql database reader provider.
class CL_PgsqlReaderProvider : public CL_DBReaderProvider
{
/// \name Construction
/// \{
public:
	CL_PgsqlReaderProvider(CL_PgsqlConnectionProvider *connection, CL_PgsqlCommandProvider *command);
	~CL_PgsqlReaderProvider();
/// \}

/// \name Attributes
/// \{
public:
	int get_column_count() const;
	CL_String get_column_name(int index) const;
	int get_name_index(const CL_StringRef &name) const;
	CL_String get_column_string(int index) const;
	bool get_column_bool(int index) const;
	char get_column_char(int index) const;
	unsigned char get_column_uchar(int index) const;
	int get_column_int(int index) const;
	unsigned int get_column_uint(int index) const;
	double get_column_double(int index) const;
	CL_DateTime get_column_datetime(int index) const;
	CL_DataBuffer get_column_binary(int index) const;
/// \}

/// \name Operations
/// \{
public:
	bool retrieve_row();
	void close();
/// \}

/// \name Implementation
/// \{
private:
	enum class ResultType
	{
		EMPTY_RESULT,
		TUPLES_RESULT
	};

	CL_PgsqlConnectionProvider *connection;
	CL_PgsqlCommandProvider *command;
	PGresult *result;
	ResultType type;
	bool closed;
	int current_row;
	int nb_rows;

	friend class CL_PgsqlConnectionProvider;
	friend class CL_PgsqlCommandProvider;
/// \}
};


/// \}
