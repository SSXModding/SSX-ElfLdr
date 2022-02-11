#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <utils/Hash.h>
#include <utils/utils.h>

#include <cstdint>

namespace elfldr::util {

	// TODO:
	// - Multi dimension/linked buckets for hash collisions maybe
	//  (I may just throw out a ERL which does that.)
	//
	// - (should really do) dynamic growing, as needed
	//      Will be done probably after the Allocator API is finalized and Vector<T> is implemented
	//		We will also need to rehash all keys when resizing which sucks but meh

	/**
	 * A simple hash table. Doesn't handle collisions,
	 * and is probably boneheaded in design. It works though.
	 */
	template <class Key, class Value, class Hasher = util::Hash<Key>>
	struct HashTable {
		inline HashTable() = default;

		inline HashTable(std::size_t bucketSize) {
			Init(bucketSize);
		}

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
		 * This is a direct API that may clash
		 * with operator[]. Sorry.
		 */
		void Set(const Key& key, const Value& value) {
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
			if(!bucket->used)
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
			ELFLDR_VERIFY(bucket != nullptr);

			// Mark this bucket as used,
			// ala std::*_map<K,V>... Sorry :(
			if(!bucket->used) {
				bucket->used = true;
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
			bool used;
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

		// TODO: this should be using allocator

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

} // namespace elfldr

#endif // HASHTABLE_H