#include <erl/ErlLoader.h>
#include <utils.h>

namespace elfldr::erl {

	static Malloc erl_malloc = nullptr;
	static Free erl_free = nullptr;

	void* ErlAlloc(std::uint32_t size) {
		//ELFLDR_RELASSERT(erl_malloc != nullptr)
		return erl_malloc(size);
	}

	void ErlFree(void* ptr) {
		//ELFLDR_RELASSERT(erl_free != nullptr)
		return erl_free(ptr);
	}

	void SetAllocationFunctions(Malloc erlmalloc, Free erlfree) {
		erl_malloc = erlmalloc;
		erl_free = erlfree;
	}

} // namespace elfldr::erl