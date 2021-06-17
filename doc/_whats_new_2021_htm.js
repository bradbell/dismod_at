var list_up0 = [
'dismod_at.htm',
'release_notes.htm',
'whats_new_2021.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'whats_new_2021.htm#06-17',
'whats_new_2021.htm#06-15',
'whats_new_2021.htm#06-14',
'whats_new_2021.htm#06-13',
'whats_new_2021.htm#06-12',
'whats_new_2021.htm#06-11',
'whats_new_2021.htm#06-10',
'whats_new_2021.htm#06-09',
'whats_new_2021.htm#06-08',
'whats_new_2021.htm#06-07',
'whats_new_2021.htm#06-06',
'whats_new_2021.htm#06-01',
'whats_new_2021.htm#05-31',
'whats_new_2021.htm#05-29',
'whats_new_2021.htm#05-21',
'whats_new_2021.htm#05-20',
'whats_new_2021.htm#05-19',
'whats_new_2021.htm#05-18',
'whats_new_2021.htm#05-15',
'whats_new_2021.htm#05-07',
'whats_new_2021.htm#05-06',
'whats_new_2021.htm#04-16',
'whats_new_2021.htm#03-02',
'whats_new_2021.htm#01-28',
'whats_new_2021.htm#01-22',
'whats_new_2021.htm#01-21',
'whats_new_2021.htm#01-19',
'whats_new_2021.htm#01-11',
'whats_new_2021.htm#01-10',
'whats_new_2021.htm#01-09',
'whats_new_2021.htm#01-08'
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
