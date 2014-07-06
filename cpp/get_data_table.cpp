// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_data_table$$
$spell
	covariate
	covariates
	sqlite
	struct
	cpp
	std
	data
$$

$section C++: Get the Data Table Information$$
$index get, data table$$
$index data, get table$$
$index table, get data$$

$head Syntax$$
$codei%# include <get_data_table>
%$$
$icode%data_table% = get_data_table(%db%, %n_covariate%)%$$

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
	size_t n_covariate
%$$
It is the number of covariates; i.e., the number of rows
in the $cref covariate_table$$.
It is also the number of columns in the data table that 
have column names beginning with $code x_$$.

$head data_struct$$
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
$code double$$ $cnext $code meas_value$$ $cnext 
	The $cref/meas_value/data_table/meas_value/$$ for this measurement
$rnext
$code double$$ $cnext $code meas_std$$ $cnext 
	The $cref/meas_std/data_table/meas_std/$$ for this measurement
$rnext
$code double$$ $cnext $code meas_eta$$ $cnext 
	The $cref/meas_eta/data_table/meas_eta/$$ for this measurement
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
$rnext
$code CppAD::vector<double>$$ $cnext $code covariate$$ $cnext
	The $cref/covariate/data_table/Covariates/$$ values for this measurement
$tend        

$head data_table$$
The return value $icode data_table$$ has prototype
$codei%
	CppAD::vector<data_struct>  %data_table%
%$$
For each $cref/data_id/data_table/data_id/$$,
$codei%
	%data_table%[%data_id%]
%$$
is the information for the corresponding data.

$subhead x$$
The value
$codei%
	%data_table%[%data_id%].x[%covariate_id%]
%$$
is the value of the covariate specified by
$cref/covariate_id/covariate_table/covariate_id/$$
and the measurement specified by $icode data_id$$.

$children%example/cpp/get_data_table_xam.cpp
%$$
$head Example$$
The file $cref get_data_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/table_error_exit.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<data_struct> get_data_table(sqlite3* db, size_t n_covariate)
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

	column_name           =  "meas_eta";
	CppAD::vector<double>     meas_eta;
	get_table_column(db, table_name, column_name, meas_eta);
	assert( n_data == meas_eta.size() );

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

	// fill in all but the covariate values
	CppAD::vector<data_struct> data_table(n_data);
	for(size_t i = 0; i < n_data; i++)
	{	data_table[i].integrand_id  = integrand_id[i];
		data_table[i].density_id    = density_id[i];
		data_table[i].node_id       = node_id[i];
		data_table[i].weight_id     = weight_id[i];
		data_table[i].meas_value    = meas_value[i];
		data_table[i].meas_std      = meas_std[i];
		data_table[i].meas_eta      = meas_eta[i];
		data_table[i].age_lower     = age_lower[i];
		data_table[i].age_upper     = age_upper[i];
		data_table[i].time_lower    = time_lower[i];
		data_table[i].time_upper    = time_upper[i];

		// alocate memory for covariates
		data_table[i].x.resize(n_covariate);
	}

	// now get the covariate values
	for(size_t j = 0; j < n_covariate; j++)
	{	std::stringstream ss;
		ss << "x_" << j;
		column_name = ss.str();
		CppAD::vector<double> x_j;
		get_table_column(db, table_name, column_name, x_j);
		for(size_t i = 0; i < n_data; i++)
			data_table[i].x[j] = x_j[i];	
	}
	return data_table;
}

} // END DISMOD_AT_NAMESPACE
