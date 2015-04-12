// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

/*
$begin ipopt_xam_nlp$$
$spell
	Ipopt
$$

$section Ipopt Example: Declare Non-linear Program Problem Class$$

$nospell
$codep */
# include <coin/IpIpoptApplication.hpp>
# include <coin/IpTNLP.hpp>
namespace {
	// Ipopt types used by this file
	typedef Ipopt::Number  Number;
	typedef Ipopt::Index   Index;
	typedef Ipopt::TNLP::IndexStyleEnum IndexStyleEnum;
	//
	// ipopt_xam_nlp
	class ipopt_xam_nlp : public Ipopt::TNLP
	{
	public:
		// default constructor
		ipopt_xam_nlp(void);
		//
		// default destructor
		virtual ~ipopt_xam_nlp(void);
		//
		virtual bool get_nlp_info(
			Index&          n            ,
			Index&          m            ,
			Index&          nnz_jac_g    ,
			Index&          nnz_h_lag    ,
			IndexStyleEnum& index_style
		);
		virtual bool get_bounds_info(
				Index       n        ,
				Number*     x_l      ,
				Number*     x_u      ,
				Index       m        ,
				Number*     g_l      ,
				Number*     g_u
		);
		virtual bool get_starting_point(
			Index           n            ,
			bool            init_x       ,
			Number*         x            ,
			bool            init_z       ,
			Number*         z_L          ,
			Number*         z_U          ,
			Index           m            ,
			bool            init_lambda  ,
			Number*         lambda
		);
		virtual bool eval_f(
			Index           n        ,
			const Number*   x        ,
			bool            new_x    ,
			Number&         obj_value
		);
		virtual bool eval_grad_f(
			Index           n        ,
			const Number*   x        ,
			bool            new_x    ,
			Number*         grad_f
		);
		virtual bool eval_g(
			Index           n        ,
			const Number*   x        ,
			bool            new_x    ,
			Index           m        ,
			Number*         g
		);
		virtual bool eval_jac_g(
			Index           n        ,
			const Number*   x        ,
			bool            new_x    ,
			Index           m        ,
			Index           nele_jac ,
			Index*          iRow     ,
			Index*          jCol     ,
			Number*         values
		);
		virtual bool eval_h(
			Index         n              ,
			const Number* x              ,
			bool          new_x          ,
			Number        obj_factor     ,
			Index         m              ,
			const Number* lambda         ,
			bool          new_lambda     ,
			Index         nele_hess      ,
			Index*        iRow           ,
			Index*        jCol           ,
			Number*       values
		);
		virtual void finalize_solution(
			Ipopt::SolverReturn               status    ,
			Index                             n         ,
			const Number*                     x         ,
			const Number*                     z_L       ,
			const Number*                     z_U       ,
			Index                             m         ,
			const Number*                     g         ,
			const Number*                     lambda    ,
			Number                            obj_value ,
			const Ipopt::IpoptData*           ip_data   ,
			Ipopt::IpoptCalculatedQuantities* ip_cq
		);


	};
}
/* $$
$$ $comment end of nospell$$
$end
------------------------------------------------------------------------------
$begin ipopt_xam_ctor$$
$spell
	ipopt_xam_nlp
$$

$section Ipopt Example: Constructor and Destructor$$

$codep */
ipopt_xam_nlp::ipopt_xam_nlp(void)
{ }
ipopt_xam_nlp::~ipopt_xam_nlp(void)
{ }
/* $$
$end
------------------------------------------------------------------------------
$begin ipopt_xam_get_nlp_info$$
$spell
	ipopt_xam_get_nlp_info
	nnz_jac
	Jacobian
	bool
	Enum
	bool
	nlp
$$

$section Return Information About Problem Sizes$$

$head Syntax$$
$codei%get_nlp_info(%n%, %m%, %nnz_jac_g%, %nnz_h_lag%, %index_style%)%$$

$head n$$
is set to the number of variables in the problem (dimension of x).

$head m$$
is set to the number of constraints in the problem (dimension of g(x)).

$head nnz_jac_g$$
is set to the number of nonzero entries in the Jacobian of g(x).

$head nnz_h_lag$$
is set to the number of nonzero entries in the Hessian of the Lagrangian
$latex f(x) + \lambda^\R{T} g(x)$$.

$head index_style$$
is set to the numbering style used for row/col entries in the sparse matrix
format (C_STYLE: 0-based, FORTRAN_STYLE: 1-based).

$head Example$$
$codep */
bool ipopt_xam_nlp::get_nlp_info(
	Index&          n            ,  // out
	Index&          m            ,  // out
	Index&          nnz_jac_g    ,  // out
	Index&          nnz_h_lag    ,  // out
	IndexStyleEnum& index_style  )  // out
{
	n           = 2;
	m           = 1;
	nnz_jac_g   = 2;
	nnz_h_lag   = 2;
	index_style = C_STYLE;
	return true;
}
/* $$
$end
-------------------------------------------------------------------------------
$begin ipopt_xam_get_bounds_info$$
$spell
	ipopt_xam_nlp
	bool
$$

$section Return Optimization Bounds$$

$head Syntax$$
$codei%get_bounds_info(%n%, %x_l%, %x_u%, %m%, %g_l%, %g_u%)%$$

$head n$$
is the number of variables in the problem (dimension of x).

$head x_l$$
set to the lower bounds for $icode x$$ (has size $icode n$$).

$head x_u$$
set to the upper bounds for $icode x$$ (has size $icode n$$).

$head m$$
is the number of constraints in the problem (dimension of g(x)).

$head g_l$$
set to the lower bounds for $icode g(x)$$ (has size $icode m$$).

$head g_u$$
set to the upper bounds for $icode g(x)$$ (has size $icode m$$).

$head Example$$
$codep */
bool ipopt_xam_nlp::get_bounds_info(
		Index       n        ,   // in
		Number*     x_l      ,   // out
		Number*     x_u      ,   // out
		Index       m        ,   // in
		Number*     g_l      ,   // out
		Number*     g_u      )   // out
{
	assert( n == 2 );
	x_l[0] = -1.0;
	x_u[0] = 1.0;
	//
	x_l[1] = -1.0e19; // default -infinity
	x_u[1] = 1.0e19;  // default +infinity
	//
	assert( m == 1 );
	//
	g_l[0] = 0.0;
	g_u[0] = 0.0;
	//
	return true;
}
/* $$
$end
-------------------------------------------------------------------------------
$begin ipopt_xam_get_starting_point$$
$spell
	init
	ipopt_xam_nlp
	bool
$$

$section Return Initial Values Where Optimization is Started$$

$head Syntax$$
$codei%get_starting_point(
	%n%, %init_x%, %x%, %init_z%, %z_L%, %z_U%, %m%, %init_lambda%, %lambda%
)%$$

$head n$$
is the number of variables in the problem (dimension of x).

$head init_x$$
if true, the ipopt options specify that the this routine
will provide an initial value for $icode x$$.

$head x$$
if $icode init_x$$ is true,
set to the initial value for the primal variables (has size $icode n$$).

$head init_z$$
if true, the ipopt options specify that the this routine
will provide an initial value for $icode x$$ upper and lower bound
multipliers.

$head z_L$$
if $icode init_z$$ is true,
set to the initial value for the lower bound multipliers (has size $icode n$$).

$head z_U$$
if $icode init_z$$ is true,
set to the initial value for the upper bound multipliers (has size $icode n$$).

$head init_lambda$$
if true, the ipopt options specify that the this routine
will provide an initial value for $icode g(x)$$ upper and lower bound
multipliers.

$head lambda$$
if $icode init_lambda$$ is true,
set to the initial value for the $icode g(x)$$ multipliers
(has size $icode m$$).

$head Example$$
$codep */
bool ipopt_xam_nlp::get_starting_point(
	Index           n            ,  // in
	bool            init_x       ,  // in
	Number*         x            ,  // out
	bool            init_z       ,  // in
	Number*         z_L          ,  // out
	Number*         z_U          ,  // out
	Index           m            ,  // out
	bool            init_lambda  ,  // in
	Number*         lambda       )  // out
}
{
	assert( n == 2 );
	assert( init_x == true );
	x[0] = 0.5;
	x[1] = 1.5;
	assert( init_z == false );
	assert( m == 1 );
	assert( init_lambda == false );

	return true;
}
/* $$
$end
-------------------------------------------------------------------------------
$begin ipopt_xam_eval_f$$
$spell
	ipopt_xam_nlp
	bool
	eval
	obj
	const
$$

$section Compute Value of Objective$$

$head Syntax$$
$codei%eval_f(%n%, %x%, %new_x%, %obj_value%)%$$

$head n$$
is the number of variables in the problem (dimension of x).

$head x$$
is the value for the primal variables at which the objective
$latex f(x)$$ is computed (has size $icode n$$).

$head new_x$$
if true, no Ipopt evaluation method was previous called with the same
value for $icode x$$.

$head obj_val$$
set to the initial value of the objective function $latex f(x)$$.

$head Example$$
$codep */
bool ipopt_xam_nlp::eval_f(
	Index           n         ,  // in
	const Number*   x         ,  // in
	bool            new_x     ,  // in
	Number&         obj_value )  // out
{
	assert( n == 2 );
	Number x2 = x[1];
	obj_value = - (x2 - 2.0) * (x2 - 2.0);

	return true;
}
/* $$
$end
-------------------------------------------------------------------------------
$begin ipopt_xam_eval_grad_f$$
$spell
	ipopt_xam_nlp
	bool
	eval
	const
$$

$section Compute Gradient of the Objective$$

$head Syntax$$
$codei%eval_grad_f(%n%, %x%, %new_x%, %grad_f%)%$$

$head n$$
is the number of variables in the problem (dimension of x).

$head x$$
is the value for the primal variables at which the gradient
$latex \nabla f(x)$$ is computed (has size $icode n$$).

$head new_x$$
if true, no Ipopt evaluation method was previous called with the same
value for $icode x$$.

$head grad_f$$
is set to the value for the gradient $latex \nabla f(x)$$
(has size $icode m$$).

$head Example$$
$codep */
bool ipopt_xam_nlp::eval_grad_f(
	Index           n         ,  // in
	const Number*   x         ,  // in
	bool            new_x     ,  // in
	Number*         grad_f    )  // out
{
	assert( n == 2 );
	Number x2 = x[1];
	grad_f[0] = 0.0;
	grad_f[1] = - 2.0 * (x2 - 2.0);
	return true;
}
/* $$
$end
-------------------------------------------------------------------------------
$begin ipopt_xam_eval_g$$
$spell
	ipopt_xam_nlp
	bool
	const
	eval
$$

$section Compute Value of Constraint Functions$$

$head Syntax$$
$codei%eval_g(%n%, %x%, %new_x%, %m%, %g%)%$$

$head n$$
is the number of variables in the problem (dimension of x).

$head x$$
is the value for the primal variables at which the constraints
$latex g(x)$$ is computed (has size $icode n$$).

$head new_x$$
if true, no Ipopt evaluation method was previous called with the same
value for $icode x$$.

$head m$$
is the number of constraints in the problem (dimension of g(x)).

$head g$$
is set to the value for the constraint functions (has size $icode m$$).

$head Example$$
$codep */
bool ipopt_xam_nlp::eval_g(
	Index           n        ,  // in
	const Number*   x        ,  // in
	bool            new_x    ,  // in
	Index           m        ,  // in
	Number*         g        )  // out
{
	assert( n == 2 );
	//
	Number x1 = x[0];
	Number x2 = x[1];
	//
	assert( m = 1 );
	//
	g[0] = - (x1 * x1 + x2 - 1.0);
	//
	return true;
}
/* $$
$end
-------------------------------------------------------------------------------
$begin ipopt_xam_eval_jac_g$$
$spell
	ipopt_xam_nlp
	bool
	eval
	const
	nele_jac
	Jacobian
	nnz
$$

$section Compute Jacobian of Constraint Functions$$

$head Syntax$$
$codei%eval_jac_g(
	%n%, %x%, %new_x%, %m%, %nele_jac%, %iRow%, %jCol%, %values%
)%$$

$head n$$
is the number of variables in the problem (dimension of x).

$head x$$
is the value for the primal variables at which the Jacobian
of the constraints $latex \nabla g(x)$$ is computed (has size $icode n$$).

$head new_x$$
if true, no Ipopt evaluation method was previous called with the same
value for $icode x$$.

$head m$$
is the number of constraints in the problem (dimension of g(x)).

$head nele_jac$$
is the number of non-zero elements in the Jacobian of $icode g(x)$$; i.e.,
the same as
$cref/nnz_jac_g/ipopt_xam_get_nlp_info/nnz_jac_g/$$.

$head iRow$$
If $icode values$$ is $code NULL$$,
$icode iRow$$ has size $icode nele_jac$$ and is set to the
row indices for the non-zero entries in the Jacobian of the constraints
$latex g^{(1)} (x)$$.

$head jCol$$
If $icode values$$ is $code NULL$$,
$icode jCol$$ has size $icode nele_jac$$ and is set to the
column indices for the non-zero entries in the Jacobian of the constraints
$latex g^{(1)} (x)$$.

$head values$$
If $icode values$$ is not $code NULL$$,
it has size $icode nele_jac$$ and $icode%values%[%k%]%$$
is set to the value of element of the Jacobian $latex g^{(1)} (x)$$
with row index $icode%iRow%[%k%]%$$
and column index $icode%jRow%[%k%]%$$.


$head Example$$
$codep */
bool ipopt_xam_nlp::eval_jac_g(
	Index           n        ,  // in
	const Number*   x        ,  // in
	bool            new_x    ,  // in
	Index           m        ,  // in
	Index           nele_jac ,  // in
	Index*          iRow     ,  // out
	Index*          jCol     ,  // out
	Number*         values   )  // out
{
	assert( nele_jac == 2 );
	if( values == CPPAD_NULL )
	{
		iRow[0] = 1;
		jRow[0] = 1;
		//
		iRow[1] = 1;
		jRow[1] = 2;
		//
		return true;
	}
	assert( n == 2 );
	Number x1 = x[0];
	assert( m == 1 );
	//
	values[0] = - 2.0 * x1;
	values[1] = - 1.0;
	//
	return true;
}
/* $$
$end
-------------------------------------------------------------------------------
$begin ipopt_xam_eval_h$$
$spell
	ipopt_xam_nlp
	bool
	eval
	const
	obj
	nele_hess
	nnz
$$

$section Compute the Hessian of the Lagrangian$$

$head Syntax$$
$codei%eval_h(
	%n%, %x%, %new_x%,%obj_factor%, %m%, %lambda%, %new_lambda%,
	%nele_hess%, %iRow%, %jCol%, %values%
)%$$

$head Lagrangian$$
The Lagrangian is defined to be
$latex \[
	L(x) = \alpha f(x) + \sum_{i=0}^{m-1} \lambda_i g_i (x)
\] $$

$head n$$
is the number of variables in the problem (dimension of x).

$head x$$
is the value for the primal variables at which the
Hessian of the Lagrangian is computed (has size $icode n$$).

$head new_x$$
if true, no Ipopt evaluation method was previous called with the same
value for $icode x$$.

$head obj_factor$$
is the factor $latex \alpha$$ that multiplies the objective $latex f(x)$$
in the definition of the Lagrangian.

$head m$$
is the number of constraints in the problem (dimension of g(x)).

$head lambda$$
is the value of the constraint multipliers $latex \lambda$$
at which the Hessian is to be evaluated (has size $icode m$$).

$head new_lambda$$
if true, no Ipopt evaluation method was previous called with the same
value for $icode lambda$$.

$head nele_hess$$
is the number of non-zero elements in the Hessian $latex L^{(2)} (x)$$; i.e.,
the same as
$cref/nnz_h_lag/ipopt_xam_get_nlp_info/nnz_h_lag/$$.

$head iRow$$
If $icode values$$ is $code NULL$$,
$icode iRow$$ has size $icode nele_hess$$ and is set to the
row indices for the non-zero entries in the Hessian
$latex L^{(2)} (x)$$.

$head jCol$$
If $icode values$$ is $code NULL$$,
$icode jCol$$ has size $icode nele_hess$$ and is set to the
column indices for the non-zero entries in the Hessian
$latex L^{(2)} (x)$$.

$head values$$
If $icode values$$ is not $code NULL$$,
it has size $icode nele_hess$$ and $icode%values%[%k%]%$$
is set to the value of element of the Hessian $latex L^{(2)} (x)$$
with row index $icode%iRow%[%k%]%$$
and column index $icode%jRow%[%k%]%$$.

$head Example$$
$codep */
	virtual bool eval_h(
		Index         n              ,
		const Number* x              ,
		bool          new_x          ,
		Number        obj_factor     ,
		Index         m              ,
		const Number* lambda         ,
		bool          new_lambda     ,
		Index         nele_hess      ,
		Index*        iRow           ,
		Index*        jCol           ,
		Number*       values
	);
{
	assert( nele_hess == 2 );
	if( values == CPPAD_NULL )
	{
		iRow[0] = 1;
		jRow[0] = 1;
		//
		iRow[1] = 2;
		jRow[1] = 2;
 		//
		return true;
	}
	assert( n == 2 );
	assert( m == 1 );
	//
	values[0] = - 2.0 * lambda[0];
	values[1] = - 2.0 * obj_factor;
	//
	return true;
}
/* $$
$end
*/
}
