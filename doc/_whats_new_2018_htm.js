var list_up0 = [
'dismod_at.htm',
'whats_new_2018.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'whats_new_2018.htm#Contents',
'whats_new_2018.htm#05-0-8',
'whats_new_2018.htm#05-07',
'whats_new_2018.htm#05-05',
'whats_new_2018.htm#05-04',
'whats_new_2018.htm#05-02',
'whats_new_2018.htm#04-30',
'whats_new_2018.htm#04-26',
'whats_new_2018.htm#04-25',
'whats_new_2018.htm#04-24',
'whats_new_2018.htm#04-23',
'whats_new_2018.htm#04-22',
'whats_new_2018.htm#04-21',
'whats_new_2018.htm#04-16',
'whats_new_2018.htm#04-13',
'whats_new_2018.htm#04-12',
'whats_new_2018.htm#04-11',
'whats_new_2018.htm#04-10',
'whats_new_2018.htm#04-09',
'whats_new_2018.htm#04-08',
'whats_new_2018.htm#04-07',
'whats_new_2018.htm#04-06',
'whats_new_2018.htm#03-21',
'whats_new_2018.htm#03-13',
'whats_new_2018.htm#03-10',
'whats_new_2018.htm#03-08',
'whats_new_2018.htm#03-06',
'whats_new_2018.htm#02-20',
'whats_new_2018.htm#02-17',
'whats_new_2018.htm#02-14',
'whats_new_2018.htm#02-13',
'whats_new_2018.htm#02-12',
'whats_new_2018.htm#02-11',
'whats_new_2018.htm#02-09',
'whats_new_2018.htm#02-08',
'whats_new_2018.htm#02-05',
'whats_new_2018.htm#02-04',
'whats_new_2018.htm#02-03',
'whats_new_2018.htm#02-02',
'whats_new_2018.htm#02-01',
'whats_new_2018.htm#01-31',
'whats_new_2018.htm#01-30',
'whats_new_2018.htm#01-28',
'whats_new_2018.htm#01-26',
'whats_new_2018.htm#01-25',
'whats_new_2018.htm#01-24',
'whats_new_2018.htm#01-23',
'whats_new_2018.htm#01-22',
'whats_new_2018.htm#01-21',
'whats_new_2018.htm#01-20',
'whats_new_2018.htm#01-18',
'whats_new_2018.htm#01-16',
'whats_new_2018.htm#01-12',
'whats_new_2018.htm#01-09'
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
'whats_new_2017.htm',
'whats_new_2016.htm',
'whats_new_2015.htm'
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
