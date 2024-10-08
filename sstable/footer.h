//
// Created by bokket on 24-3-26.
//

#pragma once

#include "format.h"
#include <string>
#include <string_view>

#include "../Log/Log.h"
#include "../db/status.h"

namespace bokket {

// echo https://github.com/bokket/Kirsidb | sha1sum
static const uint64_t kTableMagicNumber = 0x41ac5b09a8323be3ull;

class Footer {
public:
	Footer() = default;

	Footer(const BlockHandle& meta, const BlockHandle& index)
		: meta_block_{meta}
		, index_block_{index} { }

	void EncodeTo(std::string& dst);

	std::string EncodeToStringIndex() const;

	std::string EncodeToString() const;

	static Status DecodeFrom(std::string& input, Footer& footer);

	//void setFilterBlockMetaData();

	void DebugString();

	BlockHandle meta_block_;
	BlockHandle index_block_;
};

} // namespace bokket
