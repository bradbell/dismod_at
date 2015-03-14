// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin child_data$$
$spell
	dismod
	const
	CppAD
	struct
$$

$section Child Indices and Data Indices$$

$head Syntax$$
$codei%child_data %cd%(%parent_node_id%, %node_table%, %data_table%)
%$$
$icode%n_child%  = %cd%.child_size()
%$$
$icode%node_id%  = %cd%.child_id2node_id(%child_id%)
%$$
$icode%child%    = %cd%.data_id2child(%data_id%)
%$$

$head cd$$
Except for it's constructor, this object has prototype
$codei%
	const child_data %cd%
%$$

$head Constructor$$

$subhead parent_node_id$$
This argument has prototype
$codei%
	size_t %parent_node_id%
%$$
and is the run table
$cref/parent_node_id/run_table/parent_node_id/$$.

$subhead node_table$$
This argument has prototype
$codei%
	const CppAD::vector<node_struct>& %node_table%
%$$
and is the $cref/node_table/get_node_table/node_table/$$.
Only the following fields of this table are used: $code parent$$.

$subhead data_table$$
This argument has prototype
$codei%
	const CppAD::vector<data_struct>& %data_table%
%$$
and is the $cref/data_table/get_data_table/data_table/$$.
Only the following fields of this table are used: $code node_id$$.

$head child_size$$

$subhead n_child$$
This return value has prototype
$codei%
	size_t %n_child%
%$$
and is the size of the
$cref/child group/node_table/parent/Child Group/$$ corresponding to the
for the specified parent node.

$head child_id2node_id$$

$subhead child_id$$
This argument has prototype
$codei%
	size_t %child_id%
%$$
and is less than $icode n_child$$.
Note that the corresponding node order is the order that the
children appear in $icode node_table$$.
Also note that if $icode%n_child% == 0%$$,
there is no valid value for $icode child_id$$.

$subhead node_id$$
This return value has prototype
$codei%
	size_t %node_id%
%$$
and is the $cref/node_id/node_table/node_id/$$ for the
corresponding $icode child_id$$.

$head data_id2child$$

$subhead data_id$$
This argument has prototype
$codei%
	size_t %data_id%
%$$
and is the $cref/data_id/data_table/data_id/$$ for a
row of the data table.

$subhead child$$
This return value has prototype
$codei%
	size_t %child%
%$$
If $icode%child% < %n_child%$$,
it is the $icode child_id$$ that this $icode data_id$$
is associated with; i.e.,
the $cref/node_id/data_table/node_id/$$ for this
$icode data_id$$ is a descendent
of the node
$codei%
	%cd%.child_id2node_id(%child%)
%$$
in the $cref node_table$$.
$pre

$$
If $icode%child% == %n_child%$$,
the $cref/node_id/data_table/node_id/$$ for this $icode data_id$$ is
the parent node.
$pre

$$
If $icode%child% == %n_child%+1%$$,
the $cref/node_id/data_table/node_id/$$ for this $icode data_id$$ is
not a descendent of the parent node.

$end
*/
# include <dismod_at/child_data.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

child_data::child_data(
	size_t                            parent_node_id ,
	const CppAD::vector<node_struct>& node_table     ,
	const CppAD::vector<data_struct>& data_table     )
{	assert( parent_node_id != size_t(-1) );

	// child_id2node_id
	assert( child_id2node_id_.size() == 0 );
	for(size_t node_id = 0; node_id < node_table.size(); node_id++)
	{	size_t parent = size_t( node_table[node_id].parent );
		if( parent == parent_node_id )
			child_id2node_id_.push_back(node_id);
	}

	// data_id2child_id
	size_t n_data = data_table.size();
	data_id2child_.resize(n_data);
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	size_t node_id = size_t( data_table[data_id].node_id );
		bool   found   = parent_node_id == node_id;
		size_t child   = child_id2node_id_.size();
		bool   more    = ! found;
		while(more)
		{	for(size_t i = 0; i < child_id2node_id_.size(); i++)
			{	if( child_id2node_id_[i] == node_id )
				{	child = i;
					found = true;
				}
			}
			more = (! found) && (node_id != size_t(-1));
			if(more)
				node_id = size_t( node_table[node_id].parent );
		}
		if( ! found )
			child = child_id2node_id_.size() + 1;
		data_id2child_[data_id] = child;
	}

}

size_t child_data::child_size(void) const
{	return child_id2node_id_.size(); }

size_t child_data::child_id2node_id(size_t child_id) const
{	return child_id2node_id_[child_id]; }

size_t child_data::data_id2child(size_t data_id) const
{	return data_id2child_[data_id]; }


} // END DISMOD_AT_NAMESPACE

