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

$head argument_table$$
The return value $icode argument_table$$ has prototype
$codei%
	CppAD::vector<argument_struct>  %argument_table%
%$$
For each $cref/argument_id/argument_table/argument_id/$$,
$codei%
	%argument_table%[%argument_id%]
%$$
is the information for the corresponding comamnd argument.

$children%example/devel/table/get_argument_table_xam.cpp
%$$
$head Example$$
The file $cref get_argument_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/get_argument_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<argument_struct> get_argument_table(sqlite3* db)
{	using std::string;
	//
	const char* name_list[] = {
		"max_num_iter",
		"number_sample",
		"ode_step_size",
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
	string table_name  = "argument";
	size_t n_argument      = check_table_id(db, table_name);
	if( n_name != n_argument )
	{	using std::cerr;
		cerr << "argument table does not have ";
		cerr << size_t( n_name ) << "rows." << std::endl;
		std::exit(1);
	}
	//
	string column_name = "argument_name";
	CppAD::vector<string>  argument_name;
	get_table_column(db, table_name, column_name, argument_name);
	assert( n_argument == argument_name.size() );
	//
	column_name = "argument_value";
	CppAD::vector<string>  argument_value;
	get_table_column(db, table_name, column_name, argument_value);
	assert( n_argument == argument_value.size() );
	//
	// check table
	for(size_t i = 0; i < n_name; i++)
	{	size_t match = n_argument;
		for(size_t argument_id = 0; argument_id < n_argument; argument_id++)
			if( name_vec[i] == argument_name[argument_id] )
				match = argument_id;
		if( match == n_argument )
		{	std::cerr << "argument table does not have a row with "
			<< "argument name = " << name_vec[i] << std::endl;
			std::exit(1);
		}
		if( name_vec[i] == "rate_info" )
		{	bool ok = false;
			ok     |= argument_value[match] == "chi_positive";
			ok     |= argument_value[match] == "iota_and_chi_zero";
			ok     |= argument_value[match] == "rho_and_chi_zero";
			ok     |= argument_value[match] == "iota_and_rho_zero";
			if( ! ok )
			{	std::cerr
				<< "argument table: argument_id = " << match << std::endl
				<< "argument_value not valid for rate_info" << std::endl;
				std::exit(1);
			}
		}
		if( name_vec[i] == "ode_step_size" )
		{	bool ok = std::atof( argument_value[match].c_str() ) > 0.0;
			if( ! ok )
			{	std::cerr
				<< "argument table: argument_id = " << match << std::endl
				<< "argument_value is <= 0.0 for ode_step_size" << std::endl;
				std::exit(1);
			}
		}
		if( name_vec[i] == "random_seed" )
		{	bool ok = std::atoi( argument_value[match].c_str() ) >= 0;
			if( ! ok )
			{	std::cerr
				<< "argument table: argument_id = " << match << std::endl
				<< "argument_value is < 0 for random_seed" << std::endl;
				std::exit(1);
			}
		}
		if( name_vec[i] == "number_sample" )
		{	bool ok = std::atoi( argument_value[match].c_str() ) >= 1;
			if( ! ok )
			{	std::cerr
				<< "argument table: argument_id = " << match << std::endl
				<< "argument_value is < 1 for number_sample" << std::endl;
				std::exit(1);
			}
		}
	}
	//
	// return table
	CppAD::vector<argument_struct> argument_table(n_argument);
	for(size_t i = 0; i < n_argument; i++)
	{	argument_table[i].argument_name  = argument_name[i];
		argument_table[i].argument_value = argument_value[i];
	}
	return argument_table;
}

} // END DISMOD_AT_NAMESPACE
