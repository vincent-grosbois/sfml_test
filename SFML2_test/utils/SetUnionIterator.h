#pragma once

#include <set>

//iterates over the union of 2 sets

template <typename T, typename D>
void iterate_over_union(std::set<T>& s1, std::set<T>& s2, D d)
{
    std::set<T>::iterator it1 = s1.begin();
    std::set<T>::iterator it2 = s2.begin();

    while (it1 != s1.end() && it2 != s2.end()) {
        if (*it1 < *it2) {
            // only in set1
            d.visit_first(*it1);
            ++it1;
        } else if (*it2 < *it1) {
            // only in set2
            d.visit_second(*it2);
            ++it2;
        } else {
            // in both
            d.visit_both(*it1, *it2);
            ++it1;
            ++it2;
        }
    }
    for (; it1 != s1.end(); ++it1) {
        // only in set1
        d.visit_first(*it1);
    }
    for (; it2 != s2.end(); ++it2) {
        // only in set2
        d.visit_second(*it2);
    }
}