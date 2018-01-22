var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_up0 = [
'dismod_at.htm',
'install_unix.htm',
'run_cmake.sh.htm'
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
'run_cmake.sh.htm',
'example_install.sh.htm'
];
var list_current0 = [
'run_cmake.sh.htm#verbose_makefile',
'run_cmake.sh.htm#build_type',
'run_cmake.sh.htm#dismod_at_prefix',
'run_cmake.sh.htm#eigen_prefix',
'run_cmake.sh.htm#Other Prefixes',
'run_cmake.sh.htm#Debug and Release',
'run_cmake.sh.htm#python3_executable',
'run_cmake.sh.htm#Choosing C++ Compiler',
'run_cmake.sh.htm#extra_cxx_flags',
'run_cmake.sh.htm#cmake_libdir',
'run_cmake.sh.htm#checkpoint_newton_step',
'run_cmake.sh.htm#system_specific_library_list'
];
function choose_across0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_across0[index-1];
}
function choose_up0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_up0[index-1];
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
function choose_current0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_current0[index-1];
}
