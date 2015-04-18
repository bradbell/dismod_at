// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef IPOPT_FIXED_NLP_HPP
# define IPOPT_FIXED_NLP_HPP

/*
$begin ipopt_fixed_nlp$$
$spell
	Ipopt
$$

$section Ipopt Example: Declare Non-linear Program Problem Class$$

$end
-----------------------------------------------------------------------------
*/
# include <coin/IpTNLP.hpp>

namespace { // BEGIN_DISMOD_AT_NAMESPACE
//
// ipopt_fixed_nlp
class ipopt_fixed_nlp : public Ipopt::TNLP
{
public:
	// Ipopt types used by this class
	typedef Ipopt::Number  Number;
	typedef Ipopt::Index   Index;
	typedef Ipopt::TNLP::IndexStyleEnum IndexStyleEnum;
	//
	// did finalize_solution agree that the solution had converged
	bool finalize_solution_ok_;
	//
	// default constructor
	ipopt_fixed_nlp(void);
	//
	// default destructor
	virtual ~ipopt_fixed_nlp(void);
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
} // END_DISMOD_AT_NAMESPACE
