// Child table for section dismod_at
document.write('\
<select onchange="dismod_at_child(this)">\
<option>dismod_at-&gt;</option>\
<option>math_abstract</option>\
<option>install</option>\
<option>get_started</option>\
<option>user</option>\
<option>database</option>\
<option>model</option>\
<option>command</option>\
<option>python</option>\
<option>whats_new_2019</option>\
<option>wish_list</option>\
<option>_reference</option>\
<option>_index</option>\
<option>_search</option>\
<option>_external</option>\
</select>\
');
function dismod_at_child(item)
{	var child_list = [
		'math_abstract.htm',
		'install.htm',
		'get_started.htm',
		'user.htm',
		'database.htm',
		'model.htm',
		'command.htm',
		'python.htm',
		'whats_new_2019.htm',
		'wish_list.htm',
		'_reference.htm',
		'_index.htm',
		'_search.htm',
		'_external.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
