// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_option_table$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Option Table Information$$

$head Syntax$$
$icode%option_table% = get_option_table(%db%)%$$

$head Purpose$$
To read the $cref option_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head option_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code std::string$$ $cnext $code option_name$$  $cnext
	The $cref/name/option_table/option_name/$$
	for this option
$rnext
$code std::string$$ $cnext $code option_value$$  $cnext
	The $cref/value/option_table/option_value/$$
	for this option
$rnext
$tend

$head option_table$$
The return value $icode option_table$$ has prototype
$codei%
	CppAD::vector<option_struct>  %option_table%
%$$
For $icode%i% = 0 , %...%, %n_option%$$,
$codei%
	%option_table%[%i%]
%$$
is the information for the one option.

$head Special Note$$
This table is special because the index $icode i$$
is not the same as $icode option_id$$ in the $cref option_table$$.
This is because all the options have default values that are used when
an option does not appear in the actual table.

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
# include <cppad/utility/to_string.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<option_struct> get_option_table(sqlite3* db)
{	using std::string;
	using CppAD::to_string;
	//
	// for error messaging
	string msg;
	//
	// option name and its default value
	// Changes here must also be made in python/dismod_at/db2csv_command.py
	struct { const char* name; const char* value; } option_list[] = {
		"accept_after_max_steps_fixed",  "5",
		"accept_after_max_steps_random", "5",
		"derivative_test_fixed",         "none",
		"derivative_test_random",        "none",
		"fit_simulate_index",            "",
		"fixed_bound_frac",              "1e-2",
		"max_num_iter_fixed",            "100",
		"max_num_iter_random",           "100",
		"number_simulate",               "1",
		"ode_step_size",                 "10.0",
		"parent_node_id",                "0",
		"print_level_fixed",             "0",
		"print_level_random",            "0",
		"quasi_fixed",                   "true",
		"random_bound",                  "",
		"random_seed",                   "0",
		"rate_case",                     "iota_pos_rho_zero",
		"tolerance_fixed",               "1e-8",
		"tolerance_random",              "1e-8"
	};
	size_t n_option = sizeof( option_list ) / sizeof( option_list[0] );
	CppAD::vector<string> name_vec(n_option), value_vec(n_option);
	for(size_t i = 0; i < n_option; i++)
	{	name_vec[i]   = option_list[i].name;
		value_vec[i]  = option_list[i].value;
	}
	//
	string table_name  = "option";
	size_t n_in_table  = check_table_id(db, table_name);
	//
	string column_name = "option_name";
	CppAD::vector<string>  option_name;
	get_table_column(db, table_name, column_name, option_name);
	assert( n_in_table == option_name.size() );
	//
	column_name = "option_value";
	CppAD::vector<string>  option_value;
	get_table_column(db, table_name, column_name, option_value);
	assert( n_in_table == option_value.size() );
	//
	// values in table
	size_t  derivative_test_fixed_level = 0;
	bool    quasi_fixed                 = true;
	int     number_simulate             = 1;
	string  fit_simulate_index          = "";
	for(size_t option_id = 0; option_id < n_in_table; option_id++)
	{	size_t match = n_option;
		for(size_t i = 0; i < n_option; i++)
			if( name_vec[i] == option_name[option_id] )
				match = i;
		if( match == n_option )
		{	msg +=  option_name[option_id];
			msg += " is not a valid option_name";
			error_exit(db, msg, table_name, option_id);
		}
		value_vec[match] = option_value[option_id];
		//
		if( name_vec[match] == "rate_case" )
		{	bool ok = false;
			ok     |= option_value[option_id] == "iota_pos_rho_zero";
			ok     |= option_value[option_id] == "iota_zero_rho_pos";
			ok     |= option_value[option_id] == "iota_zero_rho_zero";
			ok     |= option_value[option_id] == "iota_pos_rho_pos";
			if( ! ok )
			{	msg = "option table: rate_case = '";
				msg += option_value[option_id] + "'";
				error_exit(db, msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "ode_step_size" )
		{	bool ok = std::atof( option_value[option_id].c_str() ) > 0.0;
			if( ! ok )
			{	msg = "option_value is <= 0.0 for ode_step_size";
				error_exit(db, msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "random_seed" )
		{	bool ok = std::atoi( option_value[option_id].c_str() ) >= 0;
			if( ! ok )
			{	msg = "option_value is < 0 for random_seed";
				error_exit(db, msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "number_simulate" )
		{	number_simulate = std::atoi( option_value[option_id].c_str() );
			bool ok = number_simulate >= 1;
			if( ! ok )
			{	msg = "option_value is < 1 for number_simulate";
				error_exit(db, msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "fit_simulate_index" )
		{	fit_simulate_index = option_value[option_id].c_str();
			if( fit_simulate_index != "" )
			{	bool ok = std::atoi( fit_simulate_index.c_str() ) >= 0;
				if( ! ok )
				{	msg = "option_value is < 0 for fit_simulate_index";
					error_exit(db, msg, table_name, option_id);
				}
			}
		}
		if( name_vec[match] == "quasi_fixed" )
		{	if( option_value[option_id] == "true" )
				quasi_fixed = true;
			else if( option_value[option_id] == "false" )
				quasi_fixed = false;
			else
			{	msg = "option_value is not true or false";
				error_exit(db, msg, table_name, option_id);
			}
		}
		if(
			name_vec[match] == "tolerance_fixed" ||
			name_vec[match] == "tolerance_random"
		)
		{	double tolerance = std::atof( option_value[option_id].c_str() );
			bool ok = 0.0 < tolerance;
			if( ! ok )
			{	msg = "option_value is not greater than zero";
				error_exit(db, msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "random_bound" )
		{	std::string random_bound_str = option_value[option_id];
			if( random_bound_str != "" )
			{	double random_bound = std::atof( random_bound_str.c_str() );
				bool ok = 0.0 <= random_bound;
				if( ! ok )
				{	msg = "option_value is less than zero.";
					error_exit(db, msg, table_name, option_id);
				}
			}
		}
		if(
			name_vec[match] == "max_num_iter_fixed" ||
			name_vec[match] == "max_num_iter_random"
		)
		{	int max_num_iter = std::atoi( option_value[option_id].c_str() );
			bool ok = -1 <= max_num_iter;
			if( ! ok )
			{	msg = "option_value is less than minus one";
				error_exit(db, msg, table_name, option_id);
			}
		}
		if(
			name_vec[match] == "print_level_fixed" ||
			name_vec[match] == "print_level_random"
		)
		{	int print_level = std::atoi( option_value[option_id].c_str() );
			bool ok = 0 <= print_level && print_level <= 12;
			if( ! ok )
			{	msg = "option_value is not between 0 and 12 inclusive";
				error_exit(db, msg, table_name, option_id);
			}
		}
		if(
			name_vec[match] == "accept_after_max_steps_fixed" ||
			name_vec[match] == "accept_after_max_steps_random"
		)
		{	int max_steps = std::atoi( option_value[option_id].c_str() );
			bool ok = 0 < max_steps;
			if( ! ok )
			{	msg = "option_value is not positive";
				error_exit(db, msg, table_name, option_id);
			}
		}
		if(
			name_vec[match] == "derivative_test_fixed" ||
			name_vec[match] == "derivative_test_random"
		)
		{	bool ok = false;
			ok     |= option_value[option_id] == "none";
			ok     |= option_value[option_id] == "first-order";
			ok     |= option_value[option_id] == "second-order";
			ok     |= option_value[option_id] == "only-second-order";
			if( ! ok )
			{	msg  = "option_value not one of the following: ";
				msg += "none, first-order, second-order, only-second-order";
				error_exit(db, msg, table_name, option_id);
			}
			if( name_vec[match] == "derivative_test_fixed" )
			{	if( option_value[option_id] == "none" )
					derivative_test_fixed_level = 0;
				else if( option_value[option_id] == "first-order" )
					derivative_test_fixed_level = 1;
				else
					derivative_test_fixed_level = 2;
			}
		}
	}
	if( quasi_fixed && (derivative_test_fixed_level > 1 ) )
	{	msg  = "quasi_fixed option is true and derivative_test_fixed";
		msg += " is second-order or only-second-order";
		error_exit(db, msg, table_name);
	}
	if( fit_simulate_index != "" )
	{	if( std::atoi( fit_simulate_index.c_str() ) >= number_simulate )
		{	msg  = "fit_simulate_index is greater than or equal number_simulate";
			error_exit(db, msg, table_name);
		}
	}
	//
	// return table
	CppAD::vector<option_struct> option_table(n_option);
	for(size_t i = 0; i < n_option; i++)
	{	option_table[i].option_name  = name_vec[i];
		option_table[i].option_value = value_vec[i];
	}
	return option_table;
}

} // END DISMOD_AT_NAMESPACE
