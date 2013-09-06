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

#include <memory>
#include <map>

#include "api_pgsql.h"
#include "ClanLib/Database/db_connection.h"

namespace clan
{

/// \brief Sqlite database connection.
///
/// \xmlonly !group=Sqlite/System! !header=sqlite.h! \endxmlonly
class CL_API_PGSQL PgsqlConnection : public DBConnection
{
/// \name Construction
/// \{

public:

	typedef std::map<std::string, std::string> Parameters;

	/// \brief Constructs a PgsqlConnection
	///
	/// \param parameters = List of std::paire<Key, Value>
	PgsqlConnection(const Parameters &parameters);

	/// \brief Constructs a PgsqlConnection
	///
	/// \param connection_string = Parameters as a string.
	///        If empty, default parameters are used.
	///        It could be a connection uri. (see libpq-connect documentation)
	PgsqlConnection(const std::string &connecton_tring);

	~PgsqlConnection();

/// \}
/// \name Attributes
/// \{

public:

/// \}
/// \name Operations
/// \{

public:

/// \}
/// \name Implementation
/// \{

private:
/// \}
};

}; // namespace clan

/// \}
