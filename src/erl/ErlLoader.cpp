#include <erl/ErlLoader.h>

#include "elf.h"

#include "ErlAllocator.h"
#include "HashTable.h"

// if building debug, then we can afford to be a bit more chatty.
#ifdef DEBUG
	#include <utils.h> // libutils

	#define ERL_DEBUG_PRINTF(format, ...) elfldr::util::DebugOut("[Erl] " format, ##__VA_ARGS__)
#else
	#define ERL_DEBUG_PRINTF(format, ...)
#endif

namespace elfldr::erl {

	/**
	 * This class is actually what we allocate
	 * when we give an Image* to people.
	 * It includes all the private members we might need.
	 *
	 * Also provides the implementation of Image::ResolveSymbol.
	 */
	struct ImageImpl : public Image {
		~ImageImpl() {
			ERL_DEBUG_PRINTF("~ImageImpl()");

			// free the byte buffer for the erl.
			if(buffer)
				delete[] buffer;
		}

		Symbol ResolveSymbol(const char* symbolName) override {
			return -1;
		}

		std::uint8_t* buffer;
		HashTable<String, Symbol> symbol_table;
	};

	// TODO: put this in utils? it might be ok to have round

	template <class ScopeExitT>
	struct ScopeExitGuard {
		constexpr ScopeExitGuard(ScopeExitT se)
			: scope_exited(util::Move(se)) {
		}

		// call the exit functor
		constexpr ~ScopeExitGuard() {
			scope_exited();
		}

	   private:
		ScopeExitT scope_exited;
	};

	Image* LoadErl(const char* path) {
		auto* image = new ImageImpl();
		bool loaded = false;

		ScopeExitGuard guard([&]() {
			// if the image wasn't loaded properly,
			// destroy it.
			if(!loaded)
				delete image;
		});

		ERL_DEBUG_PRINTF("Attempting to load ERL \"%s\"", path);
	}

	void DestroyErl(Image* theImage) {
		if(theImage)
			delete theImage;
	}

} // namespace elfldr::erl