#ifndef KIEDIS_UTILS_H
#define KIEDIS_UTILS_H

namespace kiedis
{
    // set the file discriptor non-blocking. return false if failed
    bool set_non_blocking(int sock);
} // namespace kiedis

#endif