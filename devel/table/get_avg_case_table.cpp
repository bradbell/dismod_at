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
$begin get_avg_case_table$$
$spell
	const
	covariate
	covariates
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Average Integrand Case Table$$

$head Syntax$$
$icode%avg_case_table% = get_avg_case_table(
	%db%, %n_covariate%, %age_min%, %age_max%, %time_min%, %time_max%
%)%$$

$head Purpose$$
To read the $cref avg_case_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head n_covariate$$
This argument has prototype
$codei%
	size_t n_covariate
%$$
and is the size of the $cref/covariate_table/get_covariate_table/$$.

$head age_min$$
This argument has prototype
$codei%
	double %age_min%
%$$
and is the minimum value in the $cref age_table$$.
If an $cref/age_lower/data_table/age_lower/$$ value is less than
$icode age_min$$, an error is reported.

$head age_max$$
This argument has prototype
$codei%
	double %age_max%
%$$
and is the maximum value in the $cref age_table$$.
If an $cref/age_upper/data_table/age_upper/$$ value is greater than
$icode age_max$$, an error is reported.
The condition
$icode%
	%age_lower% <= %age_upper%
%$$
is also checked.

$head time_min$$
This argument has prototype
$codei%
	double %time_min%
%$$
and is the minimum value in the $cref time_table$$.
If an $cref/time_lower/data_table/time_lower/$$ value is less than
$icode time_min$$, an error is reported.

$head time_max$$
This argument has prototype
$codei%
	double %time_max%
%$$
and is the maximum value in the $cref time_table$$.
If an $cref/time_upper/data_table/time_upper/$$ value is greater than
$icode time_max$$, an error is reported.
The condition
$icode%
	%time_lower% <= %time_upper%
%$$
is also checked.

$head avg_case_table$$
The return value $icode avg_case_table$$ has prototype
$codei%
	CppAD::vector<avg_case_struct>  %avg_case_table%
%$$
For each $cref/avg_case_id/avg_case_table/avg_case_id/$$,
$codei%
	%avg_case_table%[%avg_case_id%]
%$$
is the information for the corresponding data.

$subhead x$$
The value
$codei%
	%avg_case_table%[%avg_case_id%].x[%covariate_id%]
%$$
is the value of the covariate corresponding to the
$cref/covariate_id/covariate_table/covariate_id/$$
and the $icode avg_case_id$$.

$head avg_case_struct$$
This is a structure with the following fields
$table
Type  $cnext Field $cnext Description
$rnext
$code int$$ $cnext $code integrand_id$$ $cnext
	The $cref/integrand_id/avg_case_table/integrand_id/$$ for this measurement
$rnext
$code int$$ $cnext $code node_id$$ $cnext
	The $cref/node_id/avg_case_table/node_id/$$ for this measurement
$rnext
$code int$$ $cnext $code weight_id$$ $cnext
	The $cref/weight_id/avg_case_table/weight_id/$$ for this measurement
$rnext
$code double$$ $cnext $code age_lower$$ $cnext
	The $cref/age_lower/avg_case_table/age_lower/$$ for this measurement
$rnext
$code double$$ $cnext $code age_upper$$ $cnext
	The $cref/age_upper/avg_case_table/age_upper/$$ for this measurement
$rnext
$code double$$ $cnext $code time_lower$$ $cnext
	The $cref/time_lower/avg_case_table/time_lower/$$ for this measurement
$rnext
$code double$$ $cnext $code time_upper$$ $cnext
	The $cref/time_upper/avg_case_table/time_upper/$$ for this measurement
$rnext
$code CppAD::vector<double>$$ $pre  $$ $cnext $code x$$ $cnext
	The $cref/covariate/avg_case_table/Covariates/$$ values for this measurement
$tend

