#pragma once

#include "json_lexer.h"
#include "serialization/slz.h"

namespace Json
{

bool parse_tokens(const DynamicArray<Token>& tokens, const String content, Slz::Document& out);
bool parse_string(const String content, Slz::Document& out);

} // namespace Json
