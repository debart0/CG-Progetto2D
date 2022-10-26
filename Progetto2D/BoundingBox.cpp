#include "BoundingBox.h"

Bounds createBoundingBox(int top, int bottom, int left, int right) {
	Bounds boundingBox; 
	boundingBox.top = top;
	boundingBox.bottom = bottom;
	boundingBox.left = left;
	boundingBox.right = right;

	return boundingBox;
}