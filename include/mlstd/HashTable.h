/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef MLSTD_HASHTABLE_H
#define MLSTD_HASHTABLE_H

#include <mlstd/Hash.h>
#include <mlstd/Utility.h>
#include "mlstd/Allocator.h"
#include "mlstd/DynamicArray.h"

namespace mlstd {

	// TODO:
	// - Quadratic probl

	/**
	 * A simple hash table/unordered map. Doesn't handle collisions,
	 * and is probably boneheaded in design. It works though.
	 *
	 * \tparam Key Key type. Must have a Hash implementation.
	 * \tparam Value The value type.
	 * \tparam Hash The Hash algorithm to use.
	 * \tparam Allocator The Allocator (see mlstd/Allocator.h for the concept definition) to use.
	 */
	template <class Key, class Value, class Hash = Hash<Key>, template <class> class Allocator = StdAllocator>
	struct HashTable {
		inline HashTable()  {
			// TODO?
			buckets.Resize(64);
		}

		/**
		 * Add a value to the hash table.
		 */
		void Insert(const Key& key, const Value& value) {
			auto* bucket = MaybeGetBucket(key);

			if(!bucket)
				return;

			// maybe util::Move both these values?
			bucket->key = key;
			bucket->value = value;
			bucket->used = true;
		}

		bool HasKey(const Key& key) {
			auto* bucket = MaybeGetBucket(key);

			if(!bucket)
				return false;

			// Fast path: if the bucket we retrieved
			// for this key isn't used, then
			// we definitely don't have the key,
			// and we don't need to bug Key::operator==.
			if(bucket->state != Bucket::State::Full)
				return false;

			return bucket->key == key;
		}

		Value* MaybeGet(const Key& key) {
			if(!HasKey(key))
				return nullptr;
			auto* bucket = MaybeGetBucket(key);
			return &bucket->value;
		}

		/**
		 * Array subscript operator.
		 */
		Value& operator[](const Key& k) {
			auto* bucket = MaybeGetBucket(k);
			MLSTD_VERIFY(bucket != nullptr);

			// Mark this bucket as used,
			// ala std::*_map<K,V>... Sorry :(
			//
			// AKA: It's not an error condition
			// if we get here and the bucket isn't used.
			if(!bucket->state == Bucket::State::Empty) {
				bucket->state = Bucket::State::Full;
				bucket->key = k;
			}

			return bucket->value;
		}

	   private:
		/**
		 * A single hash bucket.
		 */
		struct Bucket {
			Key key;
			Value value;
			enum class State : uint8_t {
				Empty,
				Full
			};

			State state{State::Empty};
		};

		uint32_t HashKey(const Key& key) {
			return Hash::hash(key) % buckets.Size();
		}

		Bucket* MaybeGetBucket(const Key& key) {
			if(!buckets.Size())
				return nullptr;
			
			return &buckets[HashKey(key)];
		}

		DynamicArray<Bucket, Allocator<Bucket>> buckets;
	};

} // namespace mlstd

#endif // MLSTD_HASHTABLE_H