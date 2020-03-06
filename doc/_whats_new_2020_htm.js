var list_up0 = [
'dismod_at.htm',
'release_notes.htm',
'whats_new_2020.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'whats_new_2020.htm#03-06',
'whats_new_2020.htm#02-29',
'whats_new_2020.htm#02-28',
'whats_new_2020.htm#02-26',
'whats_new_2020.htm#02-20',
'whats_new_2020.htm#02-18',
'whats_new_2020.htm#02-14',
'whats_new_2020.htm#02-13',
'whats_new_2020.htm#01-31',
'whats_new_2020.htm#01-16',
'whats_new_2020.htm#01-08',
'whats_new_2020.htm#01-07'
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
