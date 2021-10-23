/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
         GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <gsl/gsl_randist.h>
# include <cppad/utility/to_string.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/hold_out_command.hpp>
# include <dismod_at/error_exit.hpp>

/*
-----------------------------------------------------------------------------
$begin hold_out_command$$
$spell
	dismod
$$

$section Hold Out Command: Randomly Sub-sample The Data$$

$head Syntax$$
$codei%dismod_at %database% hold_out %integrand_name% %max_fit%$$

$head Purpose$$
This command is used to set a maximum number of data values
that are included in subsequent fits.
It is intended to make the initialization and fitting faster.
The random sample of which values to include can be made repeatable using
$cref/random_seed/option_table/random_seed/$$.

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head integrand_name$$
This is the
$cref/integrand/integrand_table/integrand_name/$$ that we are sub-sampling.

$head max_fit$$
This is the maximum number of data points to fit for the specified integrand;
i.e., the maximum number that are not held out.
If for this integrand there are more than $icode mas_fit$$ points with
$cref/hold_out/data_table/hold_out/$$ zero in the data table,
points are randomly held out so that there are $icode max_fit$$
points fit for this integrand.

$subhead Balancing$$
The choice of which points to include in the fit tries to sample the
same number of data points from each of the child nodes (and the parent node).
If there are not sufficiently many data for one of these nodes, the others
make up the difference.

$head data_subset_table$$
Only rows of the $cref data_subset_table$$ that correspond to this integrand
are modified.
The $cref/hold_out/data_subset_table/hold_out/$$ is set one (zero)
if the corresponding data is (is not) selected for hold out.
Only points that have $icode hold_out$$ zero in the data table
can have hold_out non-zero in the data_subset table.
See the fit command $cref/hold_out/fit_command/hold_out/$$
documentation.

$head Example$$
The file $cref user_hold_out.py$$ contains an example and test
using this command.

$end
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
// ---------------------------------------------------------------------------
void hold_out_command(
	sqlite3*                                      db                ,
	const std::string&                            integrand_name    ,
	const std::string&                            max_fit_str       ,
	const child_info&                             child_info4data   ,
	const CppAD::vector<integrand_struct>&        integrand_table   ,
	const CppAD::vector<data_struct>&             data_table        )
{	using std::string;
	using CppAD::vector;
	using CppAD::to_string;
	//
	// rng
	// gsl random number generator
	gsl_rng* rng = CppAD::mixed::get_gsl_rng();
	//
	// data_subset_table
	vector<data_subset_struct> data_subset_table = get_data_subset(db);
	//
	// n_subset
	size_t n_subset = data_subset_table.size();
	//
	// max_fit
	size_t max_fit = std::atoi( max_fit_str.c_str() );
	//
	// this_integrand
	integrand_enum this_integrand = number_integrand_enum;
	for(size_t i = 0; i < size_t(number_integrand_enum); ++i)
	{	if( integrand_enum2name[i] == integrand_name )
			this_integrand = integrand_enum(i);
	}
	if( this_integrand == number_integrand_enum )
	{	string msg = "hold_out_command: " + integrand_name;
		msg       += " is not a valid integrand name";
		error_exit(msg);
	}
	//
	// n_child
	size_t n_child = child_info4data.child_size();
	//
	// clear any previous hold_outs for this integrand
	for(size_t data_subset_id = 0; data_subset_id < n_subset; ++data_subset_id)
			data_subset_table[data_subset_id].hold_out = 0;
	//
	// src:
	// src[child_id] are the possible indices for this child_id
	CppAD::vector< CppAD::vector<int> > src(n_child + 1);
	for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
	{	// information about this data row
		size_t data_id      = data_subset_table[subset_id].data_id;
		size_t integrand_id = data_table[data_id].integrand_id;
		size_t child_id    = child_info4data.table_id2child(data_id);
		int    hold_out    = data_table[data_id].hold_out;
		integrand_enum integrand = integrand_table[integrand_id].integrand;
		//
		// is this data row in set of rows to choose from
		bool in_src = true;
		in_src     &= integrand == this_integrand;
		in_src     &= hold_out == 0;
		if( in_src )
			src[child_id].push_back( int(subset_id) );
	}
	//
	// src_size
	CppAD::vector<size_t> src_size(n_child + 1);
	for(size_t child_id = 0; child_id <= n_child; ++child_id)
		src_size[child_id] = src[child_id].size();
	//
	// size_order
	CppAD::vector<size_t> size_order(n_child + 1);
	CppAD::index_sort(src_size, size_order);
	//
	// n_fit
	size_t n_fit = 0;
	//
	for(size_t k = 0; k < n_child + 1; ++k)
	{	//
		// child_id
		size_t child_id = size_order[k];
		//
		// number of children (or parent) left
		size_t n_left = n_child + 1 - k;
		//
		// max_fit_child
		size_t max_fit_child = 1 + (max_fit - n_fit) / n_left;
		if( max_fit_child + n_fit > max_fit )
			max_fit_child = max_fit - n_fit;
		//
		// check if there will be hold outs for this child
		if(  max_fit_child < src_size[child_id] )
		{	//
			// n_hold_out
			size_t n_hold_out = src_size[child_id] - max_fit_child;
			//
			// dest: array of indices that are chosen
			CppAD::vector<int> dest(n_hold_out);
			//
			// choose which elements to hold out
			gsl_ran_choose(
				rng,
				dest.data(),
				n_hold_out,
				src[child_id].data(),
				src_size[child_id],
				sizeof(int)
			);
			//
			// hold out the chosen elements
			for(size_t i = 0; i < n_hold_out; ++i)
			{	int subset_id = dest[i];
				assert( data_subset_table[subset_id].hold_out == 0 );
				data_subset_table[subset_id].hold_out = 1;
			}
		}
		// n_fit
		size_t n_fit_child = std::min( max_fit, src_size[child_id] );
		n_fit             += n_fit_child;
	}
	//
	// drop old data_subset table
	string sql_cmd    = "drop table data_subset";
	exec_sql_cmd(db, sql_cmd);
	//
	// write new data_subset table
	string table_name = "data_subset";
	size_t n_col      = 5;
	vector<string> col_name(n_col), col_type(n_col);
	vector<string> row_value(n_col * n_subset);
	vector<bool>   col_unique(n_col);
	//
	col_name[0]       = "data_id";
	col_type[0]       = "integer";
	col_unique[0]     = true;
	//
	col_name[1]       = "hold_out";
	col_type[1]       = "integer";
	col_unique[1]     = false;
	//
	col_name[2]       = "density_id";
	col_type[2]       = "integer";
	col_unique[2]     = false;
	//
	col_name[3]       = "eta";
	col_type[3]       = "real";
	col_unique[3]     = false;
	//
	col_name[4]       = "nu";
	col_type[4]       = "real";
	col_unique[4]     = false;
	//
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	int data_id    = data_subset_table[subset_id].data_id;
		int hold_out   = data_subset_table[subset_id].hold_out;
		int density_id = data_subset_table[subset_id].density_id;
		double eta     = data_subset_table[subset_id].eta;
		double nu      = data_subset_table[subset_id].nu;
		row_value[n_col * subset_id + 0] = to_string( data_id );
		row_value[n_col * subset_id + 1] = to_string( hold_out );
		row_value[n_col * subset_id + 2] = to_string( density_id );
		if( std::isnan(eta) )
			row_value[n_col * subset_id + 3] = "";
		else
			row_value[n_col * subset_id + 3] = to_string( eta );
		if( std::isnan(nu) )
			row_value[n_col * subset_id + 4] = "";
		else
			row_value[n_col * subset_id + 4] = to_string( nu );
	}
	create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}
} // END_DISMOD_AT_NAMESPACE
