// Child table for section nslist_table
document.write('\
<select onchange="nslist_table_child(this)">\
<option>nslist_table-&gt;</option>\
<option>nslist_table.py</option>\
</select>\
');
function nslist_table_child(item)
{	var child_list = [
		'nslist_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
