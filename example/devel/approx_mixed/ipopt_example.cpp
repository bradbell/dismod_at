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
the lower bounds for $icode x$$ (has size $icode n$$).

$head x_u$$
the upper bounds for $icode x$$ (has size $icode n$$).

$head m$$
is the number of constraints in the problem (dimension of g(x)).

$head g_l$$
the lower bounds for $icode g(x)$$ (has size $icode m$$).

$head g_u$$
the upper bounds for $icode g(x)$$ (has size $icode m$$).

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
*/
}
