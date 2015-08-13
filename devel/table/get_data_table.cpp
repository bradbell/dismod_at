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
$begin get_data_table$$
$spell
	const
	covariate
	covariates
	sqlite
	struct
	cpp
	std
	data
$$

$section C++: Get the Data Table Information$$

$head Syntax$$
$icode%get_data_table(
	%db%, %n_covariate%, %age_min%, %age_max%, %time_min%, %time_max%,
	%data_table%, %covariate_value%
)%$$

$head Purpose$$
To read the $cref data_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head n_covariate$$
This argument has prototype
$codei%
	size_t %n_covariate%
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

$head data_table$$
This argument has prototype
$codei%
	CppAD::vector<data_struct>&  %data_table%
%$$
On input its size is zero and upon return it has one element for
each row in the data table.
For each $cref/data_id/data_table/data_id/$$,
$codei%
	%data_table%[%data_id%]
%$$
is the $code data_struct$$ information for the corresponding data.

$subhead data_struct$$
This is a structure with the following fields
$table
Type  $cnext Field $cnext Description
$rnext
$code int$$ $cnext $code integrand_id$$ $cnext
	The $cref/integrand_id/data_table/integrand_id/$$ for this measurement
$rnext
$code int$$ $cnext $code density_id$$ $cnext
	The $cref/density_id/data_table/density_id/$$ for this measurement
$rnext
$code int$$ $cnext $code node_id$$ $cnext
	The $cref/node_id/data_table/node_id/$$ for this measurement
$rnext
$code int$$ $cnext $code weight_id$$ $cnext
	The $cref/weight_id/data_table/weight_id/$$ for this measurement
$rnext
$code int$$ $cnext $code hold_out$$ $cnext
	The $cref/hold_out/data_table/hold_out/$$ for this measurement
$rnext
$code double$$ $cnext $code meas_value$$ $cnext
	The $cref/meas_value/data_table/meas_value/$$ for this measurement
$rnext
$code double$$ $cnext $code meas_std$$ $cnext
	The $cref/meas_std/data_table/meas_std/$$ for this measurement
$rnext
$code double$$ $cnext $code age_lower$$ $cnext
	The $cref/age_lower/data_table/age_lower/$$ for this measurement
$rnext
$code double$$ $cnext $code age_upper$$ $cnext
	The $cref/age_upper/data_table/age_upper/$$ for this measurement
$rnext
$code double$$ $cnext $code time_lower$$ $cnext
	The $cref/time_lower/data_table/time_lower/$$ for this measurement
$rnext
$code double$$ $cnext $code time_upper$$ $cnext
	The $cref/time_upper/data_table/time_upper/$$ for this measurement
$tend

$head covariate_value$$
This argument has prototype
$codei%
	CppAD::vector<double>&  %covariate_value%
%$$
On input its size is zero.
Upon return, its size is the number of data points times
the number of covariates.
For each
$cref/covariate_id/covariate_table/covariate_id/$$ and $icode data_id$$ pair
$codei%
	covariate_value[%data_id% * %n_covariate% + %covariate_id%]
%$$
is the corresponding covariate value.

