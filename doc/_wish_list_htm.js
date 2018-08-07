var list_up0 = [
'dismod_at.htm',
'wish_list.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'wish_list.htm#csv2db Command',
'wish_list.htm#csv2db Command.Covariate Multipliers',
'wish_list.htm#csv2db Command.Random Effects',
'wish_list.htm#csv2db Command.Grids',
'wish_list.htm#csv2db Command.Value Priors',
'wish_list.htm#csv2db Command.Difference Priors',
'wish_list.htm#csv2db Command.Sample',
'wish_list.htm#ODE Solution',
'wish_list.htm#ODE Solution.Prevalence ODE',
'wish_list.htm#ODE Solution.Step Size',
'wish_list.htm#ODE Solution.Large Excess Mortality',
'wish_list.htm#ODE Solution.Conditionals',
'wish_list.htm#ODE Solution.Checkpointing',
'wish_list.htm#meas_std',
'wish_list.htm#Command Diagrams',
'wish_list.htm#Lagrange Multipliers',
'wish_list.htm#Documentation',
'wish_list.htm#Documentation.Real World Example',
'wish_list.htm#Documentation.User Examples',
'wish_list.htm#Documentation.Model Documentation',
'wish_list.htm#Sampling Random Effects',
'wish_list.htm#Special Random Effects Hessian',
'wish_list.htm#Random Starting Point',
'wish_list.htm#Tracing Optimization',
'wish_list.htm#Windows Install'
];
var list_down1 = [
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
