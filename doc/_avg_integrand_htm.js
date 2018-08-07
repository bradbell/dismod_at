var list_up0 = [
'dismod_at.htm',
'model.htm',
'avg_integrand.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'avg_integrand.htm#Ordinary Differential Equation',
'avg_integrand.htm#Data or Avgint Table Notation',
'avg_integrand.htm#Data or Avgint Table Notation.i',
'avg_integrand.htm#Data or Avgint Table Notation.a_i',
'avg_integrand.htm#Data or Avgint Table Notation.b_i',
'avg_integrand.htm#Data or Avgint Table Notation.s_i',
'avg_integrand.htm#Data or Avgint Table Notation.t_i',
'avg_integrand.htm#Data or Avgint Table Notation.x_ij',
'avg_integrand.htm#Data or Avgint Table Notation.w_i',
'avg_integrand.htm#Data or Avgint Table Notation.n_i',
'avg_integrand.htm#Rate Functions',
'avg_integrand.htm#Rate Functions.Parent Rate, q_k',
'avg_integrand.htm#Rate Functions.Child Rate Effect, u_ik',
'avg_integrand.htm#Rate Functions.J_k',
'avg_integrand.htm#Rate Functions.Rate Covariate Multiplier, alpha_jk',
'avg_integrand.htm#Rate Functions.Adjusted Rate, r_ik',
'avg_integrand.htm#Rate Functions.pini, p_i0(t)',
'avg_integrand.htm#Rate Functions.iota_i(a,t)',
'avg_integrand.htm#Rate Functions.rho_i(a,t)',
'avg_integrand.htm#Rate Functions.chi_i(a,t)',
'avg_integrand.htm#Rate Functions.omega_i(a,t)',
'avg_integrand.htm#S_i(a,t)',
'avg_integrand.htm#C_i(a,t)',
'avg_integrand.htm#Differential Equation',
'avg_integrand.htm#Integrand, I_i(a,t)',
'avg_integrand.htm#Integrand, I_i(a,t).Sincidence',
'avg_integrand.htm#Integrand, I_i(a,t).remission',
'avg_integrand.htm#Integrand, I_i(a,t).mtexcess',
'avg_integrand.htm#Integrand, I_i(a,t).mtother',
'avg_integrand.htm#Integrand, I_i(a,t).mtwith',
'avg_integrand.htm#Integrand, I_i(a,t).susceptible',
'avg_integrand.htm#Integrand, I_i(a,t).withC',
'avg_integrand.htm#Integrand, I_i(a,t).prevalence',
'avg_integrand.htm#Integrand, I_i(a,t).Tincidence',
'avg_integrand.htm#Integrand, I_i(a,t).mtspecific',
'avg_integrand.htm#Integrand, I_i(a,t).mtall',
'avg_integrand.htm#Integrand, I_i(a,t).mtstandard',
'avg_integrand.htm#Integrand, I_i(a,t).relrisk',
'avg_integrand.htm#Measurement Value Covariates',
'avg_integrand.htm#Measurement Value Covariates.K_i',
'avg_integrand.htm#Measurement Value Covariates.Multiplier, beta_j',
'avg_integrand.htm#Measurement Value Covariates.Measurement Effect',
'avg_integrand.htm#Weight Integral, wbar_i',
'avg_integrand.htm#Average Integrand, A_i'
];
var list_down2 = [
'install_unix.htm',
'get_started.htm',
'user.htm',
'database.htm',
'model.htm',
'command.htm',
'python.htm',
'devel.htm',
'whats_new_2018.htm',
'wish_list.htm'
];
var list_down1 = [
'model_variables.htm',
'avg_integrand.htm',
'data_like.htm',
'fixed_value.htm',
'fixed_diff.htm',
'fixed_prior.htm',
'random_value.htm',
'random_diff.htm',
'random_prior.htm',
'statistic.htm',
'bilinear.htm',
'ode_grid.htm',
'posterior.htm',
'prev_dep.htm'
];
function choose_up0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_up0[index-1];
}
function choose_across0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_across0[index-1];
}
function choose_current0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_current0[index-1];
}
function choose_down2(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down2[index-1];
}
function choose_down1(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down1[index-1];
}
function choose_down0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down0[index-1];
}
