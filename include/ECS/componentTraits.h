#ifndef LSIM_COMPONENTTRAITS_H
#define LSIM_COMPONENTTRAITS_H

template<typename T>
struct ComponentTraits {
        static_assert(!sizeof(T), "ERROR: No ComponentTraits specialization found for T. did you forget to include the traits header?");
};

#endif //LSIM_COMPONENTTRAITS_H
