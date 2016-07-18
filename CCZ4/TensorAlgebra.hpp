#ifndef TENSORALGEBRA_HPP_
#define TENSORALGEBRA_HPP_

#include "always_inline.hpp"
#include "tensor.hpp"

class TensorAlgebra
{
public:
    template <class data_t>
    static tensor<2, data_t>
    compute_inverse(const tensor<2,data_t,3>& matrix ) //This function only works for 3D matrix
    {
        //TODO, FIXME: this function currently assumes a symmetic tensor ... change this?
        data_t deth = matrix[0][0]*matrix[1][1]*matrix[2][2] + 2*matrix[0][1]*matrix[0][2]*matrix[1][2] - matrix[0][0]*matrix[1][2]*matrix[1][2] - matrix[1][1]*matrix[0][2]*matrix[0][2] - matrix[2][2]*matrix[0][1]*matrix[0][1];
        tensor<2, data_t> h_UU;
        {
            h_UU[0][0] = (matrix[1][1]*matrix[2][2] - matrix[1][2]*matrix[1][2]) / deth;
            h_UU[0][1] = (matrix[0][2]*matrix[1][2] - matrix[0][1]*matrix[2][2]) / deth;
            h_UU[0][2] = (matrix[0][1]*matrix[1][2] - matrix[0][2]*matrix[1][1]) / deth;
            h_UU[1][1] = (matrix[0][0]*matrix[2][2] - matrix[0][2]*matrix[0][2]) / deth;
            h_UU[1][2] = (matrix[0][1]*matrix[0][2] - matrix[0][0]*matrix[1][2]) / deth;
            h_UU[2][2] = (matrix[0][0]*matrix[1][1] - matrix[0][1]*matrix[0][1]) / deth;
            h_UU[1][0] = h_UU[0][1];
            h_UU[2][0] = h_UU[0][2];
            h_UU[2][1] = h_UU[1][2];
        }
        return h_UU;
    }


    template <class data_t>
    ALWAYS_INLINE
    static data_t
    compute_trace(const tensor<2, data_t> &tensor_LL, const tensor<2, data_t> &inverse_metric)
    {
        data_t trace = 0;
        FOR2(i,j)
        {
            trace += inverse_metric[i][j]*tensor_LL[i][j];
        }
        return trace;
    }

    template <class data_t>
    ALWAYS_INLINE
    static data_t
    compute_trace(const tensor<2, data_t> &tensor_UL)
    {
        data_t trace = 0;
        FOR2(i,j) trace += tensor_UL[i][j];
        return trace;
    }

    template <class data_t>
    ALWAYS_INLINE
    static data_t
    compute_dot_product(const tensor<1, data_t> &vector1, const tensor<1, data_t> &vector2)
    {
        data_t dot_product = 0;
        FOR2(i,j)
        {
            dot_product += vector1[i]*vector2[i];
        }
        return dot_product;
    }

    template <class data_t>
    ALWAYS_INLINE
    static void
    make_trace_free(tensor<2, data_t> &tensor_LL, const tensor<2, data_t> &metric, const tensor<2, data_t> &inverse_metric)
    {
        auto trace = compute_trace(tensor_LL, inverse_metric);
        FOR2(i,j)
        {
            tensor_LL[i][j] -= 1./( (double) GR_SPACEDIM) * metric[i][j] * trace;
        }
    }

    template <class data_t>
    ALWAYS_INLINE
    static tensor<1, data_t>
    raise_all(const tensor<1, data_t> &tensor_L, const tensor<2, data_t> &inverse_metric)
    {
        tensor<1, data_t> tensor_U = 0;
        FOR1(i)
        {
            tensor_U[i] += inverse_metric[i][j]*tensor_L[j];
        }
    }

    template <class data_t>
    ALWAYS_INLINE
    static tensor<2, data_t>
    raise_all(const tensor<2, data_t> &tensor_LL, const tensor<2, data_t> &inverse_metric)
    {
        tensor<2, data_t> tensor_UU = 0;
        FOR4(i,j,k,l)
        {
            tensor_UU[i][j] += inverse_metric[i][k]*inverse_metric[j][l]*tensor_LL[k][l];
        }
        return tensor_UU;
    }
};

#endif /* TENSORALGEBRA_HPP_ */
