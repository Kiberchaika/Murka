# Murka
![](https://raw.githubusercontent.com/Kiberchaika/Murka/master/murka_logo.jpg?raw=true)

Murka is a lightweight UI library aimed at a modern, rich, GPU-accelerated and highly interactive audio plugin UI design. It supports JUCE 7 but it's not dependent on it.

Inspired by ImGUI, it's an "immediate mode" UI. It means that it doesn't require you to design an explicit hierarchy, you write your draw and interaction code in a single draw thread, without managing the widgets lifecycle or input thread interaction with the draw thread.

Murka is designed to make it as easy as possible to create custom widgets with real-time feedback. It requires some knowledge of OpenGL to use it to the fullest but provides drawing functions for fonts and basic shapes.

## Quickstart

Here's the steps to use Murka in JUCE:

1. Clone Murka somewhere.
2. In Projucer, add a new module and set the cloned Murka path as its path.
3. In the module settings, remove the check against "Use global path for this module"
4. Create a new JUCE component class, inherited from `murka::JuceMurkaBaseComponent`.

## Main component example

To use Murka in JUCE, inherit the `murka::JuceMurkaBaseComponent` and place all your Murka code in `draw()`.

**Like this:**

```cpp
#pragma once

#include <JuceHeader.h>
#include "juce_murka/JuceMurkaBaseComponent.h"

class MainComponent : public JuceMurkaBaseComponent
{
public:
	MainComponent() {
		setSize(600, 400);
	}

	void MainComponent::initialise() override {
		JuceMurkaBaseComponent::initialise();

		// load font from Projucer's BinaryData.cpp
		m.setFontFromRawData("InterRegular.ttf", BinaryData::InterRegular_ttf, BinaryData::InterRegular_ttfSize, 12);
	}

	void draw() override {
		m.clear(100, 100, 100);
		if (m.prepare<murka::Button>({ 10, 10, 150, 45 }).text("Button 1").draw().pressed) {
			DBG("Button 1 pressed!");
		}
		if (m.prepare<murka::Button>({ 10, 60, 150, 45 }).text("Button 2").draw().pressed) {
			DBG("Button 2 pressed!");
		}
	}

private:
  
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

```

## Examples

Please refer to examples in this repository.

[https://github.com/Kiberchaika/Murka_examples](https://github.com/Kiberchaika/Murka_examples)

## License

Murka is MIT licensed.

