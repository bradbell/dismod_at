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
$begin checkpoint_hes$$
$spell
	std
	cpp
	hes
	obj
	adfun
	eval
	const
	sv
	var
$$

$section Checkpoint a Sparse Hessian Calculation$$

$head 2DO$$
It is intended that this will be used to
replace the $cref/ran_like_hes/approx_mixed_ran_like_hes/$$
function in $code record_ran_obj.cpp$$.
Currently this leads to slow down durn the calculation of sparsity patterns
with respect to the fixed effects Hessian.

$head Syntax$$
$icode%checkpoint_hes %atom_hes%()
%$$
$icode%atom_hes%.initialize(%
	%name%, %a1_adfun%, %a1_x%, %a1_w%, %row%, %col%, %work%
)%$$
$icode%sv% = atom_hes%.size_var()
%$$
$code%atom_hes%.eval%(%a1_x%, %a1_val%)
%$$

$head Purpose$$
This stores the operation sequence corresponding to a sparse
Hessian calculation so that it can be used as an atomic operation
(instead of being re-taped multiple times).

$head Constructor$$
The sparse Hessian checkpoint object constructor
$codei%
	checkpoint_hes %atom_hes%()
%$$
creates a $code CppAD::checkpoint<double>$$ object for evaluating
sparse Hessians.
This checkpoint object is accessed using the $icode atom_hes$$ object.

$head Destructor$$
The object $icode atom_hes$$ must still exist (not be destructed)
for as long as any $code CppAD::ADFun$$ objects use its atomic operation.

$head initialize$$
The $icode atom_hes$$ object must be initialized,
before any calls to its $code eval$$ function, using the syntax
$icode%atom_hes%.initialize(%
	%a1_adfun%, %a1_w%, %row%, %col%, %work%,
)%$$
All of the arguments to this function are stored by reference,
so they must exist for as long as the $icode atom_hes$$ object exists.

$subhead name$$
This $code initialize$$ argument has prototype
$codei%
	const char* name
%$$
It is the name used for error reporting.
The suggested value for $icode name$$ is $icode atom_hes$$; i.e.,
the same as used for the object being initialized.

$subhead a1_adfun$$
This $code initialize$$ argument has prototype
$codei%
	CppAD::ADFun< CppAD::AD<double> > %a1_adfun%
%$$
This is a recording of the function,
$latex f : \B{R}^n \rightarrow \B{R}^m$$,
that we are computing sparse Hessians for.

$subhead a1_x$$
This $code initialize$$ argument has prototype
$codei%
	const CppAD::vector< CppAD::AD<double> >& %a1_x%
%$$
It is a point at which we could evaluate the sparse Hessian.

$subhead a1_w$$
This $code initialize$$ argument has prototype
$codei%
	const CppAD::vector< CppAD::AD<double> >& %a1_w%
%$$
It is the weighting in the range space; i.e.,
we are computing sparse Hessians for
$latex \[
	\sum_{i=0}^{m-1} w_i f_i (x)
\]$$.

$subhead row$$
This $code initialize$$ argument has prototype
$codei%
	const CppAD::vector<size_t>& %row%
%$$
It is the row indices for the elements of the sparse Hessian
that we are calculating.

$subhead col$$
This $code initialize$$ argument has prototype
$codei%
	const CppAD::vector<size_t>& %col%
%$$
It is the column indices for the elements of the sparse Hessian
that we are calculating.
It must have the same size as $icode row$$ above.

$subhead work$$
This $code initialize$$ argument has prototype
$codei%
	CppAD::sparse_hessian_work& %work%
%$$
It is the result of a previous call to
$codei%
	%a1_adfun%.SparseHessian(
		%a1_x%, %a1_w%, %p%, %row%, %col%, %a1_val%, %work%
	)
%$$
where $icode p$$ is the Hessian sparsity pattern for $icode a1_adfun$$
with prototype
$code%
	CppAD::vector< std::set<size_t> > %p%
%$$
and $icode a1_val$$ has size $icode m$$ and prototype
$codei%
	const CppAD::vector< CppAD::AD<double> >& %a1_val%
%$$

$head size_var$$
The return value for this member function has prototype
$codei%
	size_t %sv%
