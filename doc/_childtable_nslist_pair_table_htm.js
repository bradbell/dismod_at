// Child table for section nslist_pair_table
document.write('\
<select onchange="nslist_pair_table_child(this)">\
<option>nslist_pair_table-&gt;</option>\
<option>nslist_pair_table.py</option>\
</select>\
');
function nslist_pair_table_child(item)
{	var child_list = [
		'nslist_pair_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
