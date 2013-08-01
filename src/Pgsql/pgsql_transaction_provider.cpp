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
#include "pgsql_transaction_provider.h"
#include "pgsql_connection_provider.h"
#include "ClanLib/Database/db_command_provider.h"
#include "ClanLib/Core/System/databuffer.h"
#include "ClanLib/Core/System/uniqueptr.h"
#include "ClanLib/Core/Text/string_help.h"

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlTransactionProvider Construction:

CL_PgsqlTransactionProvider::CL_PgsqlTransactionProvider(CL_PgsqlConnectionProvider *connection, const CL_DBTransaction::Type type)
: connection(connection), type(type)
{
	//We assert that (connection != nullptr)
	if (connection->active_transaction)
		throw CL_Exception("Only one database transaction may be active for a connection");
	CL_String transaction_type;
	switch (type)
	{
	case CL_DBTransaction::deferred:
		transaction_type = "SET CONSTRAINTS ALL DEFERRED;";
		break;
	case CL_DBTransaction::immediate:
		transaction_type = "SET CONSTRAINTS ALL IMMEDIATE;";
		break;
	case CL_DBTransaction::default_transaction:
		break;
	default:
		throw CL_Exception("Unknown transaction type");
	}
	execute("START TRANSACTION;");
	if (!transaction_type.empty())
		execute(transaction_type);
	connection->active_transaction = this;
}

CL_PgsqlTransactionProvider::~CL_PgsqlTransactionProvider()
{
	rollback();
	if (connection)
		connection->active_transaction = nullptr;
}

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlTransactionProvider Attributes:


/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlTransactionProvider Operations:

void CL_PgsqlTransactionProvider::commit()
{
	//We assert that (connection != nullptr) and (connection->active_transaction == this|nullptr)
	if (connection->active_transaction)
	{
		execute("COMMIT;");
		connection->active_transaction = nullptr;
	}
}

void CL_PgsqlTransactionProvider::rollback()
{
	//We assert that (connection != nullptr) and (connection->active_transaction == this|nullptr)
	if (connection->active_transaction)
	{
		execute("ROLLBACK;");
		connection->active_transaction = nullptr;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CL_PgsqlTransactionProvider Implementation:

inline
void CL_PgsqlTransactionProvider::execute(const CL_String &cmd)
{
	CL_UniquePtr<CL_DBCommandProvider> command(
		connection->create_command(
			cmd,
			CL_DBCommand::sql_statement)
		);
	connection->execute_non_query(command.get());
}
