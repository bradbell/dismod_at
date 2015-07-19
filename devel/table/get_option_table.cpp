// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_agument_table$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Node Table Information$$

$head Syntax$$
$icode%agument_table% = get_agument_table(%db%)%$$

$head Purpose$$
To read the $cref agument_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head agument_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code std::string$$ $cnext $code agument_name$$  $cnext
	The $cref/agument_name/agument_table/agument_name/$$u
	for this command argument
$rnext
$code std::string$$ $cnext $code agument_value$$  $cnext
	The $cref/agument_value/agument_table/agument_value/$$
	for this command argument
$rnext
$tend

$head option_table$$
The return value $icode option_table$$ has prototype
$codei%
	CppAD::vector<option_struct>  %option_table%
%$$
For each $cref/option_id/option_table/option_id/$$,
$codei%
	%option_table%[%option_id%]
%$$
is the information for the corresponding comamnd argument.

$children%example/devel/table/get_option_table_xam.cpp
%$$
$head Example$$
The file $cref get_option_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/get_option_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/to_string.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<option_struct> get_option_table(sqlite3* db)
{	using std::string;
	//
	// for error messaging
	string msg;
	size_t null_id  = size_t(DISMOD_AT_NULL_INT);
	//
	const char* name_list[] = {
		"max_num_iter",
		"number_sample",
		"ode_step_size",
		"print_level",
		"parent_node_id",
		"tolerance",
		"random_seed",
		"rate_info"
	};
	size_t n_name = sizeof( name_list ) / sizeof( name_list[0] );
	CppAD::vector<string> name_vec(n_name);
	for(size_t i = 0; i < n_name; i++)
		name_vec[i] = name_list[i];
	//
	string table_name  = "option";
	size_t n_option      = check_table_id(db, table_name);
	if( n_name != n_option )
	{	msg = "option table does not have " + to_string(n_name) + " rows.";
		error_exit(db, msg, table_name, null_id);
	}
	//
	string column_name = "option_name";
	CppAD::vector<string>  option_name;
	get_table_column(db, table_name, column_name, option_name);
	assert( n_option == option_name.size() );
	//
	column_name = "option_value";
	CppAD::vector<string>  option_value;
	get_table_column(db, table_name, column_name, option_value);
	assert( n_option == option_value.size() );
	//
	// check table
	for(size_t i = 0; i < n_name; i++)
	{	size_t match = n_option;
		for(size_t option_id = 0; option_id < n_option; option_id++)
			if( name_vec[i] == option_name[option_id] )
				match = option_id;
		if( match == n_option )
		{	msg  = "table does not have a row with ";
			msg +=  "option_name = ";
			msg +=  name_vec[i];
			error_exit(db, msg, table_name, null_id);
		}
		if( name_vec[i] == "rate_info" )
		{	bool ok = false;
			ok     |= option_value[match] == "chi_positive";
			ok     |= option_value[match] == "iota_and_chi_zero";
			ok     |= option_value[match] == "rho_and_chi_zero";
			ok     |= option_value[match] == "iota_and_rho_zero";
			if( ! ok )
			{	msg = "option_value not valid for rate_info";
				error_exit(db, msg, table_name, match);
			}
		}
		if( name_vec[i] == "ode_step_size" )
		{	bool ok = std::atof( option_value[match].c_str() ) > 0.0;
			if( ! ok )
			{	msg = "option_value is <= 0.0 for ode_step_size";
				error_exit(db, msg, table_name, match);
			}
		}
		if( name_vec[i] == "random_seed" )
		{	bool ok = std::atoi( option_value[match].c_str() ) >= 0;
			if( ! ok )
			{	msg = "option_value is < 0 for random_seed";
				error_exit(db, msg, table_name, match);
			}
		}
		if( name_vec[i] == "number_sample" )
		{	bool ok = std::atoi( option_value[match].c_str() ) >= 1;
			if( ! ok )
			{	msg = "option_value is < 1 for number_sample";
				error_exit(db, msg, table_name, match);
			}
		}
		if( name_vec[i] == "print_level" )
		{	int print_level = std::atoi( option_value[match].c_str() );
			bool ok = 0 <= print_level && print_level <= 12;
			if( ! ok )
			{	msg = "option_value is not between 0 and 12 inclusinve";
				error_exit(db, msg, table_name, match);
			}
		}

	}
	//
	// return table
	CppAD::vector<option_struct> option_table(n_option);
	for(size_t i = 0; i < n_option; i++)
	{	option_table[i].option_name    = option_name[i];
		option_table[i].option_value = option_value[i];
	}
	return option_table;
}

} // END DISMOD_AT_NAMESPACE
