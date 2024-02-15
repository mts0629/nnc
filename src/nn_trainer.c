/**
 * @file nn_trainer.c
 * @brief Train a network
 *
 */
#include "nn_trainer.h"

#include <stdlib.h>

#include "loss.h"
#include "nn_layer.h"
#include "nn_net.h"

float nn_train_step(NnNet *net, const float *x, const float *t, const float learning_rate) {
    const float *y = nn_net_forward(net, x);

    const int osize = nn_net_layers(net)[net->size - 1].out;

    float *dy = malloc(sizeof(float) * osize);

    // Get difference between the output and the label
    for (int i = 0; i < osize; i++) {
        dy[i] = y[i] - t[i];
    }

    float loss = mse_loss(y, t, osize);

    nn_net_backward(net, dy);

    nn_net_update(net, learning_rate);

    free(dy);
    dy = NULL;

    return loss;
}