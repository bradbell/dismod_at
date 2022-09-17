// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_prior_table$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Prior Table Information$$

$head Syntax$$
$icode%prior_table% = get_prior_table(%db%, %density_table%)%$$

$head Prototype$$
$srcthisfile%
	0%// BEGIN_GET_PRIOR_TABLE%// END_GET_PRIOR_TABLE%1
%$$

$head Purpose$$
To read the $cref prior_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$
is an open connection to the database.

$head prior_table$$
For each $cref/prior_id/prior_table/prior_id/$$,
$codei%
	%prior_table%[%prior_id%]
%$$
is the information for the corresponding prior.

$head density_table$$
The $cref density_table$$ is used to check for errors in the prior table;
e.g., the standard deviation for a prior must be positive unless the
density is uniform.

$head prior_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code std::string$$ $cnext $code prior_name$$   $cnext
	The $cref/prior_name/prior_table/prior_name/$$ for this prior
$rnext
$code int$$ $cnext $code density_id$$  $cnext
	The $cref/density_id/prior_table/density_id/$$ for this prior
$rnext
$code double$$ $cnext $code lower$$ $cnext
	The $cref/lower/prior_table/lower/$$ limit for this prior
$rnext
$code double$$ $cnext $code upper$$         $cnext
	The $cref/upper/prior_table/upper/$$ limit for this prior
$rnext
$code double$$ $cnext $code mean$$          $cnext
	The $cref/mean/prior_table/mean/$$ for this prior
$rnext
$code double$$ $cnext $code std$$           $cnext
	The $cref/std/prior_table/std/$$ for this prior
$rnext
$code double$$ $cnext $code eta$$           $cnext
	The $cref/eta/prior_table/eta/$$ for this prior
$rnext
$code double$$ $cnext $code nu$$            $cnext
	The $cref/nu/prior_table/nu/$$ for this prior
$tend

$head Checks$$
The values in the $cref prior_table$$ are checked to make sure that:
$list number$$
The limits and mean satisfy $icode%lower% <= %mean% <= %upper%$$.
$lnext
The standard deviation $icode%std% > 0%$$
(except for the uniform density case).
$lnext
In the log density cases $icode eta$$ is not null
and $icode%eta% > 0%$$.
$lnext
In the Student density cases $icode nu$$ is not null
and $icode%nu% > 2%$$.
$lend

$children%example/devel/table/get_prior_table_xam.cpp
%$$
$head Example$$
The file $cref get_prior_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/
# include <cmath>
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/get_density_table.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_GET_PRIOR_TABLE
CppAD::vector<prior_struct> get_prior_table(
	sqlite3*                           db            ,
	const CppAD::vector<density_enum>& density_table )
// END_GET_PRIOR_TABLE
{	using std::string;

	// user for error messaging
	string msg;

	string table_name  = "prior";
	size_t n_prior      = check_table_id(db, table_name);

	string column_name =  "prior_name";
	CppAD::vector<string>  prior_name;
	get_table_column(db, table_name, column_name, prior_name);
	assert( prior_name.size() == n_prior );

	column_name        =  "density_id";
	CppAD::vector<int>     density_id;
	get_table_column(db, table_name, column_name, density_id);
	assert( density_id.size() == n_prior );

	column_name        =  "lower";
	CppAD::vector<double>  lower;
	get_table_column(db, table_name, column_name, lower);
	assert( lower.size() == n_prior );

	column_name        =  "upper";
	CppAD::vector<double>  upper;
	get_table_column(db, table_name, column_name, upper);
	assert( upper.size() == n_prior );

	column_name        =  "mean";
	CppAD::vector<double>  mean;
	get_table_column(db, table_name, column_name, mean);
	assert( mean.size() == n_prior );

	column_name        =  "std";
	CppAD::vector<double>  std;
	get_table_column(db, table_name, column_name, std);
	assert( std.size() == n_prior );

	column_name        =  "eta";
	CppAD::vector<double>  eta;
	get_table_column(db, table_name, column_name, eta);
	assert( eta.size() == n_prior );

	column_name        =  "nu";
	CppAD::vector<double>  nu;
	get_table_column(db, table_name, column_name, nu);
	assert( nu.size() == n_prior );

	double minus_infinity = std::atof("-inf");
	double plus_infinity  = std::atof("+inf");
	CppAD::vector<prior_struct> prior_table(n_prior);
	bool limit_warning = false;
	for(size_t i = 0; i < n_prior; i++)
	{	prior_table[i].prior_name  = prior_name[i];
		prior_table[i].density_id = density_id[i];
		//
		size_t number_limit = 0;
		//
		prior_table[i].lower      = lower[i];
		if( std::isnan( lower[i] ) )
			prior_table[i].lower      = minus_infinity;
		else
			++number_limit;
		//
		prior_table[i].upper      = upper[i];
		if( std::isnan( upper[i] ) )
			prior_table[i].upper      = plus_infinity;
		else
			++number_limit;
		//
		if( (number_limit == 1) & (! limit_warning) )
		{	msg = "A lower or upper limit is null (infinite) but not both.\n"
				"This can lead to problems in Ipopt initialization.\n"
				"It is currently a warning but will be changed to an error.";
			log_message(db, &std::cerr, "warning", msg, table_name, i);
			limit_warning = true;
		}
		//
		prior_table[i].mean       = mean[i];
		prior_table[i].std        = std[i];
		prior_table[i].eta        = eta[i];
		prior_table[i].nu         = nu[i];
		//
		// check values using ok so that nan returns correct result
		bool ok = mean[i] <= prior_table[i].upper;
		if(	! ok )
		{	msg = "mean greater than upper limit";
			error_exit(msg, table_name, i);
		}
		ok = prior_table[i].lower <= mean[i];
		if(	! ok )
		{	msg = "mean less than lower limit";
			error_exit(msg, table_name, i);
		}
		ok = density_table[density_id[i]] == uniform_enum || std[i] > 0.0;
		if( ! ok )
		{	msg = "std <= 0 and density is not uniform";
			error_exit(msg, table_name, i);
		}
		switch( density_table[density_id[i]]  )
		{	case log_gaussian_enum:
			case log_laplace_enum:
			case log_students_enum:
			ok = eta[i] > 0.0;
			break;

			default:
			ok = true;
		}
		if( ! ok )
		{	msg = "This is a log density and eta not greater than zero.";
			error_exit(msg, table_name, i);
		}
		switch( density_table[density_id[i]] )
		{	case students_enum:
			case log_students_enum:
			ok = nu[i] > 2.0;
			break;

			default:
			ok = true;
		}
		if( ! ok )
		{	msg = "This is a Student's density and nu not greater than two.";
			error_exit(msg, table_name, i);
		}
	}
	return prior_table;
}

} // END DISMOD_AT_NAMESPACE
