// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin child_info$$
$spell
	avgint
	dismod
	const
	CppAD
	struct
$$

$section Child Indices and Tables Indices$$

$head Syntax$$
$codei%child_info %child_object%(%parent_node_id%, %node_table%, %table%)
%$$
$icode%n_child%  = %child_object%.child_size()
%$$
$icode%node_id%  = %child_object%.child_id2node_id(%child_id%)
%$$
$icode%child%    = %child_object%.table_id2child(%table_id%)%$$

$head child_object$$
Except for it's constructor, this object has prototype
$codei%
	const child_info %child_object%
%$$

$head Constructor$$

$subhead parent_node_id$$
This argument has prototype
$codei%
	size_t %parent_node_id%
%$$
and is the
$cref/parent_node_id/option_table/parent_node_id/$$.
for the fit command.

$subhead node_table$$
This argument has prototype
$codei%
	const CppAD::vector<node_struct>& %node_table%
%$$
and is the $cref/node_table/get_node_table/node_table/$$.
Only the following fields of this table are used: $code parent$$.

$subhead table$$
This argument has one of the following prototypes
$codei%
	const CppAD::vector<%data_struct%>&     %table%
	const CppAD::vector<%avgint_struct%>& %table%
%$$


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

$head table_id2child$$

$subhead table_id$$
This argument has prototype
$codei%
	size_t %table_id%
%$$
and is the primary key in the $icode table$$, i.e.,
the index into the vector $icode table$$.

$subhead child$$
This return value has prototype
$codei%
	size_t %child%
%$$
If $icode%child% < %n_child%$$,
$codei%
	%table%[%table_id%].node_id
%$$
is a descendent of
$codei%
	%child_object%.child_id2node_id(%child%)
%$$
in the $cref node_table$$.
$pre

$$
If $icode%child% == %n_child%$$,
$codei%
	%table%[%table_id%].node_id
%$$
is the parent node.
$pre

$$
If $icode%child% == %n_child%+1%$$,
$codei%
	%table%[%table_id%].node_id
%$$
is not the parent node and not a descendent of the parent node.

$end
*/
# include <dismod_at/child_info.hpp>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/get_avgint_table.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template<class Row>
child_info::child_info(
	size_t                            parent_node_id         ,
	const CppAD::vector<node_struct>& node_table             ,
	const CppAD::vector<Row>&         table                  )
{	assert( parent_node_id != size_t(-1) );

	// child_id2node_id
	assert( child_id2node_id_.size() == 0 );
	for(size_t node_id = 0; node_id < node_table.size(); node_id++)
	{	size_t parent = size_t( node_table[node_id].parent );
		if( parent == parent_node_id )
			child_id2node_id_.push_back(node_id);
	}

	// table_id2child_id
	size_t n_table = table.size();
	table_id2child_.resize(n_table);
	for(size_t table_id = 0; table_id < n_table; table_id++)
	{	size_t node_id = size_t( table[table_id].node_id );
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
			more = (! found) && (node_id != DISMOD_AT_NULL_SIZE_T);
			if(more)
				node_id = size_t( node_table[node_id].parent );
		}
		if( ! found )
			child = child_id2node_id_.size() + 1;
		table_id2child_[table_id] = child;
	}
}

size_t child_info::child_size(void) const
{	return child_id2node_id_.size(); }

size_t child_info::child_id2node_id(size_t child_id) const
{	return child_id2node_id_[child_id]; }

size_t child_info::table_id2child(size_t table_id) const
{	return table_id2child_[table_id]; }


// instantiate child_info constructor for two possible cases
template child_info::child_info(
	size_t                            parent_node_id         ,
	const CppAD::vector<node_struct>& node_table             ,
	const CppAD::vector<data_struct>& table
);
template child_info::child_info(
	size_t                                parent_node_id             ,
	const CppAD::vector<node_struct>&     node_table             ,
	const CppAD::vector<avgint_struct>& table
);

} // END DISMOD_AT_NAMESPACE

