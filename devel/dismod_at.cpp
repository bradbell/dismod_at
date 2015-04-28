// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin dismod_at_exe$$
$escape $$
$spell
	Dismod
	Ipopt
	num_iter
$$

$section The Dismod Age Time Executable$$

$head Syntax$$
$codei%dismod_at %database%$$
$icode%parent_node% %ode_step_size% %tolerance% %max_num_iter%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ and $code output$$ tables.
The input tables are not modified and result are written to the
output tables.

$head parent_node$$
If all of the characters in $icode parent_node$$ are decimal digits,
then  it specifies the parent
$cref/node_id/node_table/node_id/$$ for this fit.
Otherwise it specifies the parent
$cref/node_name/node_table/node_name/$$.

$head ode_step_size$$
is the step size used to integrate the ordinary differential equation.
The step size is the same in age and time because it is along cohort lines.

$head tolerance$$
is the desired relative convergence tolerance (requested of Ipopt).

$head max_num_iter$$
is the maximum number of iterations.
The algorithm terminates with an error message if the number of
iterations exceeded this number.

$end
*/
# include <cctype>

# include <dismod_at/get_db_input.hpp>

int main(int n_arg, const char** argv)
{	// ---------------- using statements ----------------------------------
	using std::cerr;
	using std::endl;
	using CppAD::vector;

	// ---------------- command line arguments ---------------------------
	const char* usage =
	"dismod_at database parent_node ode_step_size tolerance max_num_iter";
	if( n_arg != 6 )
	{	cerr << usage << endl <<
		"Expected 5 command line argument but found " << narg - 1 << endl;
		exit(1);
	}
	size_t i_arg = 0;
	const std::string database_str      = argv[++i_arg];
	const std::string parent_node_str   = argv[++i_arg];
	const std::string ode_step_size_str = argv[++i_arg];
	const std::string tolerance_str     = argv[++i_arg];
	const std::string max_num_iter_str  = argv[++i_arg];
	//
	// --------------- get the input tables -----------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(database_str, new_file);

	dismod_at::db_input_struct db_input;
	get_db_input(database, db_input);
	//
	// --------------- parent_node_id -----------------------------------
	// parent_node_id
	size_t parent_node_id;
	bool is_id = true;
	for(size_t i = 0; i < parent_node_str.size(); i++)
		is_id &= std::isdigit( parent_node_str[i] );
	if( is_id )
		parent_node_id = std::atoi( parent_node_str.cstr() );
	else
	{	parent_node_id = db_input.node_table.size();
		for(size_t id = 0; id < db_input.node_table.size(); id++)
			if( db_input.node_table[id].node_name == parent_node_str )
				parent_node_id = id;
		if( parent_node_id == db_input.node_table.size() )
		{	cerr << "dismod_at: cannot find node_name " << parent_node_str
			<< endl << "in " << database_str << " node table" << endl;
		}
	}
	// --------------- run fit_model ------------------------------------
	dismod_at::fit_model fit_object(
     pack_object, prior_table, s_info_vec, data_object, prior_object
	);
	fit_object.run_fit(tolerance_str, max_num_iter_str);
	vector<double> solution = fit_object.get_solution()
}
