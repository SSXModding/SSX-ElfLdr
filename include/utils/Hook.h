/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_HOOK_H
#define ELFLDR_HOOK_H

namespace elfldr::util {

	namespace detail {
		/**
		 * Detail base helper for hooking a function.
		 * Don't call this directly, use the typed
		 * HookFunction() API. It's there for a reason :P
		 *
		 * \return Allocated trampoline for function.
		 * \param[out] dest Destination function to hook.
		 * \par
		 */
		void* HookFunctionBase(void* dest, const void* hook);
	} // namespace detail

	/**
	 * Hook a function.
	 * \tparam HookT Hook function pointer type. Also determines trampoline type.
	 * \return The trampoline. You can use this to call the original routine
	 * 			from your hook.
	 * 	\param[out] funcptr Function to hook. This function must be at least 5*sizeof(uint32_t) bytes long.
	 * 	\param[in] hook The hook routine.
	 */
	template <class HookT>
	constexpr HookT HookFunction(void* funcptr, const HookT hook) {
		return UBCast<HookT>(detail::HookFunctionBase(funcptr, UBCast<void*>(hook)));
	}

} // namespace elfldr

#endif // ELFLDR_HOOK_H
