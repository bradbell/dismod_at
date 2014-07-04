/*
$begin get_rate_table$$
$spell
	sqlite
	enum
	cpp
$$

$section C++: Get the Rate Table Information$$
$index get, rate table$$
$index rate, get table$$
$index table, get rate$$

$head Syntax$$
$codei # include <get_rate_table>
%$$
$icode%rate_table% = get_rate_table(%db%)%$$

$head Purpose$$
To read the $cref rate_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head rate_enum$$
This is an enum type with the following values:
$table
$icode rate$$        $pre  $$ $cnext Corresponding String    $rnext
$code iota_enum$$    $pre  $$ $cnext $code "iota"$$          $rnext        
$code rho_enum$$     $pre  $$ $cnext $code "rho"$$           $rnext        
$code chi_enum$$     $pre  $$ $cnext $code "chi"$$           $rnext        
$code omega_enum$$   $pre  $$ $cnext $code "omega"$$
$tend        
The number of these enum values is $code number_rate_enum$$.

$head rate_enum2name$$
This is a global variable. If $icode%rate% < number_rate_enum%$$, 
$codei%rate_enum2name[%rate%]%$$ is the string corresponding
to the $icode rate$$ enum value.

$head rate_table$$
The return value $icode rate_table$$ has prototype
$codei%
	CppAD::vector<rate_enum>  %rate_table%
%$$
For each $cref/rate_id/rate_table/rate_id/$$,
$codei%
	%rate_table%[%rate_id%]
%$$
is the enum value for the corresponding
$cref/rate_name/rate_table/rate_name/$$.


$children%example/cpp/get_rate_table_xam.cpp
%$$
$head Example$$
The file $cref get_rate_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_rate_table.hpp>

namespace {
	void error_exit(size_t row, std::string msg)
	{	using std::cerr;
		using std::endl;
		cerr << msg << endl;
		cerr << "Error detected in rate table";
		if( row > 0 )
			cerr << " at row " << row;
		cerr << "." << endl;
		exit(1);
	}
}

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// rate names in same order as enum type in get_rate_table.hpp
const char* rate_enum2name[] = {
	"iota",
	"rho",
	"chi",
	"omega"
};

CppAD::vector<rate_enum> get_rate_table(sqlite3* db)
{	using std::string;

	string table_name  = "rate";
	string column_name = "rate_id";
	CppAD::vector<int>  rate_id;
	get_table_column(db, table_name, column_name, rate_id);
	
	column_name        =  "rate_name";
	CppAD::vector<string>  rate_name;
	get_table_column(db, table_name, column_name, rate_name);

	if( rate_id.size() != size_t( number_rate_enum ) )
	{	std::stringstream ss;
		ss << "rate table does not have ";
		ss << size_t( number_rate_enum) << " rows.";
		error_exit(0, ss.str());
	}
	assert( rate_id.size() == rate_name.size() );

	CppAD::vector<rate_enum> rate_table(number_rate_enum);
	for(size_t i = 0; i < number_rate_enum; i++)
	{	if( rate_id[i] != i )
		{	string s = "rate_id must start at zero and increment by one.";
			error_exit(i+1, s);
		}
		bool found = false;
		for( size_t j = 0; j < number_rate_enum; j++)
		{	if( rate_enum2name[j] == rate_name[i] )
			{	rate_table[i] = rate_enum(j);
				found         = true;
			}
		}
		if( ! found )
		{	string s = "rate_name is not iota, rho, chi, or omega.";
			error_exit(i+1, s);
		}
	}
	return rate_table;
}

} // END DISMOD_AT_NAMESPACE
