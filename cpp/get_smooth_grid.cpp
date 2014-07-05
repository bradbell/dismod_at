/*
$begin get_smooth_grid$$
$spell
	sqlite
	struct
	cpp
	std
	dage
	dtime
$$

$section C++: Get the Smoothing Grid Information$$
$index get, smooth grid$$
$index smooth, get grid$$
$index grid, get smooth$$

$head Syntax$$
$codei # include <get_smooth_grid>
%$$
$icode%smooth_grid% = get_smooth_grid(%db%)%$$

$head Purpose$$
To read the $cref smooth_grid$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head smooth_grid_struct$$
This is a structure with the following fields
$table
$code int smooth_id$$   $pre  $$ $cnext 
	The $cref/smooth_id/smooth_grid/smooth_id/$$ for this smoothing. 
$rnext
$code double age$$      $pre  $$ $cnext 
	The $cref/age/smooth_grid/age/$$ for this grid point
$rnext
$code double time$$      $pre  $$ $cnext 
	The $cref/time/smooth_grid/time/$$ for this grid point
$rnext
$code int value_like_id$$    $pre  $$ $cnext 
	The $cref/value_like_id/smooth_grid/value_like_id/$$
	for this smoothing, age, and time.
$rnext
$code int dage_like_id$$    $pre  $$ $cnext 
	The $cref/dage_like_id/smooth_grid/dage_like_id/$$
	for this smoothing, age, and time.
$rnext
$code int dtime_like_id$$    $pre  $$ $cnext 
	The $cref/dtime_like_id/smooth_grid/dtime_like_id/$$
	for this smoothing, age, and time.
$tend        

$head smooth_grid$$
The return value $icode smooth_grid$$ has prototype
$codei%
	CppAD::vector<smooth_grid_struct>  %smooth_grid%
%$$
For each $cref/smooth_grid_id/smooth_grid/smooth_grid_id/$$,
$codei%
	%smooth_grid%[%smooth_grid_id%]
%$$
is the information for a specific smoothing and specific age and time.

$children%example/cpp/get_smooth_grid_xam.cpp
%$$
$head Example$$
The file $cref get_smooth_grid_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cmath>
# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_smooth_grid.hpp>

namespace {
	void error_exit(size_t row, std::string msg)
	{	using std::cerr;
		using std::endl;
		cerr << msg << endl;
		cerr << "Error detected in smooth grid";
		if( row > 0 )
			cerr << " at row " << row;
		cerr << "." << endl;
		exit(1);
	}
}

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<smooth_grid_struct> get_smooth_grid(sqlite3* db)
{	using std::string;

	string table_name  = "smooth_grid";
	string column_name = "smooth_grid_id";
	CppAD::vector<int>    smooth_grid_id;
	get_table_column(db, table_name, column_name, smooth_grid_id);
	size_t n_smooth = smooth_grid_id.size();

	column_name        =  "smooth_id";
	CppAD::vector<int>     smooth_id;
	get_table_column(db, table_name, column_name, smooth_id);
	assert( smooth_id.size() == n_smooth );

	column_name        =  "age";
	CppAD::vector<double>  age;
	get_table_column(db, table_name, column_name, age);
	assert( age.size() == n_smooth );

	column_name        =  "time";
	CppAD::vector<double>  time;
	get_table_column(db, table_name, column_name, time);
	assert( time.size() == n_smooth );

	column_name        =  "value_like_id";
	CppAD::vector<int>     value_like_id;
	get_table_column(db, table_name, column_name, value_like_id);
	assert( value_like_id.size() == n_smooth );


	column_name        =  "dage_like_id";
	CppAD::vector<int>     dage_like_id;
	get_table_column(db, table_name, column_name, dage_like_id);
	assert( dage_like_id.size() == n_smooth );

	column_name        =  "dtime_like_id";
	CppAD::vector<int>     dtime_like_id;
	get_table_column(db, table_name, column_name, dtime_like_id);
	assert( dtime_like_id.size() == n_smooth );

	CppAD::vector<smooth_grid_struct> smooth_grid(n_smooth);
	for(size_t i = 0; i < n_smooth; i++)
	{	if( smooth_grid_id[i] != i )
		{	string s = 
			"smooth_grid_id must start at zero and increment by one.";
			error_exit(i+1, s);
		}
		smooth_grid[i].smooth_id      = smooth_id[i];
		smooth_grid[i].age            = age[i];
		smooth_grid[i].time           = time[i];
		smooth_grid[i].value_like_id  = value_like_id[i];
		smooth_grid[i].dage_like_id   = dage_like_id[i];
		smooth_grid[i].dtime_like_id  = dtime_like_id[i];
	}
	return smooth_grid;
}

} // END DISMOD_AT_NAMESPACE
