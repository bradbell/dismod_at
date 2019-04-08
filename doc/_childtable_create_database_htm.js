// Child table for section create_database
document.write('\
<select onchange="create_database_child(this)">\
<option>create_database-&gt;</option>\
<option>create_database.py</option>\
</select>\
');
function create_database_child(item)
{	var child_list = [
		'create_database.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
