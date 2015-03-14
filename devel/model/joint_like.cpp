// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin devel_joint_like_ctor$$
$spell
	vec
	var
	const
	CppAD
	struct
	std
$$

$section Joint Likelihood$$

$head Under Construction$$

$head Syntax$$
$codei%joint_like %joint_object%(%data_object%, %pack_object%, %prior_table%)%$$

$head Purpose$$
This object can be used to evaluate the joint likelihood of the
$cref/fixed effects/model_variable/Fixed Effects, theta/$$, $latex \theta$$,
$cref/random effects/model_variable/Random Effects, u/$$, $latex u$$,
and the
$cref/measurement vector/data_like/Data Table Notation/y_i/$$, $latex y$$
as a function of the fixed and random effects; i.e., to evaluate
$latex \[
f( u , \theta ) = \B{p}( y | u , \theta ) \B{p}( u | \theta ) \B{p}( \theta )
\] $$
up to a constant multiple that does not depend on
$latex u$$ or $latex \theta$$.

$head References$$
The $icode joint_object$$ will hold a reference to its arguments
$icode data_object$$, $icode pack_object$$, and $icode prior_table$$.
For this reason, $icode joint_object$$ cannot be used once after
$icode data_object$$, $icode pack_object$$, or $icode prior_table$$
has been deleted.

$head data_object$$
This argument has prototype
$codei%
	const data_model& %data_object%
%$$
see $cref/data_model/devel_data_model/$$,
and contains the information necessary for evaluation of
$latex \B{p}( y | u , \theta )$$.

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
and is the $cref pack_info$$ information corresponding to corresponding
to this problem.

$head prior_table$$
This argument has prototype
$codei%
	const CppAD::vector<prior_struct>&  %prior_table%
%$$
and is the $cref/prior_table/get_prior_table/prior_table/$$.
Only to following fields are used
$cref/density_id/prior_table/density_id/$$,
$cref/mean/prior_table/mean/$$,
$cref/std/prior_table/std/$$,
$cref/eta/prior_table/eta/$$.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/joint_like.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// consctructor
joint_like::joint_like(
	const data_model&                      data_object     ,
	const pack_info&                        pack_object       ,
	const CppAD::vector<prior_struct>&     prior_table     )
:
data_object_(data_object) ,
pack_object_(pack_object)     ,
prior_table_(prior_table)
{ }


} // END DISMOD_AT_NAMESPACE
