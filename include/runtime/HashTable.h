#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <runtime/Hash.h>
#include <runtime/Utility.h>

namespace elfldr {

	// TODO:
	// - Multi dimension/linked buckets for hash collisions maybe
	//  (I may just throw out a ERL which does that.)
	//
	// - (should really do) dynamic growing, as needed
	//      Will be done probably after the Allocator API is finalized and Vector<T> is implemented
	//		We will also need to rehash all keys when resizing which sucks but meh

	/**
	 * A simple hash table/hashmap. Doesn't handle collisions,
	 * and is probably boneheaded in design. It works though.
	 *
	 * \tparam Key Key type. Must have a Hash implementation.
	 * \tparam Value The value type.
	 * \tparam Hash The Hash algorithm to use.
	 * \tparam Allocator The Allocator (see runtime/Allocator.h for the concept definition) to use.
	 */
	template <class Key, class Value, class Hash = Hash<Key>, template<class> class Allocator = StdAllocator>
	struct HashTable {
		inline HashTable() = default;

		inline HashTable(size_t bucketSize) {
			Init(bucketSize);
		}

		~HashTable() {
			if(bucket_size)
				FreeBuckets();
		}

		void Init(size_t size) {
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
			//
			// AKA: It's not an error condition
			// if we get here and the bucket isn't used.
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

		Allocator<Bucket> alloc;

		uint32_t HashKey(const Key& key) {
			return Hash::hash(key) % bucket_size;
		}

		Bucket* MaybeGetBucket(const Key& key) {
			if(!buckets)
				return nullptr;
			return &buckets[HashKey(key)];
		}

		void AllocBuckets(size_t size) {
			// avoid a memory leak by freeing any old buckets
			// beforehand.
			if(buckets)
				FreeBuckets();

			buckets = alloc.Allocate(size);

			// initialize storage
			for(size_t i = 0; i < size; ++i)
				alloc.Construct(&buckets[i]);

			bucket_size = size;
		}

		void FreeBuckets() {
			for(size_t i = 0; i < bucket_size; ++i)
				buckets[i].~Bucket();

			alloc.Deallocate(buckets);
			bucket_size = 0;
		}

		Bucket* buckets {};
		size_t bucket_size {};
	};

} // namespace elfldr

#endif // HASHTABLE_H