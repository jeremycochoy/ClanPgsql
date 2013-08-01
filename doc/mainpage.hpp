//! \mainpage
//!
//! \section summary Summary
//! ClanPgsql is a PostgreSQL backend for ClanLib's Database library.
//! It allow using postgresql more or less the same way as ClanSqlite.
//!
//! \section sample A code sample
//!
//! \code
//!
//! CL_PgsqlConnection connection("postgresql://user@localhost/database?connect_timeout=10&application_name=bar");
//!
//! CL_DBCommand command = connection.create_command("SELECT UserName FROM Users WHERE UserId=?1");
//! command.set_input_parameter_int(1, user_id);
//! CL_String user_name = connection.execute_scalar_string(command)
//!
//! \endcode
