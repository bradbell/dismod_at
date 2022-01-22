// Child table for section release_notes
document.write('\
<select onchange="release_notes_child(this)">\
<option>release_notes-&gt;</option>\
<option>whats_new_2022</option>\
<option>whats_new_2021</option>\
<option>whats_new_2020</option>\
<option>whats_new_2019</option>\
<option>whats_new_2018</option>\
<option>whats_new_2017</option>\
<option>whats_new_2016</option>\
<option>whats_new_2015</option>\
</select>\
');
function release_notes_child(item)
{	var child_list = [
		'whats_new_2022.htm',
		'whats_new_2021.htm',
		'whats_new_2020.htm',
		'whats_new_2019.htm',
		'whats_new_2018.htm',
		'whats_new_2017.htm',
		'whats_new_2016.htm',
		'whats_new_2015.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
