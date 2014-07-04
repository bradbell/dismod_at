/*
$begin get_density_table$$
$spell
	sqlite
	enum
	cpp
$$

$section C++: Get the Density Table Information$$
$index get, density table$$
$index density, get table$$
$index table, get density$$

$head Syntax$$
$codei # include <get_density_table>
%$$
$icode%density_table% = get_density_table(%db%)%$$

$head Purpose$$
To read the $cref density_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head density_enum$$
This is an enum type with the following values:
$table
$icode density$$        $pre  $$ $cnext Corresponding String    $rnext
$code gaussian_enum$$    $pre  $$ $cnext $code "gaussian"$$          $rnext        
$code laplace_enum$$     $pre  $$ $cnext $code "laplace"$$           $rnext        
$code log_gaussian_enum$$     $pre  $$ $cnext $code "log_gaussian"$$           $rnext        
$code log_laplace_enum$$   $pre  $$ $cnext $code "log_laplace"$$
$tend        
The number of these enum values is $code number_density_enum$$.

$head density_enum2name$$
This is a global variable. If $icode%density% < number_density_enum%$$, 
$codei%density_enum2name[%density%]%$$ is the string corresponding
to the $icode density$$ enum value.

$head density_table$$
The return value $icode density_table$$ has prototype
$codei%
	CppAD::vector<density_enum>  %density_table%
%$$
For each $cref/density_id/density_table/density_id/$$,
$codei%
	%density_table%[%density_id%]
%$$
is the enum value for the corresponding
$cref/density_name/density_table/density_name/$$.


$children%example/cpp/get_density_table_xam.cpp
%$$
$head Example$$
The file $cref get_density_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_density_table.hpp>

namespace {
	void error_exit(size_t row, std::string msg)
	{	using std::cerr;
		using std::endl;
		cerr << msg << endl;
		cerr << "Error detected in density table";
		if( row > 0 )
			cerr << " at row " << row;
		cerr << "." << endl;
		exit(1);
	}
}

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// density names in same order as enum type in get_density_table.hpp
const char* density_enum2name[] = {
	"gaussian",
	"laplace",
	"log_gaussian",
	"log_laplace"
};

CppAD::vector<density_enum> get_density_table(sqlite3* db)
{	using std::string;

	string table_name  = "density";
	string column_name = "density_id";
	CppAD::vector<int>  density_id;
	get_table_column(db, table_name, column_name, density_id);
	
	column_name        =  "density_name";
	CppAD::vector<string>  density_name;
	get_table_column(db, table_name, column_name, density_name);

	if( density_id.size() != size_t( number_density_enum ) )
	{	std::stringstream ss;
		ss << "density table does not have ";
		ss << size_t( number_density_enum) << " rows.";
		error_exit(0, ss.str());
	}
	assert( density_id.size() == density_name.size() );

	CppAD::vector<density_enum> density_table(number_density_enum);
	for(size_t i = 0; i < number_density_enum; i++)
	{	if( density_id[i] != i )
		{	string s = "density_id must start at zero and increment by one.";
			error_exit(i+1, s);
		}
		bool found = false;
		for( size_t j = 0; j < number_density_enum; j++)
		{	if( density_enum2name[j] == density_name[i] )
			{	density_table[i] = density_enum(j);
				found         = true;
			}
		}
		if( ! found )
		{	string s = "density_name is not iota, rho, chi, or omega.";
			error_exit(i+1, s);
		}
	}
	return density_table;
}

} // END DISMOD_AT_NAMESPACE
