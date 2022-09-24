/**
 * @file test_scnn_fc.c
 * @brief Unit tests of scnn_fc.c
 * 
 */
#include "scnn_fc.h"

#include "scnn_blas.h"

#include "unity_fixture.h"

TEST_GROUP(scnn_fc);

TEST_SETUP(scnn_fc)
{}

TEST_TEAR_DOWN(scnn_fc)
{}

TEST(scnn_fc, alloc_and_free)
{
    scnn_layer_params params = { .in = 2, .out = 10 };
    scnn_layer *fc = scnn_fc_layer(params);

    TEST_ASSERT_NOT_NULL(fc);

    TEST_ASSERT_EQUAL_INT(params.in, fc->params.in);
    TEST_ASSERT_EQUAL_INT(params.out, fc->params.out);

    TEST_ASSERT_NOT_NULL(fc->forward);
    TEST_ASSERT_NOT_NULL(fc->backward);

    TEST_ASSERT_NOT_NULL(fc->set_size);

    scnn_layer_free(&fc);

    TEST_ASSERT_NULL(fc);
}

TEST(scnn_fc, alloc_fail_invalid_param_in)
{
    scnn_layer_params params = { .in = 0, .out = 10 };
    scnn_layer *fc = scnn_fc_layer(params);

    TEST_ASSERT_NULL(fc);
}

TEST(scnn_fc, alloc_fail_invalid_param_out)
{
    scnn_layer_params params = { .in = 2, .out = 0 };
    scnn_layer *fc = scnn_fc_layer(params);

    TEST_ASSERT_NULL(fc);
}

