#pragma once
#include "BoxedText.hpp"
class BoxedTextScrollable : public BoxedText
{
public:
	BoxedTextScrollable(const sf::Rect<float>& theBox, float textSize);
	void goToNextText() { goToText(current_pos+1) ;};
	void goToPreviousText() { goToText(current_pos-1) ;};
	bool isLast() const { return current_pos == (string_list.size() - 1) ; };

protected:
	std::vector<std::string> string_list;
	int current_pos;
	void goToText(int pos);
	virtual void SplitTextForHeight();
};

