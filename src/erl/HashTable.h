#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cstdint>

#include <utils/utils.h>
#include <utils/Hash.h>

namespace elfldr::erl {

	// TODO:
	// multi dim buckets for hash collisions maybe
	// (I may just throw out a ERL which does that.)

	/**
	 * A simple hash table.
	 */
	template <class Key, class Value, class Hasher = util::Hash<Key>>
	struct HashTable {
		inline HashTable() = default;

		~HashTable() {
			if(bucket_size)
				FreeBuckets();
		}

		void Init(std::size_t size) {
			if(!bucket_size)
				AllocBuckets(size);
		}

		/**
		 * Add a value to the hash table.
		 */
		void Set(const Key& key, Value value) {
			auto* bucket = MaybeGetBucket(key);

			if(!bucket)
				return;

			//util::DebugOut("hashTable chose bucket %d", (bucket - &buckets[0]));

			// maybe util::Move both these values?
			bucket->key = key;
			bucket->value = value;
		}

		bool HasKey(const Key& key)  {
			auto* bucket = MaybeGetBucket(key);

			if(!bucket)
				return false;

			return bucket->key == key;
		}

		Value* MaybeGet(const Key& key) {
			if(!HasKey(key))
				return nullptr;
			auto* bucket = MaybeGetBucket(key);

			//util::DebugOut("hashTable chose bucket %d", (bucket - &buckets[0]));
			//util::DebugOut("hashTable key is %s, value is %08x", bucket->key.c_str(), bucket->value);

			return &bucket->value;
		}

	   private:
		struct Bucket {
			Key key;
			Value value;
		};

		std::uint32_t HashKey(const Key& key) {
			return Hasher::hash(key) % bucket_size;
		}

		Bucket* MaybeGetBucket(const Key& key) {
			if(!buckets)
				return nullptr;

			// this might be wrong. idk though
			return &buckets[HashKey(key)];
		}

		void AllocBuckets(std::size_t size) {
			// avoid a memory leak by freeing any old buckets
			// beforehand.
			if(buckets)
				FreeBuckets();

			buckets = new Bucket[size];
			bucket_size = size;
		}

		void FreeBuckets() {
			delete[] buckets;
			bucket_size = 0;
		}

		Bucket* buckets {};
		std::size_t bucket_size {};
	};

} // namespace elfldr::erl

#endif // HASHTABLE_H