var list_up0 = [
'dismod_at.htm',
'whats_new_2019.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'whats_new_2019.htm#Contents',
'whats_new_2019.htm#06-26',
'whats_new_2019.htm#06-25',
'whats_new_2019.htm#06-24',
'whats_new_2019.htm#06-23',
'whats_new_2019.htm#06-22',
'whats_new_2019.htm#06-21',
'whats_new_2019.htm#06-07',
'whats_new_2019.htm#06-02',
'whats_new_2019.htm#06-01',
'whats_new_2019.htm#05-24',
'whats_new_2019.htm#05-21',
'whats_new_2019.htm#05-12',
'whats_new_2019.htm#05-07',
'whats_new_2019.htm#05-05',
'whats_new_2019.htm#05-03',
'whats_new_2019.htm#05-02',
'whats_new_2019.htm#05-01',
'whats_new_2019.htm#04-30',
'whats_new_2019.htm#04-29',
'whats_new_2019.htm#04-28',
'whats_new_2019.htm#04-27',
'whats_new_2019.htm#04-26',
'whats_new_2019.htm#04-25',
'whats_new_2019.htm#04-24',
'whats_new_2019.htm#04-23',
'whats_new_2019.htm#04-22',
'whats_new_2019.htm#04-21',
'whats_new_2019.htm#04-20',
'whats_new_2019.htm#04-18',
'whats_new_2019.htm#04-17',
'whats_new_2019.htm#04-15',
'whats_new_2019.htm#04-14',
'whats_new_2019.htm#04-12',
'whats_new_2019.htm#04-11',
'whats_new_2019.htm#04-10',
'whats_new_2019.htm#04-09',
'whats_new_2019.htm#04-08',
'whats_new_2019.htm#04-06',
'whats_new_2019.htm#02-16',
'whats_new_2019.htm#01-26',
'whats_new_2019.htm#01-23',
'whats_new_2019.htm#01-22',
'whats_new_2019.htm#01-16',
'whats_new_2019.htm#01-15',
'whats_new_2019.htm#01-10'
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