$children%example/devel/table/get_avg_case_table_xam.cpp
%$$
$head Example$$
The file $cref get_avg_case_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/get_avg_case_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/table_error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<avg_case_struct> get_avg_case_table(
	sqlite3* db        ,
	size_t n_covariate ,
	double age_min     ,
	double age_max     ,
	double time_min    ,
	double time_max    )
{	using std::string;
	// TODO: This could be more efficient if we only allcated one temporary
	// column at a time (to use with get_table column)

	string table_name  = "avg_case";
	size_t n_avg_case      = check_table_id(db, table_name);

	string column_name = "integrand_id";
	CppAD::vector<int>    integrand_id;
	get_table_column(db, table_name, column_name, integrand_id);
	assert( n_avg_case == integrand_id.size() );

	column_name        = "node_id";
	CppAD::vector<int>    node_id;
	get_table_column(db, table_name, column_name, node_id);
	assert( n_avg_case == node_id.size() );

	column_name        = "weight_id";
	CppAD::vector<int>    weight_id;
	get_table_column(db, table_name, column_name, weight_id);
	assert( n_avg_case == weight_id.size() );

	column_name           =  "age_lower";
	CppAD::vector<double>     age_lower;
	get_table_column(db, table_name, column_name, age_lower);
	assert( n_avg_case == age_lower.size() );

	column_name           =  "age_upper";
	CppAD::vector<double>     age_upper;
	get_table_column(db, table_name, column_name, age_upper);
	assert( n_avg_case == age_upper.size() );

	column_name           =  "time_lower";
	CppAD::vector<double>     time_lower;
	get_table_column(db, table_name, column_name, time_lower);
	assert( n_avg_case == time_lower.size() );

	column_name           =  "time_upper";
	CppAD::vector<double>     time_upper;
	get_table_column(db, table_name, column_name, time_upper);
	assert( n_avg_case == time_upper.size() );

	// fill in all but the covariate values
	CppAD::vector<avg_case_struct> avg_case_table(n_avg_case);
	for(size_t i = 0; i < n_avg_case; i++)
	{	avg_case_table[i].integrand_id  = integrand_id[i];
		avg_case_table[i].node_id       = node_id[i];
		avg_case_table[i].weight_id     = weight_id[i];
		avg_case_table[i].age_lower     = age_lower[i];
		avg_case_table[i].age_upper     = age_upper[i];
		avg_case_table[i].time_lower    = time_lower[i];
		avg_case_table[i].time_upper    = time_upper[i];

		// alocate memory for covariates
		avg_case_table[i].x.resize(n_covariate);
	}

	// now get the covariate values
	for(size_t j = 0; j < n_covariate; j++)
	{	std::stringstream ss;
		ss << "x_" << j;
		column_name = ss.str();
		CppAD::vector<double> x_j;
		get_table_column(db, table_name, column_name, x_j);
		for(size_t i = 0; i < n_avg_case; i++)
			avg_case_table[i].x[j] = x_j[i];
	}

	// check for erorr conditions
	string msg;
	for(size_t avg_case_id = 0; avg_case_id < n_avg_case; avg_case_id++)
	{	// -------------------------------------------------------------
		double age_lower  = avg_case_table[avg_case_id].age_lower;
		double age_upper  = avg_case_table[avg_case_id].age_upper;
		if( age_lower < age_min )
		{	msg = "age_lower is less than minimum age in age table";
			table_error_exit("avg_case", avg_case_id, msg);
		}
		if( age_max < age_upper )
		{	msg = "age_upper is greater than maximum age in age table";
			table_error_exit("avg_case", avg_case_id, msg);
		}
		if( age_upper < age_lower )
		{	msg = "age_lower is greater than age_upper";
			table_error_exit("avg_case", avg_case_id, msg);
		}
		// ------------------------------------------------------------
		double time_lower = avg_case_table[avg_case_id].time_lower;
		double time_upper = avg_case_table[avg_case_id].time_upper;
		if( time_lower < time_min )
		{	msg = "time_lower is less than minimum time in time table";
			table_error_exit("avg_case", avg_case_id, msg);
		}
		if( time_max < time_upper )
		{	msg = "time_upper is greater than maximum time in time table";
			table_error_exit("avg_case", avg_case_id, msg);
		}
		if( time_upper < time_lower )
		{	msg = "time_lower is greater than time_upper";
			table_error_exit("avg_case", avg_case_id, msg);
		}
	}
	return avg_case_table;
}

} // END DISMOD_AT_NAMESPACE
