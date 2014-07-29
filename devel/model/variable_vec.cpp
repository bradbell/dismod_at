// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <cppad/cppad.hpp>
# include <dismod_at/dismod_at.hpp>

/*
$begin varaible_vec$$
$spell
	var
	dismod
	vec
	mulcov
	const
	CppAD
	struct
	enum
$$

$section The Variable Vector Class$$

$head Syntax$$
$codei%dismod_at::variable_vec %var%(
	%run_table%,    %node_table%, %data_table%, 
	%mulcov_table%, %rate_table%, %integrand_table%
	%smooth_info_vec%,
)%$$

$head name_table$$
For $icode name$$ equal to 
$code run$$, $code node$$, $code data$$, $code mulcov$$, $code rate$$,
and $code integrand$$ these arguments have prototype
$codei%
	const CppAD::vector<run_struct>&      %run_table%
	const CppAD::vector<node_struct>&     %node_table%
	const CppAD::vector<data_struct>&     %data_table%
	const CppAD::vector<mulcov_struct>&   %mulcov_table%
	const CppAD::vector<rate_enum>&       %rate_table%
	const CppAD::vector<integrand_enum>&  %integrand_table%,
%$$
and are the corresponding tables; e.g., see
$cref/run_table/get_run_table/run_table/$$.

$head smooth_info_vec$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_info>&     %smooth_info_vec%
%$$
and is size is the number of rows in the $cref smooth_table$$.
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%smooth_info_vec%[%smooth_id%]
%$$ 
is the $cref smooth_info$$ for the corresponding $icode smooth_id$$.

$end
*/

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
variable_vec<Float>::variable_vec(
	const CppAD::vector<run_struct>&      run_table         ,
	const CppAD::vector<node_struct>&     node_table        ,
	const CppAD::vector<data_struct>&     data_table        ,
	const CppAD::vector<mulcov_struct>&   mulcov_table      ,
	const CppAD::vector<rate_enum>&       rate_table        ,
	const CppAD::vector<integrand_enum>&  integrand_table   ,
	const CppAD::vector<smooth_info>&     smooth_info_vec
) :
data_table_( data_table )     ,
smooth_info_vec_( smooth_info_vec )
{	using std::string;

	size_t n_run       = run_table.size();
	size_t n_node      = node_table.size();
	size_t n_data      = data_table.size();
	size_t n_mulcov    = mulcov_table.size();
	size_t n_rate      = rate_table.size();
	size_t n_integrand = integrand_table.size();
	size_t n_smooth    = smooth_info_vec.size();
	assert( n_run == 1 );
	assert( n_rate == number_rate_enum );

	// parent_node_id_
	size_t parent_node_id = run_table[0].parent_node;

	// child_node_id_
	assert( node_id_.size() == 0 );
	node_id_.push_back( parent_node_id );
	for(size_t i = 0; i < n_node; i++)
	{	if( node_table[i].parent == parent_node_id )
			node_id_.push_back(i);
	}

	// data_id2child_index_
	data_id2node_index_.resize( data_table.size() );
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	size_t node_id = data_table[data_id].node_id;
		size_t j       = 0;
		bool   more    = true;
		while( more )
		{	bool found = false;
			while( (! found) &&  j < node_id_.size() )
			{	found = node_id == node_id_[j];
				if( ! found )
					j++;
			}
			if( found )
				more = false;
			else 
			{	node_id = node_table[node_id].parent;
				more    = node_id != size_t(-1);
				j       = 0;
			}
		}
		if( node_id == size_t(-1) )
			data_id2node_index_[data_id] = size_t(-1);
		else
			data_id2node_index_[data_id] = j;
	}

	// rate_mean_mulcov_
	rate_mean_mulcov_.resize( n_rate );
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	assert( rate_mean_mulcov_[rate_id].size() == 0 );
		for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
		{	bool match = mulcov_table[mulcov_id].mulcov_type == "rate_mean";
			match     &= mulcov_table[mulcov_id].rate_id == rate_id;
			if( match )
			{	mulcov_pair info;
				info.covariate_id = mulcov_table[mulcov_id].covariate_id;
				info.smooth_id    = mulcov_table[mulcov_id].smooth_id;
				for(size_t i = 0; i < rate_mean_mulcov_.size(); i++)
				{	size_t tmp = rate_mean_mulcov_[i].covariate_id;	
					if(info.covariate_id == tmp )
					{	string table_name = "mulcov";
						string message = 
							"This rate_mean covaraite appears twice";
						table_error_exit(table_name, mulcov_id, message);
					}
				}
				rate_mean_mulcov_.pushback(info);
			}
		}
	}

	// meas_mean_mulcov_
	meas_mean_mulcov_.resize( n_integrand );
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	assert( meas_mean_mulcov_[integrand_id].size() == 0 );
		for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
		{	bool match = mulcov_table[mulcov_id].mulcov_type == "meas_mean";
			match &= mulcov_table[mulcov_id].integrand_id == integrand_id;
			if( match )
			{	mulcov_pair info;
				info.covariate_id = mulcov_table[mulcov_id].covariate_id;
				info.smooth_id    = mulcov_table[mulcov_id].smooth_id;
				for(size_t i = 0; i < meas_mean_mulcov_.size(); i++)
				{	size_t tmp = meas_mean_mulcov_[i].covariate_id;	
					if(info.covariate_id == tmp )
					{	string table_name = "mulcov";
						string message = 
							"This meas_mean covaraite appears twice";
						table_error_exit(table_name, mulcov_id, message);
					}
				}
				meas_mean_mulcov_.pushback(info);
			}
		}
	}

	// meas_std_mulcov_
	meas_std_mulcov_.resize( n_integrand );
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	assert( meas_std_mulcov_[integrand_id].size() == 0 );
		for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
		{	bool match = mulcov_table[mulcov_id].mulcov_type == "meas_std";
			match &= mulcov_table[mulcov_id].integrand_id == integrand_id;
			if( match )
			{	mulcov_pair info;
				info.covariate_id = mulcov_table[mulcov_id].covariate_id;
				info.smooth_id    = mulcov_table[mulcov_id].smooth_id;
				for(size_t i = 0; i < meas_std_mulcov_.size(); i++)
				{	size_t tmp = meas_std_mulcov_[i].covariate_id;	
					if(info.covariate_id == tmp )
					{	string table_name = "mulcov";
						string message = 
							"This meas_std covaraite appears twice";
						table_error_exit(table_name, mulcov_id, message);
					}
				}
				meas_std_mulcov_.pushback(info);
			}
		}
	}
}

} // END DISMOD_AT_NAMESPACE
