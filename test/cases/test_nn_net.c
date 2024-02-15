/**
 * @file test_nn_net.c
 * @brief Unit tests of nn_net.c
 *
 */
#include "nn_net.h"

#include "unity.h"

#include "mock_nn_layer.h"

NnNet *net;

NnLayerParams dummy_layer_params[] = {
    { .batch_size = 1, .in = 3 * 28 * 28, .out = 100 },
    { .out = 10 },
    { .out = 2 }
};

float dummy_x[3 * 28 * 28];
float dummy_y0[100];
float dummy_y1[10];
float dummy_y2[2];

float dummy_dx0[3 * 28 * 28];
float dummy_dx1[100];
float dummy_dx2[10];
float dummy_dy[2];

void setUp(void) {
    net = NULL;
}

void tearDown(void) {}

void test_allocate_and_free(void) {
    net = nn_net_alloc();

    TEST_ASSERT_NOT_NULL(net);

    TEST_ASSERT_EQUAL_INT(0, nn_net_size(net));

    TEST_ASSERT_NULL(nn_net_layers(net));

    TEST_ASSERT_NULL(nn_net_input(net));
    TEST_ASSERT_NULL(nn_net_output(net));

    nn_net_free(&net);
    TEST_ASSERT_NULL(net);
}

void test_free_pointer_to_NULL(void) {
    nn_net_free(&net);
}

void test_free_NULL(void) {
    nn_net_free(NULL);
}

void test_append_layer(void) {
    net = nn_net_alloc();

    TEST_ASSERT_EQUAL_PTR(net, nn_net_append(net, dummy_layer_params[0]));

    TEST_ASSERT_EQUAL_INT(1, nn_net_size(net));

    TEST_ASSERT_NOT_NULL(nn_net_layers(net));

    TEST_ASSERT_EQUAL_INT(dummy_layer_params[0].batch_size, nn_net_layers(net)[0].batch_size);
    TEST_ASSERT_EQUAL_INT(dummy_layer_params[0].in, nn_net_layers(net)[0].in);
    TEST_ASSERT_EQUAL_INT(dummy_layer_params[0].out, nn_net_layers(net)[0].out);

    TEST_ASSERT_EQUAL_PTR(&nn_net_layers(net)[0], nn_net_input(net));
    TEST_ASSERT_EQUAL_PTR(&nn_net_layers(net)[0], nn_net_output(net));

    nn_net_free(&net);
    TEST_ASSERT_NULL(net);
}

void test_append_3layers(void) {
    net = nn_net_alloc();

    for (int i = 0; i < 3; i++) {
        nn_layer_connect_ExpectAnyArgs();
        TEST_ASSERT_EQUAL_PTR(net, nn_net_append(net, dummy_layer_params[i]));
    }

    TEST_ASSERT_EQUAL_INT(3, nn_net_size(net));

    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_INT(dummy_layer_params[0].batch_size, nn_net_layers(net)[0].batch_size);
        TEST_ASSERT_EQUAL_INT(dummy_layer_params[i].in, nn_net_layers(net)[i].in);
        TEST_ASSERT_EQUAL_INT(dummy_layer_params[i].out, nn_net_layers(net)[i].out);
     }

    TEST_ASSERT_EQUAL_PTR(&nn_net_layers(net)[0], nn_net_input(net));
    TEST_ASSERT_EQUAL_PTR(&nn_net_layers(net)[2], nn_net_output(net));

    nn_net_free(&net);
    TEST_ASSERT_NULL(net);
}

void test_append_fail_if_net_is_NULL(void) {
    TEST_ASSERT_NULL(nn_net_append(NULL, dummy_layer_params[0]));
}

void test_init(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    nn_net_append(net, dummy_layer_params[0]);

    nn_layer_init_ExpectAndReturn(&nn_net_layers(net)[0], &nn_net_layers(net)[0]);
    TEST_ASSERT_EQUAL_PTR(net, nn_net_init(net));

    nn_net_free(&net);
}

void test_init_3layers(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    for (int i = 0; i < 3; i++) {
        nn_net_append(net, dummy_layer_params[i]);
    }

    for (int i = 0; i < 3; i++) {
        nn_layer_init_ExpectAndReturn(&nn_net_layers(net)[i], &nn_net_layers(net)[i]);
    }
    TEST_ASSERT_EQUAL_PTR(net, nn_net_init(net));

    nn_net_free(&net);
}

void test_init_fail_if_net_is_NULL(void) {
    TEST_ASSERT_NULL(nn_net_init(NULL));
}

