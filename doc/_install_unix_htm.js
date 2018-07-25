var list_up0 = [
'dismod_at.htm',
'install_unix.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'install_unix.htm#System Requirements',
'install_unix.htm#System Requirements.C++ Compiler',
'install_unix.htm#System Requirements.Fortran Compiler',
'install_unix.htm#System Requirements.git',
'install_unix.htm#System Requirements.cmake',
'install_unix.htm#System Requirements.pkg-config',
'install_unix.htm#System Requirements.python3',
'install_unix.htm#System Requirements.sqlite3',
'install_unix.htm#System Requirements.gsl',
'install_unix.htm#System Requirements.suitesparse',
'install_unix.htm#Download dismod_at.git',
'install_unix.htm#Paths',
'install_unix.htm#Paths.Adding to a Path',
'install_unix.htm#Paths.PKG_CONFIG_PATH',
'install_unix.htm#Paths.LD_LIBRARY_PATH',
'install_unix.htm#Paths.PATH',
'install_unix.htm#Paths.PYTHONPATH',
'install_unix.htm#Special Requirements',
'install_unix.htm#Special Requirements.run_cmake.sh',
'install_unix.htm#Special Requirements.eigen',
'install_unix.htm#Special Requirements.ipopt',
'install_unix.htm#Special Requirements.CppAD',
'install_unix.htm#Special Requirements.cppad_mixed',
'install_unix.htm#dismod_at',
'install_unix.htm#dismod_at.Setup',
'install_unix.htm#dismod_at.Cmake Command',
'install_unix.htm#dismod_at.Build',
'install_unix.htm#dismod_at.Test',
'install_unix.htm#dismod_at.Install',
'install_unix.htm#Example'
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
var list_down0 = [
'run_cmake.sh.htm',
'example_install.sh.htm'
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
