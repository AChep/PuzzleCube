#pragma once

namespace rutils_internal {
	void renderText(void*, const char*, float, float);
	void renderInt(void*, int, float, float, bool);
}

namespace rutils {
	void renderTime(void*, int, float, float);
	void renderText(void*, const char*, float, float);
	void renderInt(void*, int, float, float);
}