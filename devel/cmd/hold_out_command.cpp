/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-22 University of Washington
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
# include <dismod_at/balance_pair.hpp>

/*
-----------------------------------------------------------------------------
$begin hold_out_command$$
$spell
	dismod
	cov
	covariate
	Covariates
$$

$section Hold Out Command: Randomly Sub-sample The Data$$

$head Syntax$$
$codei%dismod_at %database% hold_out %integrand_name% %max_fit
%$$
$codei%dismod_at %database% hold_out %integrand_name% %max_fit%$$
$icode%cov_name% %cov_value_1% %cov_value_2%$$

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

$head cov_name$$
If this argument is present, it specifies a covariate column that
will be balanced; see covariate balancing below:

$head cov_value_1$$
If this argument is present, it specifies one of the covariate values
for the balancing. This is a string representation of a $code double$$ value.

$head cov_value_2$$
If this argument is present, it specifies the opposite covariate value
for the balancing. This is a string representation of a $code double$$ value.

$head Balancing$$

$subhead Child Nodes$$
The choice of which points to include in the fit tries to sample the
same number of data points from each of the child nodes (and the parent node).
If there are not sufficiently many data for one of these nodes, the others
make up the difference.

$subhead Covariates$$
If $icode cov_name$$ is present, any sample that has the
covariate value $icode cov_value_1$$ or $icode cov_value_2$$
will be paired with a sample from the opposite value (if possible).

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
The files $cref user_hold_out_1.py$$ and $cref user_hold_out_2.py$$
contain examples and tests using this command.

$end
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
// ---------------------------------------------------------------------------
void hold_out_command(
	sqlite3*                                      db                ,
	const std::string&                            integrand_name    ,
	const std::string&                            max_fit_str       ,
	const std::string&                            cov_name          ,
	const std::string&                            cov_value_1_str   ,
	const std::string&                            cov_value_2_str   ,
	const child_info&                             child_info4data   ,
	const CppAD::vector<integrand_struct>&        integrand_table   ,
	const CppAD::vector<covariate_struct>&        covariate_table   ,
	const CppAD::vector<data_struct>&             data_table        ,
	const CppAD::vector<double>&                  data_cov_value    )
{	using std::string;
	using CppAD::vector;
	using CppAD::to_string;
	//

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
	// n_covariate, covariate_id
	size_t n_covariate = covariate_table.size();
	size_t covariate_id = n_covariate;
	for(size_t i = 0; i < n_covariate; ++i)
		if( covariate_table[i].covariate_name == cov_name )
			covariate_id = i;
	if( covariate_id == n_covariate && 0 < cov_name.size() )
	{	string msg = "hold_out_command: " + cov_name;
		msg       += " is not a valid covariate name";
		error_exit(msg);
	}
	//
	// cov_value_1, cov_value_2
	double cov_value_1 = std::numeric_limits<double>::quiet_NaN();
	double cov_value_2 = std::numeric_limits<double>::quiet_NaN();
	if( 0 < cov_name.size() )
	{	cov_value_1 = std::atof( cov_value_1_str.c_str() );
		cov_value_2 = std::atof( cov_value_2_str.c_str() );
	}
	//
	// n_child
	size_t n_child = child_info4data.child_size();
	//
	// avail:
	// avail[child_id] are the available subset indices for this child_id
	CppAD::vector< CppAD::vector<int> > avail(n_child + 1);
	for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
	{	// information about this data row
		size_t data_id      = data_subset_table[subset_id].data_id;
		size_t integrand_id = data_table[data_id].integrand_id;
		size_t child_id    = child_info4data.table_id2child(data_id);
		int    hold_out    = data_table[data_id].hold_out;
		integrand_enum integrand = integrand_table[integrand_id].integrand;
		if( integrand == this_integrand )
		{	if( hold_out != 0 )
			{	assert( data_subset_table[subset_id].hold_out == 0 );
			}
			else
			{   // clear the hold_out values for this integrand
				data_subset_table[subset_id].hold_out = 0;
				// add this to the possible hold_out set
				avail[child_id].push_back( int(subset_id) );
			}
		}
	}
	//
	// avail_size
	CppAD::vector<size_t> avail_size(n_child + 1);
	for(size_t child_id = 0; child_id <= n_child; ++child_id)
		avail_size[child_id] = avail[child_id].size();
	//
	// size_order
	CppAD::vector<size_t> size_order(n_child + 1);
	CppAD::index_sort(avail_size, size_order);
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
		// n_fit
		if( avail_size[child_id] <= max_fit_child )
		{	// include all the data for this child
			n_fit += avail_size[child_id];
		}
		else if( cov_name.size() == 0 )
		{   // hold out some data for this child
			n_fit += max_fit_child;
			//
			// n_hold_out
			size_t n_hold_out = avail_size[child_id] - max_fit_child;
			//
			// chosen: array of indices that are chosen
			CppAD::vector<int> chosen(n_hold_out);
			//
			// choose which elements to hold out
			gsl_ran_choose(
				rng,
				chosen.data(),
				n_hold_out,
				avail[child_id].data(),
				avail_size[child_id],
				sizeof(int)
			);
			//
			// data_subset_table[subset_id].hold_out
			for(size_t i = 0; i < n_hold_out; ++i)
			{	int subset_id = chosen[i];
				assert( data_subset_table[subset_id].hold_out == 0 );
				data_subset_table[subset_id].hold_out = 1;
			}
		}
		else
		{	assert( covariate_id < n_covariate );
			assert( ! std::isnan( cov_value_1 ) );
			assert( ! std::isnan( cov_value_2 ) );
			//
			// sample_vec
			typedef std::pair<size_t, double> pair_t;
			CppAD::vector<pair_t> pair_vec;
			size_t n_pair = avail_size[child_id];
			for(size_t i = 0; i < n_pair; ++i)
			{	size_t subset_id = size_t ( avail[child_id][i] );
				size_t data_id   = data_subset_table[subset_id].data_id;
				size_t node_id   = data_table[data_id].node_id;
				size_t index     = data_id * n_covariate + covariate_id;
				double cov_value = data_cov_value[index];
				pair_vec.push_back( pair_t( node_id, cov_value ) );
			}
			CppAD::vector<size_t> sample_vec = balance_pair(
				max_fit_child, pair_vec, cov_value_1, cov_value_2
			);
			//
			// hold_out_vec
			CppAD::vector<bool> hold_out_vec( n_pair );
			for(size_t i = 0; i < n_pair; ++i)
				hold_out_vec[i] = true;
			for(size_t i = 0; i < max_fit_child; ++i)
				hold_out_vec[ sample_vec[i] ] = false;
			//
			// data_subset_table[subset_id].hold_out
			for(size_t i = 0; i < n_pair; ++i)
			{	if( hold_out_vec[i] )
				{	size_t subset_id = size_t( avail[child_id][i] );
					assert( data_subset_table[subset_id].hold_out == 0 );
					data_subset_table[subset_id].hold_out = 1;
				}
			}
		}
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
