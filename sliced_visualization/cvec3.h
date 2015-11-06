#ifndef _CVEC3_H_
#define _CVEC3_H_

#include <glm/glm.hpp>

/* cvec3.h
 *
 * Wrapper class for glm::vec3 so that it can be put in ordered containers
 *
 */
class cvec3 : public glm::vec3 {
    // Inherit constructors
    using glm::vec3::vec3;

    public:
        inline bool operator<(const cvec3 &other) const
        {
            return x != other.x ? x < other.x
                   : y != other.y ? y < other.y
                   : z < other.z;
        }

        inline bool operator>(const cvec3 &other) const
        {
            return x != other.x ? x > other.x
                   : y != other.y ? y > other.y
                   : z > other.z;
        }

        inline bool operator==(const cvec3 &other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }

        inline bool operator!=(const cvec3 &other) const
        {
            return x != other.x || y != other.y || z != other.z;
        }
};

#endif
