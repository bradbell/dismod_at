var list_up0 = [
'dismod_at.htm',
'whats_new_2018.htm',
'whats_new_2017.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'whats_new_2017.htm#12-30',
'whats_new_2017.htm#12-22',
'whats_new_2017.htm#12-21',
'whats_new_2017.htm#12-20',
'whats_new_2017.htm#12-16',
'whats_new_2017.htm#12-13',
'whats_new_2017.htm#12-10',
'whats_new_2017.htm#11-12',
'whats_new_2017.htm#11-08',
'whats_new_2017.htm#10-25',
'whats_new_2017.htm#10-24',
'whats_new_2017.htm#10-22',
'whats_new_2017.htm#10-21',
'whats_new_2017.htm#10-20',
'whats_new_2017.htm#10-18',
'whats_new_2017.htm#10-17',
'whats_new_2017.htm#10-14',
'whats_new_2017.htm#10-13',
'whats_new_2017.htm#10-09',
'whats_new_2017.htm#10-06',
'whats_new_2017.htm#10-05',
'whats_new_2017.htm#10-04',
'whats_new_2017.htm#10-02',
'whats_new_2017.htm#10-01',
'whats_new_2017.htm#09-30',
'whats_new_2017.htm#09-29',
'whats_new_2017.htm#09-28',
'whats_new_2017.htm#09-27',
'whats_new_2017.htm#09-26',
'whats_new_2017.htm#09-25',
'whats_new_2017.htm#09-23',
'whats_new_2017.htm#09-21',
'whats_new_2017.htm#09-20',
'whats_new_2017.htm#09-19',
'whats_new_2017.htm#09-18',
'whats_new_2017.htm#09-17',
'whats_new_2017.htm#09-16',
'whats_new_2017.htm#09-02',
'whats_new_2017.htm#09-01',
'whats_new_2017.htm#08-30',
'whats_new_2017.htm#08-07',
'whats_new_2017.htm#05-23',
'whats_new_2017.htm#05-05',
'whats_new_2017.htm#05-02',
'whats_new_2017.htm#05-01',
'whats_new_2017.htm#04-29',
'whats_new_2017.htm#04-27',
'whats_new_2017.htm#04-26',
'whats_new_2017.htm#04-25',
'whats_new_2017.htm#04-24',
'whats_new_2017.htm#04-23',
'whats_new_2017.htm#04-22',
'whats_new_2017.htm#04-21',
'whats_new_2017.htm#04-20',
'whats_new_2017.htm#04-19',
'whats_new_2017.htm#04-18',
'whats_new_2017.htm#04-17',
'whats_new_2017.htm#04-16',
'whats_new_2017.htm#04-15',
'whats_new_2017.htm#04-09',
'whats_new_2017.htm#04-07',
'whats_new_2017.htm#04-05',
'whats_new_2017.htm#04-04',
'whats_new_2017.htm#04-03',
'whats_new_2017.htm#04-01',
'whats_new_2017.htm#03-30',
'whats_new_2017.htm#03-29',
'whats_new_2017.htm#03-17',
'whats_new_2017.htm#03-06',
'whats_new_2017.htm#03-04',
'whats_new_2017.htm#03-03',
'whats_new_2017.htm#01-27',
'whats_new_2017.htm#01-26',
'whats_new_2017.htm#01-24',
'whats_new_2017.htm#01-14',
'whats_new_2017.htm#01-15'
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
