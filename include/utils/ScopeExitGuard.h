//
// Created by lily on 1/9/22.
//

#ifndef ELFLDR_SCOPEEXITGUARD_H
#define ELFLDR_SCOPEEXITGUARD_H

#include <utils/utils.h>

namespace elfldr::util {

	/**
	 * A scope exit guard.
	 */
	template <class ScopeExitT>
	struct ScopeExitGuard {
		constexpr ScopeExitGuard(ScopeExitT se)
			: scope_exited(util::Move(se)) {
		}

		constexpr ~ScopeExitGuard() {
			if(should_call)
				scope_exited();
		}

		/**
		 * Make this ScopeExitGuard not call the attached
		 * function on exit. This can be done for instance,
		 * if a function is successfully returning some heap
		 * value and doesn't need to free it anymore.
		 */
		void DontCall() {
			should_call = false;
		}

	   private:
		ScopeExitT scope_exited;
		bool should_call { true };
	};

} // namespace elfldr::util

#endif // ELFLDR_SCOPEEXITGUARD_H
