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

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
variable_vec<Float>::variable_vec(
	const CppAD::vector<run_struct>&     run_table      ,
	const CppAD::vector<node_struct>&    node_table     ,
	const CppAD::vector<data_struct>&    data_table     ,
	const CppAD::vector<smooth_struct>&  smooth_table   ,
	const CppAD::vector<mulcov_struct>&  mulcov_table
)
{	assert( run_table.size() == 1 );

	// parent_node_id_
	size_t parent_node_id = run_table[0].parent_node;

	// child_node_id_
	assert( node_id_.size() == 0 );
	node_id_.push_back( parent_node_id );
	for(size_t i = 0; i < node_table.size(); i++)
	{	if( node_table[i].parent == parent_node_id )
			node_id_.push_back(i);
	}

	// data_id2child_index_
	data_id2node_index_.resize( data_table.size() );
	for(size_t data_id = 0; data_id < data_table.size(); data_id++)
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

	// smooth_table_
	smooth_table_ = smooth_table;

	// rate_mean_mulcov_covariate_id_ 
	// rate_mean_mulcov_smooth_id_
}

} // END DISMOD_AT_NAMESPACE
