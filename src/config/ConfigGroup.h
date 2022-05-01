
namespace elfldr::config {

	// a group in a config filr.
	struct ConfigGroup {
		template <class T>
		T& GetValue(const char* key);

		/**
		 * Get another config group.
		 */
		ConfigGroup& GetGroup(const char* subgroup);
	};

} // namespace elfldr::config