// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
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
	The $cref/name/option_table/Conventions/option_name/$$
	for this option
$rnext
$code std::string$$ $cnext $code option_value$$  $cnext
	The $cref/value/option_table/Conventions/option_value/$$
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



# include <cppad/utility/to_string.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_option_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/split_space.hpp>
# include <dismod_at/log_message.hpp>

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
		// BEGIN_SORT_THIS_LINE_PLUS_1
		{ "accept_after_max_steps_fixed",     "5"                  },
		{ "accept_after_max_steps_random",    "5"                  },
		{ "age_avg_split",                    ""                   },
		{ "avgint_extra_columns",             ""                   },
		{ "bound_frac_fixed",                 "1e-2"               },
		{ "bound_random",                     ""                   },
		{ "data_extra_columns",               ""                   },
		{ "derivative_test_fixed",            "none"               },
		{ "derivative_test_random",           "none"               },
		{ "limited_memory_max_history_fixed", "30"                 },
		{ "max_num_iter_fixed",               "100"                },
		{ "max_num_iter_random",              "100"                },
		{ "meas_noise_effect",                "add_std_scale_all"  },
		{ "ode_step_size",                    "10.0"               },
		{ "parent_node_id",                   ""                   },
		{ "parent_node_name",                 ""                   },
		{ "print_level_fixed",                "0"                  },
		{ "print_level_random",               "0"                  },
		{ "quasi_fixed",                      "true"               },
		{ "random_seed",                      "0"                  },
		{ "rate_case",                        "iota_pos_rho_zero"  },
		{ "tolerance_fixed",                  "1e-8"               },
		{ "tolerance_random",                 "1e-8"               },
		{ "warn_on_stderr",                   "true"               },
		{ "zero_sum_random",                  ""                   }
		// END_SORT_THIS_LINE_MINUS_1
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
	for(size_t option_id = 0; option_id < n_in_table; option_id++)
	{	// minimum_meas_cv
		if( option_name[option_id] == "minimum_meas_cv" )
		{	msg  = "minimum_meas_cv is no longer a valid option name";
			msg += "\nThis was moved to integrand table on 2018-05-23.";
			error_exit(msg, table_name, option_id);
		}
		if( option_name[option_id] == "meas_std_effect" )
        {	msg  = "meas_std_effect was deprecated on 2019-04-07\n";
			msg += "and may not work in the future. ";
			msg += "It should be changed to meas_noise_effect.";
			log_message(
				db, &std::cout, "warning", msg, table_name, option_id
			);
			option_name[option_id] = "meas_noise_effect";
		}
		//
		size_t match = n_option;
		for(size_t i = 0; i < n_option; i++)
			if( name_vec[i] == option_name[option_id] )
				match = i;
		if( match == n_option )
		{	msg  =  option_name[option_id];
			msg += " is not a valid option_name";
			error_exit(msg, table_name, option_id);
		}
		value_vec[match] = option_value[option_id];
		//
		if( name_vec[match] == "meas_noise_effect" )
		{	bool ok = false;
			ok     |= option_value[option_id] == "add_std_scale_all";
			ok     |= option_value[option_id] == "add_std_scale_log";
			ok     |= option_value[option_id] == "add_var_scale_all";
			ok     |= option_value[option_id] == "add_var_scale_log";
			if( ! ok )
			{	msg = "option table: meas_noise_effect = '";
				msg += option_value[option_id] + "'";
				error_exit(msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "rate_case" )
		{	bool ok = false;
			ok     |= option_value[option_id] == "iota_pos_rho_zero";
			ok     |= option_value[option_id] == "iota_zero_rho_pos";
			ok     |= option_value[option_id] == "iota_zero_rho_zero";
			ok     |= option_value[option_id] == "iota_pos_rho_pos";
			if( ! ok )
			{	msg = "option table: rate_case = '";
				msg += option_value[option_id] + "'";
				error_exit(msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "ode_step_size" )
		{	bool ok = std::atof( option_value[option_id].c_str() ) > 0.0;
			if( ! ok )
			{	msg = "option_value is <= 0.0 for ode_step_size";
				error_exit(msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "random_seed" )
		{	bool ok = std::atoi( option_value[option_id].c_str() ) >= 0;
			if( ! ok )
			{	msg = "option_value is < 0 for random_seed";
				error_exit(msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "quasi_fixed" )
		{	if( option_value[option_id] == "true" )
				quasi_fixed = true;
			else if( option_value[option_id] == "false" )
				quasi_fixed = false;
			else
			{	msg = "option_value is not true or false";
				error_exit(msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "warn_on_stderr" )
		{	if(
				option_value[option_id] != "true" &&
				option_value[option_id] != "false" )
			{	msg = "option_value is not true or false";
				error_exit(msg, table_name, option_id);
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
				error_exit(msg, table_name, option_id);
			}
		}
		if( name_vec[match] == "bound_random" )
		{	std::string bound_random_str = option_value[option_id];
			if( bound_random_str != "" )
			{	double bound_random = std::atof( bound_random_str.c_str() );
				bool ok = 0.0 <= bound_random;
				if( ! ok )
				{	msg = "option_value is less than zero.";
					error_exit(msg, table_name, option_id);
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
				error_exit(msg, table_name, option_id);
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
				error_exit(msg, table_name, option_id);
			}
		}
		if(
			name_vec[match] == "accept_after_max_steps_fixed"   ||
			name_vec[match] == "accept_after_max_steps_random"  ||
			name_vec[match] == "limited_memory_max_history_fixed"
		)
		{	int pos_integer = std::atoi( option_value[option_id].c_str() );
			bool ok = 0 < pos_integer;
			if( ! ok )
			{	msg = "option_value is not a positive integer";
				error_exit(msg, table_name, option_id);
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
			ok     |= option_value[option_id] == "adaptive";
			ok     |= option_value[option_id] == "trace-adaptive";
			if( ! ok )
			{	msg  = "option_value not one of the following:\n";
				msg += "none, first-order, second-order, only-second-order\n";
				msg += "adaptive, trace-adaptive";
				error_exit(msg, table_name, option_id);
			}
			if( name_vec[match] == "derivative_test_fixed" )
			{	if( option_value[option_id] == "none" )
					derivative_test_fixed_level = 0;
				else if( option_value[option_id] == "second-order" )
					derivative_test_fixed_level = 2;
				else if( option_value[option_id] == "only-second-order" )
					derivative_test_fixed_level = 2;
				else
					derivative_test_fixed_level = 1;
			}
		}
		if( name_vec[match] == "zero_sum_random" )
		{	CppAD::vector<string> rate_list = split_space(
					option_value[option_id]
			);
			size_t n_rate = number_rate_enum;
			for(size_t i = 0; i < rate_list.size(); i++)
			{	string rate_name = rate_list[i];
				bool found = false;
				for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
					found = found || rate_name == get_rate_name(rate_id);
				if( ! found )
				{	msg  = rate_name + " in option_value list";
					msg += " is not a valid rate name";
					error_exit(msg, table_name, option_id);
				}
			}
		}
	}
	if( quasi_fixed && (derivative_test_fixed_level > 1 ) )
	{	msg  = "quasi_fixed option is true and derivative_test_fixed";
		msg += " is second-order or only-second-order";
		error_exit(msg, table_name);
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
