#include <erl/ErlLoader.h>

#include "ErlAllocator.h"

namespace elfldr::erl {
	
	/**
	 * This class is actually what we allocate
	 * when we give an Image* to people.
	 *
	 * Also provides the implementaiton of Image::ResolveSymbol.
	 */
	struct ImageInternal : public Image {
		
		~ImageInternal() {
			// TODO: dealloc stuff.
		}
			
		void* ResolveSymbol(const char* symbolName) override {
			
		}
	
	};

	Image* LoadErl(const char* path) {
		return new ImageInternal();
	}

	void DestroyErl(Image* theImage) {
		if(theImage)
			delete theImage;
	}
	
} // namespace elfldr::erl