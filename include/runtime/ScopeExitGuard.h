/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_SCOPEEXITGUARD_H
#define ELFLDR_SCOPEEXITGUARD_H

#include <utils/Utils.h>

namespace elfldr {

	/**
	 * A scope exit guard.
	 * This allows code to be run when a scope exits,
	 * e.g: when a function is returning.
	 * It can also be aborted if it does not need to run.
	 */
	template <class ScopeExitT>
	struct ScopeExitGuard {
		constexpr ScopeExitGuard(ScopeExitT se)
			: scope_exited(elfldr::Move(se)) {
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

} // namespace elfldr

#endif // ELFLDR_SCOPEEXITGUARD_H
