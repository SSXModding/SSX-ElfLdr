#ifndef BINMAP_H
#define BINMAP_H

#include <cstdint>

namespace elfldr {

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

			Entries[Index].key = key;
			Entries[Index].value = value;
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

			return &entry->value;
		}

	   private:
		struct MapEntry {
			TKey key;
			TValue value;
		};

		/**
		 * Maybe find a raw entry, with the given key.
		 *
		 * \param[in] key The key to find.
		 * \returns a pointer to the entry structure which has this key,
		 * 			or nullptr if not found
		 */
		constexpr MapEntry* MaybeFindEntry(TKey key) {
			for(uint32_t i = 0; i < GetCount(); ++i) {
				if(Entries[i].key == key)
					return &Entries[i];
			}

			return nullptr;
		}

		MapEntry Entries[MAX_ELEMENTS] {};
		uint32_t Index { 0 };
	};

} // namespace elfldr

#endif