%$$
It is the number of variables in the tape used to represent the Hessian.
If the  $code initialize$$ member function has not been called,
the return value is $icode%sv% = 0%$$.

$head eval$$
The $code initialize$$ member function must be called before
the $code eval$$ member function is used.

$subhead a1_x$$
This $code eval$$ argument has prototype
$codei%
	const CppAD::vector< CppAD::AD<double> >& %a1_x%
%$$
It is a point at which we are evaluating the sparse Hessian.

$subhead a1_val$$
This $code eval$$ argument has prototype
$codei%
	CppAD::vector< CppAD::AD<double> >& %a1_val%
%$$
It size must have the same size as $icode row$$ above.
The input value of its elements does not matter.
Upon return,
for $icode%k% = 0 ,%...%, %m%-1%$$,
$icode%a1_val%[%k%]%$$ is the value of the
partial of the sparse Hessian at row index $icode%row%[%k%]%$$
and column index $icode%col%[%k%]%$$.

$end
----------------------------------------------------------------------------
*/
# include <dismod_at/checkpoint_hes.hpp>
# include <dismod_at/configure.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// -------------------------------------------------------------------------
// ctor
checkpoint_hes_algo::checkpoint_hes_algo(
	CppAD::ADFun<a1_double>&          a1_adfun  ,
	const CppAD::vector<a1_double>&   a1_w      ,
	const CppAD::vector<size_t>&      row       ,
	const CppAD::vector<size_t>&      col       ,
	CppAD::sparse_hessian_work&       work      )
:
a1_adfun_(a1_adfun),
a1_w_(a1_w),
row_(row),
col_(col),
work_(work)
{ }
// operator()
void checkpoint_hes_algo::operator()(
	const a1d_vector& a1_x, a1d_vector& a1_val
)
{	// size of outputs
	size_t n_nonzero = row_.size();
	assert( col_.size() == n_nonzero );
	assert( a1_val.size() == n_nonzero );
	if( n_nonzero == 0 )
	{	// special case where Hessian is zero.
		return;
	}
	assert( a1_w_.size() == a1_adfun_.Range() );

	// work is defined
	CppAD::vector< std::set<size_t> > not_used(0);

	// compute the sparse Hessian
	a1_adfun_.SparseHessian(
		a1_x, a1_w_, not_used, row_, col_, a1_val, work_
	);
}
// -------------------------------------------------------------------------
// ctor
checkpoint_hes::checkpoint_hes(void)
: atom_fun_(DISMOD_AT_NULL_PTR)
{ }
// destructor
checkpoint_hes::~checkpoint_hes(void)
{	if( atom_fun_ != DISMOD_AT_NULL_PTR )
		delete atom_fun_;
}
// initialize
void checkpoint_hes::initialize(
	const char*                       name      ,
	CppAD::ADFun<a1_double>&          a1_adfun  ,
	const a1d_vector&                 a1_x      ,
	const CppAD::vector<a1_double>&   a1_w      ,
	const CppAD::vector<size_t>&      row       ,
	const CppAD::vector<size_t>&      col       ,
	CppAD::sparse_hessian_work&       work      )
{	assert( atom_fun_ == DISMOD_AT_NULL_PTR );
	assert( row.size() == col.size() );
	//
	checkpoint_hes_algo* algo = new checkpoint_hes_algo(
		a1_adfun, a1_w, row, col, work
	);
	//
	a1d_vector a1_val( row.size() );
	atom_fun_ = new CppAD::checkpoint<double>(name, *algo, a1_x, a1_val);
	assert( atom_fun_ != DISMOD_AT_NULL_PTR );
	//
	// user boolean sparsity patterns
	atom_fun_->option(CppAD::atomic_base<double>::bool_sparsity_enum);
	//
	delete algo;
}
// size_var
size_t checkpoint_hes::size_var(void)
{	if( atom_fun_ ==  DISMOD_AT_NULL_PTR )
		return 0;
	return atom_fun_->size_var();
}
// eval
void checkpoint_hes::eval(const a1d_vector& a1_x, a1d_vector& a1_val)
{	assert( atom_fun_ != DISMOD_AT_NULL_PTR );
	(*atom_fun_)(a1_x, a1_val);
}

} // END_DISMOD_AT_NAMESPACE
