#pragma once

typedef struct {
	int top;
	int bottom;
	int left;
	int right;
}Bounds;

Bounds createBoundingBox(int top, int bottom, int left, int right);