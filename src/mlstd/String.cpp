/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// Explicitly instantiates the String classes,
// so that only one copy ALWAYS ends up in the binary.

#include <mlstd/String.h>

template struct mlstd::BasicStringView<char>;
template struct mlstd::BasicString<char>;