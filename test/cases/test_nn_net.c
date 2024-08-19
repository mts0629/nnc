/**
 * @file test_nn_net.c
 * @brief Unit tests of nn_net.c
 *
 */
#include "nn_net.h"

#include "mock_nn_layer.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_allocate_and_free_layer(void) {
    NnNet net;

    NnLayer dummy_layer;
    // Function `nn_layer_alloc` call isn't fully tested
    // arguments are not verified because of using malloc internally
    nn_layer_alloc_params_ExpectAnyArgsAndReturn(&dummy_layer);
    TEST_ASSERT_EQUAL_PTR(
        &net,
        nn_net_alloc_layers(
            &net,
            (NnLayerParams[]){
                // Test with an identity layer
                { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
                {}
            }
        )
    );

    TEST_ASSERT_EQUAL_INT(1, nn_net_size(&net));

    TEST_ASSERT_NOT_NULL(nn_net_layers(&net));

    NnLayer *layer = &nn_net_layers(&net)[0];
    TEST_ASSERT_EQUAL_PTR(layer, nn_net_input(&net));
    TEST_ASSERT_EQUAL_PTR(layer, nn_net_output(&net));

    TEST_ASSERT_EQUAL_INT(NN_LAYER_TYPE_IDENTITY, layer->params.type);
    TEST_ASSERT_EQUAL_INT(1, layer->params.batch_size);
    TEST_ASSERT_EQUAL_INT(2, layer->params.in);
    TEST_ASSERT_EQUAL_INT(2, layer->params.out);

    // Same with `nn_layer_alloc_params`,
    // `nn_layer_free_params` is not fully tested
    nn_layer_free_params_ExpectAnyArgs();
    nn_net_free_layers(&net);
    TEST_ASSERT_NULL(net.layers);
}

