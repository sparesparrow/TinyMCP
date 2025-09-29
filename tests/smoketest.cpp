#include <iostream>
#include "Public/PublicDef.h"

int main() {
    // Minimal link-time check: ensure we can include a header and link lib
    std::cout << "TinyMCP smoketest OK" << std::endl;
    return 0;
}

