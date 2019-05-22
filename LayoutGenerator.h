#pragma once

#include "MurkaShapes.h"

class LayoutGenerator {
public:
	LayoutGenerator() {

	};

	MurkaShape currentShape;

	void restart(MurkaShape startShape, int LAYOUT_TYPE /* It's a question what the layout types should be */) {

	}

	// This function returns the next shape and iterates the generator.
	// Layout type is a 2d table descriptor, each element being either a fixed size or a portion of 
	// the size of the context with which that state was initiated.
	MurkaShape nextShape() {
		// Updates the position in this generator's 2d table
	}
};