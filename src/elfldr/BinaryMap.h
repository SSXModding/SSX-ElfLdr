/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef BINMAP_H
#define BINMAP_H

#include <stdint.h>

namespace elfldr {

	// This file will probably be deprecated soon

	/**
	 * A static compile-time map which can only hold a max amount of elements.
	 * Uses a very simple, and slow (well.. it's fast on the EE, so..)
	 * binary search algorithm. Zero-allocation as well, which is important.
	 *
	 * \tparam TKey Key type.
	 * \tparam TValue Value type.
	 * \tparam MAX_ELEMENTS Max amount of elements this map can store.
	 */
	template <class TKey, class TValue, uint32_t MAX_ELEMENTS>
	struct BinaryMap {
		// traits
		using KeyType = TKey;
		using ValueType = TValue;
		constexpr static uint32_t MaxElements = MAX_ELEMENTS;

		/**
		 * Insert a value.
		 *
		 * \param[in] key The key. Checked for duplicates.
		 * \param[in] value The value.
		 */
		constexpr void Insert(TKey key, const TValue& value) {
			// Avoid duplicate entries.
			if(MaybeFindEntry(key) != nullptr)
				return;

			// TODO: if index goes over MAX_ELEMENTS, scream
			Keys[Index] = key;
			Values[Index] = value;
			Index++;
		}

		/**
		 * Get the amount of elements stored.
		 *
		 * \returns Amount of elements stored.
		 */
		constexpr uint32_t GetCount() const {
			return Index;
		}

		/**
		 * Maybe get a value by key.
		 *
		 * \param[in] key The key.
		 * \returns A pointer to the value on success, nullptr othewise.
		 */
		constexpr TValue* MaybeGetValue(TKey key) {
			auto* entry = MaybeFindEntry(key);
			if(!entry)
				return nullptr;

			return entry;
		}

		constexpr TKey* GetKeys() {
			return &Keys[0];
		}

		/**
		 * Retrieve all values.
		 * This can be used to get all contained values.
		 */
		constexpr TValue* GetValues() {
			return &Values[0];
		}

	   private:
		/**
		 * Maybe find a raw entry, with the given key.
		 *
		 * \param[in] key The key to find.
		 * \returns a pointer to the entry structure which has this key,
		 * 			or nullptr if not found
		 */
		constexpr TValue* MaybeFindEntry(TKey key) {
			for(uint32_t i = 0; i < GetCount(); ++i) {
				if(Keys[i] == key)
					return &Values[i];
			}

			return nullptr;
		}
		// seperate so they both are stored
		// in a linear order
		TKey Keys[MAX_ELEMENTS] {};
		TValue Values[MAX_ELEMENTS] {};
		uint32_t Index { 0 };
	};

} // namespace elfldr

#endif