void test_allocate_and_free_3layers(void) {
    NnNet net;

    NnLayerParams layer_params[] = {
        { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
        { NN_LAYER_TYPE_IDENTITY },
        { NN_LAYER_TYPE_IDENTITY },
        {}
    };

    NnLayer dummy_layer;
    nn_layer_alloc_params_ExpectAnyArgsAndReturn(&dummy_layer);
    nn_layer_connect_ExpectAnyArgs();
    nn_layer_alloc_params_ExpectAnyArgsAndReturn(&dummy_layer);
    nn_layer_connect_ExpectAnyArgs();
    nn_layer_alloc_params_ExpectAnyArgsAndReturn(&dummy_layer);
    TEST_ASSERT_EQUAL_PTR(&net, nn_net_alloc_layers(&net, layer_params));

    TEST_ASSERT_EQUAL_INT(3, nn_net_size(&net));

    TEST_ASSERT_EQUAL_PTR(&nn_net_layers(&net)[0], nn_net_input(&net));
    TEST_ASSERT_EQUAL_PTR(&nn_net_layers(&net)[2], nn_net_output(&net));

    for (int i = 0; i < 3; i++) {
        NnLayer *layer = &nn_net_layers(&net)[i];
        TEST_ASSERT_EQUAL_INT(layer_params[i].type, layer->params.type);
        TEST_ASSERT_EQUAL_INT(
            layer_params[i].batch_size, layer->params.batch_size
        );
        TEST_ASSERT_EQUAL_INT(layer_params[i].in, layer->params.in);
        TEST_ASSERT_EQUAL_INT(layer_params[i].out, layer->params.out);
    }

    nn_layer_free_params_ExpectAnyArgs();
    nn_layer_free_params_ExpectAnyArgs();
    nn_layer_free_params_ExpectAnyArgs();
    nn_net_free_layers(&net);
    TEST_ASSERT_NULL(net.layers);
}

void test_allocation_fail_if_net_is_NULL(void) {
    TEST_ASSERT_NULL(
        nn_net_alloc_layers(
            NULL,
            (NnLayerParams[]){
                { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
                {}
            }
        )
    );
}

void test_allocation_fail_if_param_list_is_NULL(void) {
    NnNet net;
    TEST_ASSERT_NULL(nn_net_alloc_layers(&net, NULL));
}

void test_allocation_fail_if_layer_parameter_is_invalid(void) {
    NnNet net;
    TEST_ASSERT_NULL(nn_net_alloc_layers(&net, (NnLayerParams[]){ {} }));
}

void test_free_layers_for_NULL(void) {
    NnNet *net = NULL;
    nn_net_free_layers(net);
}

void test_free_layers_when_layers_are_NULL(void) {
    NnNet net = { .layers=NULL };
    nn_net_free_layers(&net);
}

void test_forward_layer(void) {
    NnNet net;

    nn_layer_connect_Ignore();
    NnLayer dummy_layer;
    nn_layer_alloc_params_IgnoreAndReturn(&dummy_layer);
    nn_net_alloc_layers(
        &net,
        (NnLayerParams[]){
            { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
            {}
        }
    );

    float dummy_x, dummy_y;
    nn_layer_forward_ExpectAndReturn(
        &nn_net_layers(&net)[0], &dummy_x, &dummy_y
    );
    TEST_ASSERT_EQUAL_PTR(&dummy_y, nn_net_forward(&net, &dummy_x));

    nn_layer_free_params_Ignore();
    nn_net_free_layers(&net);
}

void test_forward_3layers(void) {
    NnNet net;

    nn_layer_connect_Ignore();
    NnLayer dummy_layer;
    nn_layer_alloc_params_IgnoreAndReturn(&dummy_layer);
    nn_net_alloc_layers(
        &net,
        (NnLayerParams[]){
            { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
            { NN_LAYER_TYPE_IDENTITY },
            { NN_LAYER_TYPE_IDENTITY },
            {}
        }
    );

    float dummy_x, dummy_y[3];
    nn_layer_forward_ExpectAndReturn(
        &nn_net_layers(&net)[0], &dummy_x, &dummy_y[0]
    );
    nn_layer_forward_ExpectAndReturn(
        &nn_net_layers(&net)[1], &dummy_y[0], &dummy_y[1]
    );
    nn_layer_forward_ExpectAndReturn(
        &nn_net_layers(&net)[2], &dummy_y[1], &dummy_y[2]
    );
    TEST_ASSERT_EQUAL_PTR(&dummy_y[2], nn_net_forward(&net, &dummy_x));

    nn_layer_free_params_Ignore();
    nn_net_free_layers(&net);
}

void test_forward_fail_if_net_is_NULL(void) {
    float dummy_x;
    TEST_ASSERT_NULL(nn_net_forward(NULL, &dummy_x));
}

void test_forward_fail_if_x_is_NULL(void) {
    NnNet net;

    nn_layer_connect_Ignore();
    NnLayer dummy_layer;
    nn_layer_alloc_params_IgnoreAndReturn(&dummy_layer);
    nn_net_alloc_layers(
        &net,
        (NnLayerParams[]){
            { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
            {}
        }
    );

    TEST_ASSERT_NULL(nn_net_forward(&net, NULL));

    nn_layer_free_params_Ignore();
    nn_net_free_layers(&net);
}

void test_backward_layer(void) {
    NnNet net;

    nn_layer_connect_Ignore();
    NnLayer dummy_layer;
    nn_layer_alloc_params_IgnoreAndReturn(&dummy_layer);
    nn_net_alloc_layers(
        &net,
        (NnLayerParams[]){
            { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
            {}
        }
    );

    float dummy_dy, dummy_dx;
    nn_layer_backward_ExpectAndReturn(
        &nn_net_layers(&net)[0], &dummy_dy, &dummy_dx
    );
    TEST_ASSERT_EQUAL_PTR(&dummy_dx, nn_net_backward(&net, &dummy_dy));

    nn_layer_free_params_Ignore();
    nn_net_free_layers(&net);
}

void test_backward_3layer(void) {
    NnNet net;

    nn_layer_connect_Ignore();
    NnLayer dummy_layer;
    nn_layer_alloc_params_IgnoreAndReturn(&dummy_layer);
    nn_net_alloc_layers(
        &net,
        (NnLayerParams[]){
            { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
            { NN_LAYER_TYPE_IDENTITY },
            { NN_LAYER_TYPE_IDENTITY },
            {}
        }
    );

    float dummy_dy, dummy_dx[3];
    nn_layer_backward_ExpectAndReturn(
        &nn_net_layers(&net)[2], &dummy_dy, &dummy_dx[2]
    );
    nn_layer_backward_ExpectAndReturn(
        &nn_net_layers(&net)[1], &dummy_dx[2], &dummy_dx[1]
    );
    nn_layer_backward_ExpectAndReturn(
        &nn_net_layers(&net)[0], &dummy_dx[1], &dummy_dx[0]
    );
    TEST_ASSERT_EQUAL_PTR(&dummy_dx[0], nn_net_backward(&net, &dummy_dy));

    nn_layer_free_params_Ignore();
    nn_net_free_layers(&net);
}

void test_backward_fail_if_net_is_NULL(void) {
    float dummy_dy;
    TEST_ASSERT_NULL(nn_net_backward(NULL, &dummy_dy));
}

void test_backward_fail_if_dy_is_NULL(void) {
    NnNet net;

    nn_layer_connect_Ignore();
    NnLayer dummy_layer;
    nn_layer_alloc_params_IgnoreAndReturn(&dummy_layer);
    nn_net_alloc_layers(
        &net,
        (NnLayerParams[]){
            { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
            {}
        }
    );

    TEST_ASSERT_NULL(nn_net_backward(&net, NULL));

    nn_layer_free_params_Ignore();
    nn_net_free_layers(&net);
}

void test_clear_grad(void) {
    NnNet net;

    nn_layer_connect_Ignore();
    NnLayer dummy_layer;
    nn_layer_alloc_params_IgnoreAndReturn(&dummy_layer);
    nn_net_alloc_layers(
        &net,
        (NnLayerParams[]){
            { NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=2 },
            { NN_LAYER_TYPE_IDENTITY },
            { NN_LAYER_TYPE_IDENTITY },
            {}
        }
    );

    nn_layer_clear_grad_Expect(&nn_net_layers(&net)[0]);
    nn_layer_clear_grad_Expect(&nn_net_layers(&net)[1]);
    nn_layer_clear_grad_Expect(&nn_net_layers(&net)[2]);
    nn_net_clear_grad(&net);

    nn_layer_free_params_Ignore();
    nn_net_free_layers(&net);
}