TEST(scnn_fc, set_size)
{
    scnn_layer_params params = { .in = 2, .out = 10 };
    scnn_layer *fc = scnn_fc_layer(params);

    fc->set_size(fc, 1, 2, 1, 1);

    TEST_ASSERT_NOT_NULL(fc->x.data);
    TEST_ASSERT_EQUAL_INT(1, fc->x.n);
    TEST_ASSERT_EQUAL_INT(2, fc->x.c);
    TEST_ASSERT_EQUAL_INT(1, fc->x.h);
    TEST_ASSERT_EQUAL_INT(1, fc->x.w);
    TEST_ASSERT_EQUAL_INT(2, fc->x.size);

    TEST_ASSERT_NOT_NULL(fc->y.data);
    TEST_ASSERT_EQUAL_INT(1, fc->y.n);
    TEST_ASSERT_EQUAL_INT(10, fc->y.c);
    TEST_ASSERT_EQUAL_INT(1, fc->y.h);
    TEST_ASSERT_EQUAL_INT(1, fc->y.w);
    TEST_ASSERT_EQUAL_INT(10, fc->y.size);

    TEST_ASSERT_NOT_NULL(fc->w.data);
    TEST_ASSERT_EQUAL_INT(10, fc->w.n);
    TEST_ASSERT_EQUAL_INT(2, fc->w.c);
    TEST_ASSERT_EQUAL_INT(1, fc->w.h);
    TEST_ASSERT_EQUAL_INT(1, fc->w.w);
    TEST_ASSERT_EQUAL_INT(20, fc->w.size);

    TEST_ASSERT_NOT_NULL(fc->b.data);
    TEST_ASSERT_EQUAL_INT(1, fc->b.n);
    TEST_ASSERT_EQUAL_INT(10, fc->b.c);
    TEST_ASSERT_EQUAL_INT(1, fc->b.h);
    TEST_ASSERT_EQUAL_INT(1, fc->b.w);
    TEST_ASSERT_EQUAL_INT(10, fc->b.size);

    TEST_ASSERT_NOT_NULL(fc->dx.data);
    TEST_ASSERT_EQUAL_INT(fc->x.n, fc->dx.n);
    TEST_ASSERT_EQUAL_INT(fc->x.c, fc->dx.c);
    TEST_ASSERT_EQUAL_INT(fc->x.h, fc->dx.h);
    TEST_ASSERT_EQUAL_INT(fc->x.w, fc->dx.w);
    TEST_ASSERT_EQUAL_INT(fc->x.size, fc->dx.size);

    TEST_ASSERT_NOT_NULL(fc->dw.data);
    TEST_ASSERT_EQUAL_INT(fc->w.n, fc->dw.n);
    TEST_ASSERT_EQUAL_INT(fc->w.c, fc->dw.c);
    TEST_ASSERT_EQUAL_INT(fc->w.h, fc->dw.h);
    TEST_ASSERT_EQUAL_INT(fc->w.w, fc->dw.w);
    TEST_ASSERT_EQUAL_INT(fc->w.size, fc->dw.size);

    TEST_ASSERT_NOT_NULL(fc->db.data);
    TEST_ASSERT_EQUAL_INT(fc->b.n, fc->db.n);
    TEST_ASSERT_EQUAL_INT(fc->b.c, fc->db.c);
    TEST_ASSERT_EQUAL_INT(fc->b.h, fc->db.h);
    TEST_ASSERT_EQUAL_INT(fc->b.w, fc->db.w);
    TEST_ASSERT_EQUAL_INT(fc->b.size, fc->db.size);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, set_size_fail_invalid_n)
{
    scnn_layer_params params = { .in = 2, .out = 10 };
    scnn_layer *fc = scnn_fc_layer(params);

    fc->set_size(fc, 0, 2, 1, 1);

    TEST_ASSERT_NULL(fc->x.data);
    TEST_ASSERT_NULL(fc->y.data);
    TEST_ASSERT_NULL(fc->w.data);
    TEST_ASSERT_NULL(fc->b.data);
    TEST_ASSERT_NULL(fc->dx.data);
    TEST_ASSERT_NULL(fc->dw.data);
    TEST_ASSERT_NULL(fc->db.data);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, set_size_fail_invalid_c)
{
    scnn_layer_params params = { .in = 2, .out = 10 };
    scnn_layer *fc = scnn_fc_layer(params);

    fc->set_size(fc, 1, 0, 1, 1);

    TEST_ASSERT_NULL(fc->x.data);
    TEST_ASSERT_NULL(fc->y.data);
    TEST_ASSERT_NULL(fc->w.data);
    TEST_ASSERT_NULL(fc->b.data);
    TEST_ASSERT_NULL(fc->dx.data);
    TEST_ASSERT_NULL(fc->dw.data);
    TEST_ASSERT_NULL(fc->db.data);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, set_size_fail_invalid_h)
{
    scnn_layer_params params = { .in = 2, .out = 10 };
    scnn_layer *fc = scnn_fc_layer(params);

    fc->set_size(fc, 1, 2, 0, 1);

    TEST_ASSERT_NULL(fc->x.data);
    TEST_ASSERT_NULL(fc->y.data);
    TEST_ASSERT_NULL(fc->w.data);
    TEST_ASSERT_NULL(fc->b.data);
    TEST_ASSERT_NULL(fc->dx.data);
    TEST_ASSERT_NULL(fc->dw.data);
    TEST_ASSERT_NULL(fc->db.data);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, set_size_fail_invalid_w)
{
    scnn_layer_params params = { .in = 2, .out = 10 };
    scnn_layer *fc = scnn_fc_layer(params);

    fc->set_size(fc, 1, 2, 1, 0);

    TEST_ASSERT_NULL(fc->x.data);
    TEST_ASSERT_NULL(fc->y.data);
    TEST_ASSERT_NULL(fc->w.data);
    TEST_ASSERT_NULL(fc->b.data);
    TEST_ASSERT_NULL(fc->dx.data);
    TEST_ASSERT_NULL(fc->dw.data);
    TEST_ASSERT_NULL(fc->db.data);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, set_size_fail_invalid_in_size)
{
    scnn_layer_params params = { .in = 2, .out = 10 };
    scnn_layer *fc = scnn_fc_layer(params);

    fc->set_size(fc, 1, 2, 3, 3);

    TEST_ASSERT_NULL(fc->x.data);
    TEST_ASSERT_NULL(fc->y.data);
    TEST_ASSERT_NULL(fc->w.data);
    TEST_ASSERT_NULL(fc->b.data);
    TEST_ASSERT_NULL(fc->dx.data);
    TEST_ASSERT_NULL(fc->dw.data);
    TEST_ASSERT_NULL(fc->db.data);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, forward)
{
    scnn_layer_params params = { .in = 2, .out = 3 };
    scnn_layer *fc = scnn_fc_layer(params);
    fc->set_size(fc, 1, 2, 1, 1);

    scnn_mat x;
    scnn_mat_init(&x, 1, 2, 1, 1);
    scnn_mat_copy_from_array(&x,
        (float[]){
            1, 1
        },
        fc->x.size);

    scnn_mat_copy_from_array(&fc->w,
        (float[]){
            0, 1, 2,
            3, 4, 5
        },
        fc->w.size);

    scnn_mat_copy_from_array(&fc->b,
        (float[]){
            1, 1, 1
        },
        fc->b.size);

    fc->forward(fc, &x);

    float answer[] = {
        4, 6, 8
    };

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(answer, fc->y.data, 3);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, forward_fail_x_is_null)
{
    scnn_layer_params params = { .in = 2, .out = 3 };
    scnn_layer *fc = scnn_fc_layer(params);
    fc->set_size(fc, 1, 2, 1, 1);

    scnn_mat_copy_from_array(&fc->w,
        (float[]){
            0, 1, 2,
            3, 4, 5
        },
        fc->w.size);

    scnn_mat_copy_from_array(&fc->b,
        (float[]){
            1, 1, 1
        },
        fc->b.size);

    float init[] = {
        0, 0, 0
    };

    scnn_mat_copy_from_array(&fc->y,
        init,
        fc->y.size);

    fc->forward(fc, NULL);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(init, fc->y.data, fc->y.size);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, forward_fail_layer_is_null)
{
    scnn_layer_params params = { .in = 2, .out = 3 };
    scnn_layer *fc = scnn_fc_layer(params);
    fc->set_size(fc, 1, 2, 1, 1);

    scnn_mat x;
    scnn_mat_init(&x, 1, 2, 1, 1);
    scnn_mat_copy_from_array(&x,
        (float[]){
            1, 1
        },
        fc->x.size);

    scnn_mat_copy_from_array(&fc->w,
        (float[]){
            0, 1, 2,
            3, 4, 5
        },
        fc->w.size);

    scnn_mat_copy_from_array(&fc->b,
        (float[]){
            1, 1, 1
        },
        fc->b.size);

    float init[] = {
        0, 0, 0
    };

    scnn_mat_copy_from_array(&fc->y,
        init,
        fc->y.size);

    fc->forward(NULL, &x);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(init, fc->y.data, fc->y.size);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, backward)
{
    scnn_layer_params params = { .in = 2, .out = 3 };
    scnn_layer *fc = scnn_fc_layer(params);
    fc->set_size(fc, 1, 2, 1, 1);

    scnn_mat x;
    scnn_mat_init(&x, 1, 2, 1, 1);
    scnn_mat_copy_from_array(&fc->x,
        (float[]){
            1, 2
        },
        fc->x.size);

    scnn_mat_copy_from_array(&fc->w,
        (float[]){
            0, 1, 2,
            3, 4, 5
        },
        fc->w.size);

    scnn_mat_copy_from_array(&fc->b,
        (float[]){
            1, 1, 1
        },
        fc->b.size);

    scnn_mat dy;
    scnn_mat_init(&dy, 1, 3, 1, 1);
    scnn_mat_copy_from_array(&dy,
        (float[]){
            8, 12, 16
        },
        dy.size);

    fc->backward(fc, &dy);

    float answer_dx[] = {
        44, 152
    };
    float answer_dw[] = {
        8,  12, 16,
        16, 24, 32
    };
    float answer_db[] = {
        8, 12, 16
    };

    //printf("\ndw\n");
    //for (int i = 0; i < fc->dw.n; i++) {
    //    for (int j = 0; j < fc->dw.c; j++) {
    //        printf("%f,", fc->dw.data[i * fc->dw.c + j]);
    //    }
    //    printf("\n");
    //}

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(answer_dx, fc->dx.data, fc->dx.size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(answer_dw, fc->dw.data, fc->dw.size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(answer_db, fc->db.data, fc->db.size);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, backward_fail_dy_is_null)
{
    scnn_layer_params params = { .in = 2, .out = 3 };
    scnn_layer *fc = scnn_fc_layer(params);
    fc->set_size(fc, 1, 2, 1, 1);

    scnn_mat x;
    scnn_mat_init(&x, 1, 2, 1, 1);
    scnn_mat_copy_from_array(&x,
        (float[]){
            1, 1
        },
        fc->x.size);

    scnn_mat_copy_from_array(&fc->w,
        (float[]){
            0, 1, 2,
            3, 4, 5
        },
        fc->w.size);

    scnn_mat_copy_from_array(&fc->b,
        (float[]){
            1, 1, 1
        },
        fc->b.size);

    scnn_mat_fill(&fc->dx, 0);
    scnn_mat_fill(&fc->dw, 0);
    scnn_mat_fill(&fc->db, 0);

    fc->backward(fc, NULL);

    TEST_ASSERT_EACH_EQUAL_FLOAT(0, fc->dx.data, fc->dx.size);
    TEST_ASSERT_EACH_EQUAL_FLOAT(0, fc->dw.data, fc->dw.size);
    TEST_ASSERT_EACH_EQUAL_FLOAT(0, fc->db.data, fc->db.size);

    scnn_layer_free(&fc);
}

TEST(scnn_fc, backward_fail_layer_is_null)
{
    scnn_layer_params params = { .in = 2, .out = 3 };
    scnn_layer *fc = scnn_fc_layer(params);
    fc->set_size(fc, 1, 2, 1, 1);

    scnn_mat x;
    scnn_mat_init(&x, 1, 2, 1, 1);
    scnn_mat_copy_from_array(&x,
        (float[]){
            1, 1
        },
        fc->x.size);

    scnn_mat_copy_from_array(&fc->w,
        (float[]){
            0, 1, 2,
            3, 4, 5
        },
        fc->w.size);

    scnn_mat_copy_from_array(&fc->b,
        (float[]){
            1, 1, 1
        },
        fc->b.size);

    scnn_mat dy;
    scnn_mat_init(&dy, 1, 3, 1, 1);
    scnn_mat_copy_from_array(&dy,
        (float[]){
            8, 12, 16
        },
        dy.size);

    scnn_mat_fill(&fc->dx, 0);
    scnn_mat_fill(&fc->dw, 0);
    scnn_mat_fill(&fc->db, 0);

    fc->backward(NULL, &dy);

    TEST_ASSERT_EACH_EQUAL_FLOAT(0, fc->dx.data, fc->dx.size);
    TEST_ASSERT_EACH_EQUAL_FLOAT(0, fc->dw.data, fc->dw.size);
    TEST_ASSERT_EACH_EQUAL_FLOAT(0, fc->db.data, fc->db.size);

    scnn_layer_free(&fc);
}