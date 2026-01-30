#pragma once

namespace SK::Internal {

enum class execution_policy {
    scalar,
    word,
    sse,
    avx
};

}