void test_init_fail_if_net_size_is_0(void) {
    net = nn_net_alloc();

    TEST_ASSERT_NULL(nn_net_init(net));

    nn_net_free(&net);
}

void test_init_fail_if_layer_init_fail(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    for (int i = 0; i < 3; i++) {
        nn_net_append(net, dummy_layer_params[i]);
    }

    nn_layer_init_ExpectAndReturn(&nn_net_layers(net)[0], &nn_net_layers(net)[0]);
    nn_layer_init_ExpectAndReturn(&nn_net_layers(net)[1], NULL);
    TEST_ASSERT_NULL(nn_net_init(net));

    nn_net_free(&net);
}

void test_forward_1layer(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    nn_net_append(net, dummy_layer_params[0]);

    nn_layer_init_IgnoreAndReturn(&nn_net_layers(net)[0]);
    nn_net_init(net);

    nn_layer_forward_ExpectAndReturn(&nn_net_layers(net)[0], dummy_x, dummy_y0);
    TEST_ASSERT_EQUAL_PTR(dummy_y0, nn_net_forward(net, dummy_x));

    nn_net_free(&net);
}

void test_forward_3layer(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    for (int i = 0; i < 3; i++) {
        nn_net_append(net, dummy_layer_params[i]);
    }

    for (int i = 0; i < 3; i++) {
        nn_layer_init_IgnoreAndReturn(&nn_net_layers(net)[i]);
    }
    nn_net_init(net);

    nn_layer_forward_ExpectAndReturn(&nn_net_layers(net)[0], dummy_x, dummy_y0);
    nn_layer_forward_ExpectAndReturn(&nn_net_layers(net)[1], dummy_y0, dummy_y1);
    nn_layer_forward_ExpectAndReturn(&nn_net_layers(net)[2], dummy_y1, dummy_y2);
    TEST_ASSERT_EQUAL_PTR(dummy_y2, nn_net_forward(net, dummy_x));

    nn_net_free(&net);
}

void test_forward_fail_if_net_is_NULL(void) {
    TEST_ASSERT_NULL(nn_net_forward(NULL, dummy_x));
}

void test_forward_fail_if_x_is_NULL(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    nn_net_append(net, dummy_layer_params[0]);

    nn_layer_init_IgnoreAndReturn(&nn_net_layers(net)[0]);
    nn_net_init(net);

    TEST_ASSERT_NULL(nn_net_forward(net, NULL));

    nn_net_free(&net);
}

void test_backward_1layer(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    nn_net_append(net, dummy_layer_params[0]);

    nn_layer_backward_ExpectAndReturn(&nn_net_layers(net)[0], dummy_dy, dummy_dx0);
    TEST_ASSERT_EQUAL_PTR(dummy_dx0, nn_net_backward(net, dummy_dy));

    nn_net_free(&net);
}

void test_backward_3layer(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    for (int i = 0; i < 3; i++) {
        nn_net_append(net, dummy_layer_params[i]);
    }

    nn_layer_backward_ExpectAndReturn(&nn_net_layers(net)[2], dummy_dy, dummy_dx2);
    nn_layer_backward_ExpectAndReturn(&nn_net_layers(net)[1], dummy_dx2, dummy_dx1);
    nn_layer_backward_ExpectAndReturn(&nn_net_layers(net)[0], dummy_dx1, dummy_dx0);
    TEST_ASSERT_EQUAL_PTR(dummy_dx0, nn_net_backward(net, dummy_dy));

    nn_net_free(&net);
}

void test_backward_fail_if_net_is_NULL(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    nn_net_append(net, dummy_layer_params[0]);

    TEST_ASSERT_NULL(nn_net_backward(NULL, dummy_dy));

    nn_net_free(&net);
}

void test_backward_fail_if_dy_is_NULL(void) {
    net = nn_net_alloc();

    nn_layer_connect_Ignore();
    nn_net_append(net, dummy_layer_params[0]);

    TEST_ASSERT_NULL(nn_net_backward(net, NULL));

    nn_net_free(&net);
}

void test_update(void) {
    net = nn_net_alloc();
    nn_layer_connect_Ignore();
    nn_net_append(net, dummy_layer_params[0]);
    nn_layer_connect_Ignore();
    nn_net_append(net, dummy_layer_params[1]);

    nn_layer_init_IgnoreAndReturn(&nn_net_layers(net)[0]);
    nn_layer_init_IgnoreAndReturn(&nn_net_layers(net)[1]);
    nn_net_init(net);

    nn_layer_update_Expect(&net->layers[0], 0.01);
    nn_layer_update_Expect(&net->layers[1], 0.01);
    nn_net_update(net, 0.01);

    nn_net_free(&net);
}