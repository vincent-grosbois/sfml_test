#include "BoxedText.hpp"


BoxedText::BoxedText(const sf::Rect<float>& theBox, float textSize):
myText(""),
	//visualText("", textSize),
	myBox(theBox),
	tooBig(false),
	highlight(true),
	highlight_color(sf::Color::Black),
	highlight_shift(sf::Vector2f(1,1))
{
	visualText.setPosition(myBox.left, myBox.top);
};

bool BoxedText::setText(const std::string& theText) {

	myText = theText;
	visualText.setString(myText);

	SplitTextForWidth();
	SplitTextForHeight();
	
	return tooBig;
}

void BoxedText::SplitTextForHeight() {

	float height = myBox.height;
	int max_numb_lines = (int)floor( height / ( visualText.getCharacterSize() * 1.4f ) ) ;

	std::vector<int> line_break_pos = std::vector<int>();
	for(int i=0; i< myText.size(); ++i) {
		if (myText.compare(i,1,"\n") == 0 ) {
			line_break_pos.push_back(i);
		}
	}

	float total_height = visualText.findCharacterPos(myText.size()-1).y + visualText.getCharacterSize();
	int total_numb_lines = (int)floor( total_height / visualText.getCharacterSize() ) ;

	tooBig = total_numb_lines > max_numb_lines;

	if(max_numb_lines  >line_break_pos.size() )
		visualText.setString(myText);
	else
		visualText.setString(myText.substr(0,line_break_pos.at(max_numb_lines-1) ) ) ;

};
void BoxedText::SplitTextForWidth() {

	float width = myBox.width;

	std::vector<int> blank_space_pos = std::vector<int>();

	for(int i=0; i< myText.size(); ++i) {
		if (myText.compare(i,1," ") == 0 ) {
			blank_space_pos.push_back(i);
		}
	};

	for(int current_blank_space_index=1; current_blank_space_index < blank_space_pos.size();  ++current_blank_space_index)
	{
		if( visualText.findCharacterPos( ((size_t)blank_space_pos.at(current_blank_space_index) - 1 ) ).x > width) {
			myText.replace(blank_space_pos.at(current_blank_space_index-1),1, std::string("\n") );
			visualText.setString(myText);	
		}	
	}
};