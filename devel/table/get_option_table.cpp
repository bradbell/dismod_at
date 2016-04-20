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

$section C++: Get the Node Table Information$$

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
For each $cref/option_id/option_table/option_id/$$,
$codei%
	%option_table%[%option_id%]
%$$
is the information for the corresponding command option.

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
	size_t null_id  = size_t(DISMOD_AT_NULL_INT);
	//
	const char* name_list[] = {
		"number_sample",
		"fit_simulate_index",
		"ode_step_size",
		"parent_node_id",
		"random_seed",
		"rate_case",
		"quasi_fixed",
		"derivative_test_fixed",
		"derivative_test_random",
		"print_level_fixed",
		"print_level_random",
		"max_num_iter_fixed",
		"max_num_iter_random",
		"tolerance_fixed",
		"tolerance_random",
		"random_bound"
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
	int derivative_test_fixed_level = -1;
	int quasi_fixed = -1;
	//
	int number_sample = -1;
	string fit_simulate_index;
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
		if( name_vec[i] == "rate_case" )
		{	bool ok = false;
			ok     |= option_value[match] == "iota_pos_rho_zero";
			ok     |= option_value[match] == "iota_zero_rho_pos";
			ok     |= option_value[match] == "iota_zero_rho_zero";
			ok     |= option_value[match] == "iota_pos_rho_pos";
			if( ! ok )
			{	msg = "option table: rate_case = '";
				msg += option_value[match] + "'";
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
		{	number_sample = std::atoi( option_value[match].c_str() );
			bool ok = number_sample >= 1;
			if( ! ok )
			{	msg = "option_value is < 1 for number_sample";
				error_exit(db, msg, table_name, match);
			}
		}
		if( name_vec[i] == "fit_simulate_index" )
		{	fit_simulate_index = option_value[match].c_str();
			if( fit_simulate_index != "" )
			{	bool ok = std::atoi( fit_simulate_index.c_str() ) >= 0;
				if( ! ok )
				{	msg = "option_value is < 0 for fit_simulate_index";
					error_exit(db, msg, table_name, match);
				}
			}
		}
		if( name_vec[i] == "quasi_fixed" )
		{	if( option_value[match] == "true" )
				quasi_fixed = 1;
			else if( option_value[match] == "false" )
				quasi_fixed = 0;
			else
			{	msg = "option_value is not true or false";
				error_exit(db, msg, table_name, match);
			}
		}
		if(
			name_vec[i] == "tolerance_fixed" ||
			name_vec[i] == "tolerance_random"
		)
		{	double tolerance = std::atof( option_value[match].c_str() );
			bool ok = 0.0 < tolerance;
			if( ! ok )
			{	msg = "option_value is not greater than zero";
				error_exit(db, msg, table_name, match);
			}
		}
		if( name_vec[i] == "random_bound" )
		{	std::string random_bound_str = option_value[match];
			if( random_bound_str != "" )
			{	double random_bound = std::atof( random_bound_str.c_str() );
				bool ok = 1.0 <= random_bound;
				if( ! ok )
				{	msg = "option_value is not greater than or equal one.";
					error_exit(db, msg, table_name, match);
				}
			}
		}
		if(
			name_vec[i] == "max_num_iter_fixed" ||
			name_vec[i] == "max_num_iter_random"
		)
		{	int max_num_iter = std::atoi( option_value[match].c_str() );
			bool ok = 0 <= max_num_iter;
			if( ! ok )
			{	msg = "option_value is less than zero";
				error_exit(db, msg, table_name, match);
			}
		}
		if(
			name_vec[i] == "print_level_fixed" ||
			name_vec[i] == "print_level_random"
		)
		{	int print_level = std::atoi( option_value[match].c_str() );
			bool ok = 0 <= print_level && print_level <= 12;
			if( ! ok )
			{	msg = "option_value is not between 0 and 12 inclusive";
				error_exit(db, msg, table_name, match);
			}
		}
		if(
			name_vec[i] == "derivative_test_fixed" ||
			name_vec[i] == "derivative_test_random"
		)
		{	bool ok = false;
			ok     |= option_value[match] == "none";
			ok     |= option_value[match] == "first-order";
			ok     |= option_value[match] == "second-order";
			ok     |= option_value[match] == "only-second-order";
			if( ! ok )
			{	msg  = "option_value not one of the following: ";
				msg += "none, first-order, second-order, only-second-order";
				error_exit(db, msg, table_name, match);
			}
			if( name_vec[i] == "derivative_test_fixed" )
			{	if( option_value[match] == "none" )
					derivative_test_fixed_level = 0;
				else if( option_value[match] == "first-order" )
					derivative_test_fixed_level = 1;
				else
					derivative_test_fixed_level = 2;
			}
		}
	}
	assert( quasi_fixed != -1 );
	if( (quasi_fixed == 1) && (derivative_test_fixed_level > 1 ) )
	{	msg  = "quasi_fixed option is true and derivative_test_fixed";
		msg += " is second-order or only-second-order";
		error_exit(db, msg, table_name);
	}
	if( fit_simulate_index != "" )
	{	if( std::atoi( fit_simulate_index.c_str() ) >= number_sample )
		{	msg  = "sample_index is greater than or equal number_sample";
			error_exit(db, msg, table_name);
		}
	}
	//
	// return table
	CppAD::vector<option_struct> option_table(n_option);
	for(size_t i = 0; i < n_option; i++)
	{	option_table[i].option_name  = option_name[i];
		option_table[i].option_value = option_value[i];
	}
	return option_table;
}

} // END DISMOD_AT_NAMESPACE
