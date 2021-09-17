#ifndef BINMAP_H
#define BINMAP_H

#include <cstdint>

namespace elfldr {

	template<class TKey, class TValue, uint32_t MAX_ELEMENTS>
	struct BinaryMap {
		
		// traits
		using KeyType = TKey;
		using ValueType = TValue;
		constexpr static uint32_t MaxElements = MAX_ELEMENTS;
		
		constexpr BinaryMap() {
			Index = 0;
		}
		
		constexpr void Insert(TKey key, const TValue& value) {
			// avoid duplicate
			if(MaybeFindEntry(key) != nullptr)
				return;
			
			Entries[Index].key = key;
			Entries[Index].value = value;
			Index++;
		}
		
		constexpr uint32_t GetCount() const {
			return Index;
		}
		
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
		
		constexpr MapEntry* MaybeFindEntry(TKey key) {
			for(uint32_t i = 0; i < GetCount(); ++i) {
				if(Entries[i].key == key)
					return &Entries[i];
			}
			
			return nullptr;
		}
		
		MapEntry Entries[MAX_ELEMENTS]{};
		uint32_t Index;
	};
	
}

#endif