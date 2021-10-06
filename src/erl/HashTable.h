#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cstdint>

namespace elfldr {
	
	
	// TODO: hash.h?
	
	// internal hash data type
	using Hash_t = std::uint32_t;
	
	template<class T> 
	struct Hash {
		//constexpr static Hash_t hash(const T&);
	};
	
	// hasher for strings
	template<>
	struct Hash<char*> {
		
		constexpr static Hash_t hash(const char*& str) {
			// TODO:
			return 32768;
		}
		
	};

	template<class Key, class Value, class Hasher = Hash<Key>>
	struct HashTable {
		
		/**
		 * Add a value to the hash table.
		 */
		void Add(const Key& key, const Value& value) {
			
		}
		
		bool HasKey(const Key& key) const {
				
		}
		
		
		private:
		
		struct Bucket {
			Key key;
			Value value;
		};
		 
		Bucket* MaybeGetBucket(const Key& key) const {
			 
			 
		}
		 
	 
		Bucket** buckets;
	};
	
	
	// HashTable<char*, erl::Symbol> should work.
	
	
}

#endif // HashTable.h