$children%example/devel/table/get_data_table_xam.cpp
%$$
$head Example$$
The file $cref get_data_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_density_table.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void get_data_table(
	sqlite3* db                                   ,
	size_t                      n_covariate       ,
	double                      age_min           ,
	double                      age_max           ,
	double                      time_min          ,
	double                      time_max          ,
	CppAD::vector<data_struct>& data_table        ,
	CppAD::vector<double>&      covariate_value   )
{	using std::string;
	// TODO: This could be more efficient if we only allcated one temporary
	// column at a time (to use with get_table column

	string table_name  = "data";
	size_t n_data      = check_table_id(db, table_name);

	string column_name = "integrand_id";
	CppAD::vector<int>    integrand_id;
	get_table_column(db, table_name, column_name, integrand_id);
	assert( n_data == integrand_id.size() );

	column_name        = "density_id";
	CppAD::vector<int>    density_id;
	get_table_column(db, table_name, column_name, density_id);
	assert( n_data == density_id.size() );

	column_name        = "hold_out";
	CppAD::vector<int>    hold_out;
	get_table_column(db, table_name, column_name, hold_out);
	assert( n_data == hold_out.size() );

	column_name        = "node_id";
	CppAD::vector<int>    node_id;
	get_table_column(db, table_name, column_name, node_id);
	assert( n_data == node_id.size() );

	column_name        = "weight_id";
	CppAD::vector<int>    weight_id;
	get_table_column(db, table_name, column_name, weight_id);
	assert( n_data == weight_id.size() );

	column_name           =  "meas_value";
	CppAD::vector<double>     meas_value;
	get_table_column(db, table_name, column_name, meas_value);
	assert( n_data == meas_value.size() );

	column_name           =  "meas_std";
	CppAD::vector<double>     meas_std;
	get_table_column(db, table_name, column_name, meas_std);
	assert( n_data == meas_std.size() );

	column_name           =  "age_lower";
	CppAD::vector<double>     age_lower;
	get_table_column(db, table_name, column_name, age_lower);
	assert( n_data == age_lower.size() );

	column_name           =  "age_upper";
	CppAD::vector<double>     age_upper;
	get_table_column(db, table_name, column_name, age_upper);
	assert( n_data == age_upper.size() );

	column_name           =  "time_lower";
	CppAD::vector<double>     time_lower;
	get_table_column(db, table_name, column_name, time_lower);
	assert( n_data == time_lower.size() );

	column_name           =  "time_upper";
	CppAD::vector<double>     time_upper;
	get_table_column(db, table_name, column_name, time_upper);
	assert( n_data == time_upper.size() );

	// fill in the data table
	assert( data_table.size() == 0 );
	data_table.resize(n_data);
	for(size_t i = 0; i < n_data; i++)
	{	data_table[i].integrand_id  = integrand_id[i];
		data_table[i].density_id    = density_id[i];
		data_table[i].node_id       = node_id[i];
		data_table[i].weight_id     = weight_id[i];
		data_table[i].hold_out      = hold_out[i];
		data_table[i].meas_value    = meas_value[i];
		data_table[i].meas_std      = meas_std[i];
		data_table[i].age_lower     = age_lower[i];
		data_table[i].age_upper     = age_upper[i];
		data_table[i].time_lower    = time_lower[i];
		data_table[i].time_upper    = time_upper[i];
	}

	// now get the covariates
	assert( covariate_value.size() == 0 );
	covariate_value.resize(n_data * n_covariate );
	for(size_t j = 0; j < n_covariate; j++)
	{	std::stringstream ss;
		ss << "x_" << j;
		column_name = ss.str();
		CppAD::vector<double> x_j;
		get_table_column(db, table_name, column_name, x_j);
		for(size_t i = 0; i < n_data; i++)
			covariate_value[ i * n_covariate + j ] = x_j[i];
	}

	// check for erorr conditions
	string msg;
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	int hold_out = data_table[data_id].hold_out;
		if( hold_out != 0 && hold_out != 1 )
		{	msg = "hold_out is not equal to zero or one";
			error_exit(db, msg, table_name, data_id);
		}
		// -------------------------------------------------------------
		double age_lower  = data_table[data_id].age_lower;
		double age_upper  = data_table[data_id].age_upper;
		if( age_lower < age_min )
		{	msg = "age_lower is less than minimum age in age table";
			error_exit(db, msg, table_name, data_id);
		}
		if( age_max < age_upper )
		{	msg = "age_upper is greater than maximum age in age table";
			error_exit(db, msg, table_name, data_id);
		}
		if( age_upper < age_lower )
		{	msg = "age_lower is greater than age_upper";
			error_exit(db, msg, table_name, data_id);
		}
		// ------------------------------------------------------------
		double time_lower = data_table[data_id].time_lower;
		double time_upper = data_table[data_id].time_upper;
		if( time_lower < time_min )
		{	msg = "time_lower is less than minimum time in time table";
			error_exit(db, msg, table_name, data_id);
		}
		if( time_max < time_upper )
		{	msg = "time_upper is greater than maximum time in time table";
			error_exit(db, msg, table_name, data_id);
		}
		if( time_upper < time_lower )
		{	msg = "time_lower is greater than time_upper";
			error_exit(db, msg, table_name, data_id);
		}
		int density_id = data_table[data_id].density_id;
		if( density_enum( density_id ) == uniform_enum )
		{	msg = "density_id corresponds to the uniform distribution";
			error_exit(db, msg, table_name, data_id);
		}
		double meas_std = data_table[data_id].meas_std;
		if( meas_std <= 0.0 )
		{	msg = "meas_std is less than or equal zero";
			error_exit(db, msg, table_name, data_id);
		}
	}
	return;
}

} // END DISMOD_AT_NAMESPACE
