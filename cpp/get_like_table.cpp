/*
$begin get_like_table$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Node Table Information$$
$index get, like table$$
$index like, get table$$
$index table, get like$$

$head Syntax$$
$codei # include <get_like_table>
%$$
$icode%like_table% = get_like_table(%db%)%$$

$head Purpose$$
To read the $cref like_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head like_struct$$
This is a structure with the following fields
$table
$code std::string name$$   $pre  $$ $cnext 
	The $cref/like_name/like_table/like_name/$$ for this likelihood  $rnext
$code int density_id$$     $pre  $$ $cnext 
	The $cref/density_id/density_table/density_id/$$ for this likelihood $rnext
$code double lower$$       $pre  $$ $cnext
	The $cref/lower/like_table/lower/$$ limit for this likelihood  $rnext
$code double upper$$        $pre  $$ $cnext
	The $cref/upper/like_table/upper/$$ limit for this likelihood  $rnext
$code double mean$$         $pre  $$ $cnext
	The $cref/mean/like_table/mean/$$ for this likelihood  $rnext
$code double std$$          $pre  $$ $cnext
	The $cref/std/like_table/std/$$ for this likelihood  $rnext
$code double eta$$          $pre  $$ $cnext
	The $cref/eta/like_table/eta/$$ for this likelihood 
$tend        

$head like_table$$
The return value $icode like_table$$ has prototype
$codei%
	CppAD::vector<like_struct>  %like_table%
%$$
For each $cref/like_id/like_table/like_id/$$,
$codei%
	%like_table%[%like_id%]
%$$
is the information for the corresponding likelihood.

$children%example/cpp/get_like_table_xam.cpp
%$$
$head Example$$
The file $cref get_like_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cmath>
# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_like_table.hpp>

namespace {
	void error_exit(size_t row, std::string msg)
	{	using std::cerr;
		using std::endl;
		cerr << msg << endl;
		cerr << "Error detected in like table";
		if( row > 0 )
			cerr << " at row " << row;
		cerr << "." << endl;
		exit(1);
	}
}

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<like_struct> get_like_table(sqlite3* db)
{	using std::string;

	string table_name  = "like";
	string column_name = "like_id";
	CppAD::vector<int>    like_id;
	get_table_column(db, table_name, column_name, like_id);
	size_t n_like = like_id.size();

	column_name        =  "like_name";
	CppAD::vector<string>  like_name;
	get_table_column(db, table_name, column_name, like_name);
	assert( like_name.size() == n_like );

	column_name        =  "density_id";
	CppAD::vector<int>     density_id;
	get_table_column(db, table_name, column_name, density_id);
	assert( density_id.size() == n_like );

	column_name        =  "lower";
	CppAD::vector<double>  lower;
	get_table_column(db, table_name, column_name, lower);
	assert( lower.size() == n_like );

	column_name        =  "upper";
	CppAD::vector<double>  upper;
	get_table_column(db, table_name, column_name, upper);
	assert( upper.size() == n_like );

	column_name        =  "mean";
	CppAD::vector<double>  mean;
	get_table_column(db, table_name, column_name, mean);
	assert( mean.size() == n_like );

	column_name        =  "std";
	CppAD::vector<double>  std;
	get_table_column(db, table_name, column_name, std);
	assert( std.size() == n_like );

	column_name        =  "eta";
	CppAD::vector<double>  eta;
	get_table_column(db, table_name, column_name, eta);
	assert( eta.size() == n_like );

	double minus_infinity = std::atof("-inf");
	double plus_infinity  = std::atof("+inf");
	CppAD::vector<like_struct> like_table(n_like);
	for(size_t i = 0; i < n_like; i++)
	{	if( like_id[i] != i )
		{	string s = "like_id must start at zero and increment by one.";
			error_exit(i+1, s);
		}
		like_table[i].name       = like_name[i];
		like_table[i].density_id = density_id[i];
		//
		like_table[i].lower      = lower[i];
		if( std::isnan( lower[i] ) )
			like_table[i].lower      = minus_infinity;
		//
		like_table[i].upper      = upper[i];
		if( std::isnan( upper[i] ) )
			like_table[i].upper      = plus_infinity;
		//
		like_table[i].mean       = mean[i];
		like_table[i].std        = std[i];
		like_table[i].eta        = eta[i];
	}
	return like_table;
}

} // END DISMOD_AT_NAMESPACE
