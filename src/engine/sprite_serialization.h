#pragma once

#include "containers/darray.h"
#include "serialization/slz.h"
#include "sprite.h"

bool animation_load_from_document(const Slz::Document& document, DynamicArray<Animation2D>& anims);