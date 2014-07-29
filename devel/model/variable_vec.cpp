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
	const CppAD::vector<run_struct>&     run_table   ,
	const CppAD::vector<node_struct>&    node_table  ,
	const CppAD::vector<data_struct>&    data_table
)
{	assert( run_table.size() == 1 );

	// parent_node_id_
	parent_node_id_ == run_table[0].parent_node;

	// child_node_id_
	assert( child_node_id_.size() == 0 );
	for(size_t node_id = 0; node_id < node_table.size(); node_id++)
	{	if( node_table[node_id].parent == parent_node_id_ )
			child_node_id_.push_back(node_id);
	}
	if( child_node_id_.size() < 2 )
	{	if( child_node_id_.size() == 1 )
			child_node_id_[0] = parent_node_id_;
		else
			child_node_id_.push_back(parent_node_id_);
	}

	// data_id2child_index_
	data_id2child_index_.resize( data_table.size() );
	for(size_t data_id = 0; data_id < data_table.size(); data_id++)
	{	size_t node_id = data_table[data_id].node_id;
		bool   more    = true;
		size_t j       = 0;
		while( more )
		{	while( more )
			{	more = node_id != child_node_id_[j];
				if( more )
				{	j++;
					if( j >= child_node_id_.size() )
						more = false;
					else
						more = node_id != child_node_id_[j];
				}
			}
			if( node_id != parent_node_id_ && j >= child_node_id_.size() )
			{	node_id = node_table[node_id].parent;
				more    = node_id != size_t(-1);
				j       = 0;
			}
		}
		if( j < child_node_id_.size() )
			data_id2child_index_[data_id] = j;
		else if ( node_id == parent_node_id_ )
			data_id2child_index_[data_id] = child_node_id_.size();
		else
		{	assert( node_id == size_t(-1) );
			data_id2child_index_[data_id] = child_node_id_.size() + 1;
		}
	}
};

} // END DISMOD_AT_NAMESPACE
