#include "BoxedTextScrollable.hpp"


BoxedTextScrollable::BoxedTextScrollable(const sf::Rect<float>& theBox, float textSize):
BoxedText(theBox, textSize), string_list(), current_pos(0)
{
};

void BoxedTextScrollable::SplitTextForHeight() {

	float height = myBox.height;
	
	if(height < 0) height = 0;

	int max_numb_lines = (int)floor( height / ( visualText.getCharacterSize() * 1.3f ) ) ;

	std::vector<int> line_break_pos = std::vector<int>();

	//add the begining of the string -1:
	line_break_pos.push_back(-1);

	for(int i=0; i< myText.size(); ++i) {
		if (myText.compare(i,1,"\n") == 0 ) {
			line_break_pos.push_back(i);
		}
	};

	//add the end of the string +1:
	line_break_pos.push_back(myText.size());

	float total_height = visualText.findCharacterPos(myText.size()-1).y + visualText.getCharacterSize();
	int total_numb_lines = (int)floor( total_height / visualText.getCharacterSize() ) ;

	tooBig = false;


	int number_of_screens = (int)ceil( (float)total_numb_lines / (float)max_numb_lines);
	int line_number_start = 0;
	int line_number_finish = max_numb_lines;

	for(int i = 0; i<number_of_screens; ++i) {

	if(line_number_finish>total_numb_lines) line_number_finish = total_numb_lines;

	string_list.push_back(myText.substr(line_break_pos.at(line_number_start)+1, line_break_pos.at(line_number_finish)-line_break_pos.at(line_number_start) ) );
	line_number_start +=max_numb_lines;
	line_number_finish +=max_numb_lines;
	}

	visualText.setString(string_list.at(0));
};

void BoxedTextScrollable::goToText(int pos) {
	
	if(pos > ((int)string_list.size())-1)
		pos = string_list.size()-1;
	else if(pos < 0)
		pos = 0;

	current_pos = pos;

	visualText.setString(string_list.at(current_pos));

	
};