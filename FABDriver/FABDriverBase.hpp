#ifndef FABDRIVERBASE_HPP_
#define FABDRIVERBASE_HPP_
//FABDriver and FABDriverBase implement looping of all points inside an FArrayBox
//with vectorisation and OpenMP.
//
//The reason why FABDriverBase is necessary is that if we want to inherit from one of the compute classes
//then the templating of the FABDriver with class compute_t becomes a problem when constructing the objects.
//FABDriverBase gives us all we need on the side of the computer but isn't templated.

//The following file has to be provided by the user.
//It must include c_NUM, the total number of simulation components.
#include "user_enum.hpp"
#include "always_inline.hpp"

#include <array>

// This exists solely to allow the compiler to
// perform type deduction on data_t.
template <typename t>
struct idx_t
{
    int m_idx;

    ALWAYS_INLINE
    idx_t (int idx) :
        m_idx (idx)
    {}

    ALWAYS_INLINE
    operator int() const
    {
        return m_idx;
    }
};

struct FABDriverBase
{
    const double *m_in_ptr[c_NUM];
    const int *m_in_lo;
    const int *m_in_hi;
    int m_in_stride[3];

    double *m_out_ptr[c_NUM];
    const int *m_out_lo;
    const int *m_out_hi;
    int m_out_stride[3];

    ALWAYS_INLINE
    int in_idx(int ix, int iy, int iz) const;
    
    ALWAYS_INLINE
    int out_idx(int ix, int iy, int iz) const;

    template <class data_t>
    std::array<data_t, c_NUM> local_vars(idx_t<data_t> idx) const;
};

#include "FABDriverBase.impl.hpp"

#endif /* FABDRIVERBASE_HPP_ */
